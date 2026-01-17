#include <atomic>
#include <thread>
#include <vector>
#include "random.h"
#include "print.h"
#include "stopwatch.h"
#include "numa_array.h"

struct alignas(std::hardware_destructive_interference_size)
	AtomicInt : std::atomic<int> {};

std::array<AtomicInt, 6000> arr;
numa::Array<AtomicInt, 3000> narr1{ 0 };
numa::Array<AtomicInt, 3000> narr2{ 1 };
numa::Array<AtomicInt, 3000> narr3{ 0 };

const auto kNumThread{ static_cast<int>(std::thread::hardware_concurrency()) };
constexpr auto kLoop{ 360'000'000 };

void ThreadFuncA(int thread_id)
{
	for (int i = 0; i < kLoop / kNumThread; ++i) {
		arr[Random::Get(0, arr.size() - 1)] += 1;
	}
}

void ThreadFuncB(int thread_id)
{
	std::array<numa::Array<AtomicInt, 3000>*, 2> arrs{ &narr1, &narr2 };

	numa::SetCPUAffinity(thread_id);

	auto& arr{ *arrs[(thread_id / 10) % 2] };

	for (int i = 0; i < kLoop / kNumThread; ++i) {
		arr[Random::Get(0, arr.Get().size() - 1)] += 1;
	}
}

void ThreadFuncC(int thread_id)
{
	std::array<numa::Array<AtomicInt, 3000>*, 2> arrs{ &narr1, &narr3 };

	numa::SetCPUAffinity(thread_id);

	auto& arr{ *arrs[(thread_id / 10) % 2] };

	for (int i = 0; i < kLoop / kNumThread; ++i) {
		arr[Random::Get(0, arr.Get().size() - 1)] += 1;
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
		auto r = Random::Get(0, 4);
		auto id = Random::Get(0, 5999);

		switch (r) {
			case 0:
				compat::Print("{} ", arr[id].load());
				break;
			case 1:
				compat::Print("{} ", narr1[id / 2].load());
				break;
			case 2:
				compat::Print("{} ", narr2[id / 2].load());
				break;
			case 3:
				compat::Print("{} ", narr3[id / 2].load());
				break;
		}
	}
	compat::Print("\n");
}