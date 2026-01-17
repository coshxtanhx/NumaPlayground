#pragma once
#include <array>
#include "my_numa.h"

namespace numa {
	template<class T, size_t kSize>
	class Array {
	public:
		using ArrayT = std::array<T, kSize>;

		Array() = default;

		Array(int numa_node) {
			Init(numa_node);
		}

		~Array() {
		#if IS_NUMA
			numa_free(arr_, sizeof(ArrayT));
		#else
			delete arr_;
		#endif
		}

		void Init(int numa_node) {
		#if IS_NUMA
			arr_ = static_cast<void*>(numa_alloc_onnode(sizeof(ArrayT), numa_node));
		#else
			arr_ = new ArrayT;
		#endif
		}

		auto& Get() const {
			return *arr_;
		}

		auto& operator[](size_t id) const {
			return (*arr_)[id];
		}

	private:
		ArrayT* arr_{};
	};
}