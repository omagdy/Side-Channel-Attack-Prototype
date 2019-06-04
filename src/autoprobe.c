#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "time.h"
#include <string.h>
#include "probe.h"
#include <unistd.h>

#define TIME_LOOP_THRESHOLD (10000000000) 	// Microseconds
#define LOOP_WAIT (5)						// Microseconds


void task_3(){
	uint64_t pr_i_time;
	int to_be_printed=0;
	int to_be_printed_elems[10];
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	uint64_t delta_us;
	int tsc_threshold;
	int time_flag=1;
	while(1){
		tsc_threshold = determing_tsc_threshold();
		while(time_flag){

			for(int i=1; i<11; i++){
				pr_i_time = reload(numbers[i]);
				if(pr_i_time < tsc_threshold){
					to_be_printed++;
					to_be_printed_elems[to_be_printed-1] =i-1;
				}
				flush(numbers[i]);
			}
			
			if(to_be_printed!=0){
				printf("Access:\t");
				for(int i=0; i<to_be_printed;i++){
					printf("[%d],  ", to_be_printed_elems[i]);
					}
					printf("\n");
			}
			to_be_printed=0;
			memset(to_be_printed_elems, 0, sizeof(to_be_printed_elems));

			clock_gettime(CLOCK_MONOTONIC_RAW, &end);
			delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000; //microseconds
			if(delta_us>TIME_LOOP_THRESHOLD){
				time_flag=0;
		}
	}
	time_flag=1;
	usleep(LOOP_WAIT);
	}
}


int main() {

	task_3();

}
