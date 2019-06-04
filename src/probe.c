#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "time.h"
#include <string.h>
#include "probe.h"
#include <unistd.h>

#define ITERATION_COUNT (1000) 				// Used for part 2a
#define TIME_LOOP_THRESHOLD (10000000000)	// Microseconds
#define LOOP_WAIT (5)						// Microseconds


void print_results_to_file(char *nme, uint64_t *arr){
	int j=0;
	char snum[50];
	FILE *fp;
	fp = fopen(nme, "w+");
    for(j = 0; j < ITERATION_COUNT; j++) {
		sprintf(snum, "%ld",arr[j]);
		strcat(snum, " ");
		fputs(snum, fp);
    }
    fputs("\n", fp);
    for(j = 0; j < ITERATION_COUNT; j++) {
		sprintf(snum, "%d",j+1);
		strcat(snum, " ");
    	fputs(snum, fp);
    }
    fclose(fp);
}


void task_2_a(){

	uint64_t pr_5_time[ITERATION_COUNT];
	uint64_t pr_7_time[ITERATION_COUNT];
	// ptr5 = (char *)malloc(sizeof(numbers[5]));
	// ptr5 = numbers[5];
	// pr_7_time[i] = reload(ptr7);
	// ptr7 = (char *)malloc(sizeof(numbers[7]));
	// ptr7 = numbers[7];

	for(int i=0; i<ITERATION_COUNT; i++){
		pr_7_time[i] = reload(numbers[7]);
		pr_5_time[i] = reload(numbers[5]);
		flush(numbers[7]);
		flush(numbers[5]);
		usleep(2);
	}
	print_results_to_file("Probe-numbers[5].csv", pr_5_time);
	print_results_to_file("Probe-numbers[7].csv", pr_7_time);
	return;
}

// void task_2_b_old(){
// 	// struct timespec tim, tim2;
// 	// tim.tv_sec  = 1;
// 	// tim.tv_nsec = 000000000L;
// 	uint64_t pr_i_time, tsc_threshold;
// 	tsc_threshold = determing_tsc_threshold();
// 	while(1){

// 		for(int i=1; i<11; i++){
// 			pr_i_time = reload(numbers[i]);
// 			if(pr_i_time < tsc_threshold){
// 				printf("Accessed numbers[%d]: %ld cycles \n", i, pr_i_time);
// 				// printf("Access\n");
// 			}
// 			// printf("T for numbers[%d]:%ld\n", i, pr_i_time);
// 			flush(numbers[i]);	
// 		}
// 		// usleep(100);

// 		// for(int i=0; i<12; i++){
// 		// 	flush(numbers[i]);
// 		// }
// 			// printf("\n");
// 			// nanosleep(&tim , &tim2);
// 			// usleep(1);
// 		}
// }

void task_2_b(){
	uint64_t pr_i_time;
	int tsc_threshold;
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	uint64_t delta_us;
	int time_flag=1;
	while(1){
		tsc_threshold = determing_tsc_threshold();
		while(time_flag){
			for(int i=1; i<11; i++){
				pr_i_time = reload(numbers[i]);
				if(pr_i_time < tsc_threshold){
					// printf("Accessed numbers[%d]: %ld cycles \n", i, pr_i_time);
					printf("Access\n");
				}
				flush(numbers[i]);
			}
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

	// task_2_a();
	task_2_b();
}
