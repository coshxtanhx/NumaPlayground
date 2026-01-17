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
}