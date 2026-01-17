#pragma once
// The operating system and NUMA are unrelated, but this was done temporarily.
#ifdef __linux__
#include <numa.h>
#include <numaif.h>
#define IS_NUMA 1
#else
#define IS_NUMA 0
#endif

namespace numa {
	void SetCPUAffinity(int cpu)
	{
#if IS_NUMA
		pthread_t pthread_id = pthread_self();
		cpu_set_t cpus;
		CPU_ZERO(&cpus);
		CPU_SET(cpu, &cpus);
		pthread_setaffinity_np(pthread_id, sizeof(cpu_set_t), &cpus);
#endif
	}

	template<class T>
	class Ptr {
	public:
		Ptr() noexcept = default;

		Ptr(int numa_node) {
			Init(numa_node);
		}

		Ptr(const Ptr& other) = delete;

		Ptr(Ptr&& other) noexcept : ptr_{ other.ptr_ } {
			other.ptr_ = nullptr;
		}

		auto& operator=(const Ptr& other) = delete;

		auto& operator=(Ptr&& other) noexcept {
			if (this == &other) {
				return *this;
			}

			ptr_ = other.ptr_;
			other.ptr_ = nullptr;
			return *this;
		}

		~Ptr() {
			if (ptr_) {
#if IS_NUMA
				numa_free(ptr, sizeof(T));
#else
				delete ptr_;
#endif
			}
		}

		void Init(int numa_node) {
#if IS_NUMA
			ptr_ = reinterpret_cast<T*>(numa_alloc_onnode(sizeof(T), numa_node));
#else
			ptr_ = new T;
#endif
		}

		auto Get() const {
			return ptr_;
		}

		auto& operator*() const {
			return *Get();
		}

		auto operator->() const {
			return Get();
		}

	private:
		T* ptr_{};
	};
}