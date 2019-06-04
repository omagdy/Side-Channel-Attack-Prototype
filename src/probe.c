#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "time.h"
#include <string.h>
#include <unistd.h>
#include <stdint.h> 

#include "probe.h"

#define ITERATION_COUNT (1000) 				// Used for part 2a
#define TIME_LOOP_THRESHOLD (10000000000)	// Microseconds
#define LOOP_WAIT (5)



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



void task_2_b(){
	
    int LOOPFOREVER = 1;
    int numbers_size = 11; // numbers variable indexes to check

    // Perform actual flush and reload on numbers variable
    const void *numbers_addr;
    uint64_t number_access_time;
    int access_detected = 0;

    // Determine detection threshold
    int DETECTION_THRESHOLD = determing_tsc_threshold();

    while(LOOPFOREVER){
            for (int i = 0; i < numbers_size; ++i)
            {
                numbers_addr = numbers[i+1];
                number_access_time = flush_and_reload(numbers_addr);
                // printf("Access for [%d], Time:%lu\n", i, number_access_time);

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
    }
}


int main() {

	// task_2_a();
	task_2_b();
}
