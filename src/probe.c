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

// void write_to_csv(int* p_time, uint64_t* l_time_1, uint64_t* l_time_2, int array_size, char* fname){

//     // To use variable filename
//     char buf[0x100];
//     snprintf(buf, sizeof(buf), "%s.csv", fname);

//     FILE *fp = fopen(buf, "w");

//     for(int i = 0; i < array_size; i++){
//         fprintf(fp, "%d,%lu,%lu\n", *p_time, *l_time_1, *l_time_2);
//         p_time++;
//         l_time_1++;
//         l_time_2++;
//     }

//     fclose (fp);
// } 


// void task_2_a(){

//     int index1 = 5, index2 = 7; // Indexes of the numbers variable to probe
//     int sleep_duration = 10; // sleep duration

//     uint64_t load_time_array_1[ITERATION_COUNT], load_time_array_2[ITERATION_COUNT];
//     int probe_time_array[ITERATION_COUNT];

//     // Pointers to save numbers index addresses
//     const void *num_addr1;
//     const void *num_addr2;

//     for (int i = 0; i < ITERATION_COUNT; ++i)
//     {

//         // Flush and reload first index
//         num_addr1 = numbers[index1];
//         load_time_array_1[i] = reload(num_addr1);

//         // Flush and reload second index
//         num_addr2 = numbers[index2];
//         load_time_array_2[i] = reload(num_addr2);

//         // Flush both indexes
//         flush(num_addr1);
//         flush(num_addr2);

//         probe_time_array[i] = sleep_duration * i;

//         // usleep(sleep_duration);
//     }

// 	write_to_csv(probe_time_array, load_time_array_1, load_time_array_2, ITERATION_COUNT, "data");

// 	return;
// }


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
