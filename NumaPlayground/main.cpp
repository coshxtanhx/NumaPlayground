#include <atomic>
#include <thread>
#include <array>
#include <vector>
#include <memory>
#include "random.h"
#include "print.h"
#include "stopwatch.h"
#include "my_numa.h"

struct alignas(std::hardware_destructive_interference_size)
	AtomicInt : std::atomic<int> {};

constexpr auto kArrSize{ 2'000'000 };

std::vector<AtomicInt> vec(kArrSize);

using NumaArrPtr = numa::Ptr<std::array<AtomicInt, kArrSize / 2>>;
NumaArrPtr narr1{ 0 };
NumaArrPtr narr2{ 1 };
NumaArrPtr narr3{ 0 };

const auto kNumThread{ static_cast<int>(std::thread::hardware_concurrency()) };
constexpr auto kLoop{ 360'000'000 };

void ThreadFuncA(int thread_id)
{
	for (int i = 0; i < kLoop / kNumThread; ++i) {
		vec[Random::Get(0, vec.size() - 1)] += 1;
	}
}

void ThreadFuncB(int thread_id)
{
	std::array<NumaArrPtr*, 2> arrs{ &narr1, &narr2 };

	numa::SetCPUAffinity(thread_id);

#if IS_NUMA
	auto& arr{ *arrs[(thread_id / 10) % 2] };
#else
	auto& arr{ *arrs[thread_id % 2] };
#endif

	for (int i = 0; i < kLoop / kNumThread; ++i) {
		(*arr)[Random::Get(0, arr->size() - 1)] += 1;
	}
}

void ThreadFuncC(int thread_id)
{
	std::array<NumaArrPtr*, 2> arrs{ &narr1, &narr3 };

	numa::SetCPUAffinity(thread_id);

#if IS_NUMA
	auto& arr{ *arrs[(thread_id / 10) % 2] };
#else
	auto& arr{ *arrs[thread_id % 2] };
#endif

	for (int i = 0; i < kLoop / kNumThread; ++i) {
		(*arr)[Random::Get(0, arr->size() - 1)] += 1;
	}
}

int main()
{	
	std::vector<std::thread> threads;
	threads.reserve(40);

	Stopwatch stopwatch;
	stopwatch.Start();

	for (int i = 0; i < kNumThread; ++i) {
		threads.emplace_back(ThreadFuncA, i);
	}

	for (auto& thread : threads) {
		thread.join();
	}

	compat::Print("{} sec\n", stopwatch.GetDuration());

	threads.clear();

	stopwatch.Start();

	for (int i = 0; i < kNumThread; ++i) {
		threads.emplace_back(ThreadFuncB, i);
	}

	for (auto& thread : threads) {
		thread.join();
	}

	compat::Print("{} sec\n", stopwatch.GetDuration());

	threads.clear();

	stopwatch.Start();

	for (int i = 0; i < kNumThread; ++i) {
		threads.emplace_back(ThreadFuncC, i);
	}

	for (auto& thread : threads) {
		thread.join();
	}

	compat::Print("{} sec\n", stopwatch.GetDuration());

	compat::Print("\n");

	for (int i = 0; i < 5; ++i) {
		auto r = Random::Get(0, 3);
		auto id = Random::Get(0, kArrSize - 1);

		switch (r) {
			case 0:
				compat::Print("{} ", vec[id].load());
				break;
			case 1:
				compat::Print("{} ", (*narr1)[id / 2].load());
				break;
			case 2:
				compat::Print("{} ", (*narr2)[id / 2].load());
				break;
			case 3:
				compat::Print("{} ", (*narr3)[id / 2].load());
				break;
		}
	}
	compat::Print("\n");
	compat::Print("Press Enter Key to Quit.\n");
	getchar();
}