#ifndef HELPER_H
#define HELPER_H

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

typedef void (*funcp)(void* arg);

struct thread_info{
    struct task_struct *tstruct;
    int* input;
    int* running;
};

struct task_struct{
    int modeCount;
    int* priority;
    struct timespec* period;
    struct timespec begin;
    int* limit;
    funcp* function;
};

int compare(const void* a, const void* b){
    return ( *(int*)a - *(int*)b );
}

void printArr(int* arr, int length){
    for (int i = 0; i < length; i++){
    	printf("%i ", arr[i]);
    }
    printf("\n");
}

struct timespec intNsToTime(int time){
    struct timespec ret;
    ret.tv_nsec = time % 1000000000;
    ret.tv_sec = time / 1000000000;

    return ret;
}

int timeToIntNs(struct timespec time){
    int ns = time.tv_sec * 1000000000;
    ns += time.tv_nsec;
    return ns;
}

int sameMode(struct task_struct* tstruct1, int mode1, struct task_struct* tstruct2, int mode2){
	/*printf("-----------------------------------------------------------------\n");
	printf("%i : %i\n", mode1, mode2);
	printf("%i : %i\n", tstruct1->function[mode1] , tstruct2->function[mode2]);
	printf("%i : %i\n", timeToIntNs(tstruct1->period[mode1]), timeToIntNs(tstruct2->period[mode2]));
	printf("%i : %i\n", tstruct1->priority[mode1], tstruct2->priority[mode2]);*/

    if(tstruct1->function[mode1] != tstruct2->function[mode2])
    	return 0;

    if(timeToIntNs(tstruct1->period[mode1]) != timeToIntNs(tstruct2->period[mode2]))
    	return 0;

    if (tstruct1->priority[mode1] != tstruct2->priority[mode2])
    	return 0;

    //printf("GLEICH");
    return 1;
}

int removeDuplicates(int* arr, int length){    //absturz wenn nicht sortiert??
	//printf("ANFANG\n");
	//printArr(arr, length);
    int n = length;
    int i = 0;

    while(i < n-1){
    	//printf("%ivon%i:\n", i,n);
    	//printArr(arr, length);
    	//printf("%i<%i\n", i, n);
        if (arr[i] == arr[i+1]){
        	//printf("%i:%i   ", arr[i-1],arr[i]);
            for (int k = i; k < n-1; k++){
            	arr[k] = arr[k+1];
            }
            arr[n-1] = -1;
            n--;
        }
        else{
            i++;
        }
        //printArr(arr, length);
        //printf("%i\n", n);
    }
    return n;
}

void removeDuplicatesTask(struct task_struct* tstruct){
	int i = 0;

	while(i < tstruct->modeCount-1){
		if (sameMode(tstruct, i, tstruct, i+1)){
			 for (int k = i; k < tstruct->modeCount-1; k++){
				 tstruct->priority[k] = tstruct->priority[k+1];
				 tstruct->function[k] = tstruct->function[k+1];
				 tstruct->limit[k] = tstruct->limit[k+1];
				 tstruct->period[k] = tstruct->period[k+1];
				 tstruct->priority[k] = tstruct->priority[k+1];
			 }
			 tstruct->modeCount--;
		}
		else{
			i++;
		}

	}
}

void printTimespec(const char* str, struct timespec t){
    printf("%s", str);
    printf("%li : %li\n", t.tv_sec, t.tv_nsec);
}



void initTaskStruct(struct task_struct* tstruct, int taskCount, int* modes){
    for(int i = 0; i < taskCount; i++){
        tstruct[i].modeCount = modes[i];
        tstruct[i].priority = (int*) malloc(modes[i]*sizeof(int));
        tstruct[i].period = (struct timespec*) malloc(modes[i]*sizeof(struct timespec));
        tstruct[i].limit = (int*) malloc(modes[i]*sizeof(int));
        tstruct[i].function = malloc(modes[i]*sizeof(funcp));
    }

}

void freeTaskStruct(struct task_struct* tstruct, int taskCount){
    for (int i = 0; i < taskCount; i++){
        free(tstruct[i].priority);
        free(tstruct[i].period);
        free(tstruct[i].limit);
        free(tstruct[i].function);
    }
}



void printTaskStruct(struct task_struct* m){
	printf("ModeCount: %i\n", m->modeCount);

    for (int i = 0; i < m->modeCount; i++){
        printf("Mode: %i \n" , i);
        printf("Priority: %i \n", m->priority[i]);
        printf("Limit: %i \n", m->limit[i]);
        printTimespec("Period: ", m->period[i]);
        printTimespec("Begin: ", m->begin);
        printf("\n");
    }
}



void printTasks(struct task_struct* t, int taskCount){
    for (int i = 0; i < taskCount; i++){
        printTaskStruct(&t[i]);
    }
}

int getRand(int min, int max){
    return (rand () % ((max + 1) - min)) + min;
}

struct timespec diff(struct timespec start, struct timespec end){
    struct timespec tmp;
    tmp.tv_sec = end.tv_sec-start.tv_sec;
    tmp.tv_nsec = end.tv_nsec-start.tv_nsec;

    if (tmp.tv_nsec < 0){
        tmp.tv_nsec += 1000000000;
        tmp.tv_sec -= 1;
    }
    return tmp;
}

struct timespec add(struct timespec t1, struct timespec t2){
    struct timespec tmp;
    tmp.tv_nsec = t1.tv_nsec + t2.tv_nsec;
    tmp.tv_sec = t1.tv_sec + t2.tv_sec;

    if (tmp.tv_nsec > 1000000000){
        tmp.tv_nsec -= 1000000000;
        tmp.tv_sec += 1;
    }
    return tmp;
}

void fillThreadInfos(struct thread_info* tinfo, struct task_struct* tstruct, int taskCount, int* run, int* externalInput){   
    for (int i = 0; i < taskCount; i++){
        tinfo[i].input = externalInput;
        tinfo[i].running = run;
        tinfo[i].tstruct = &tstruct[i]; 
    }
}

int getMode(struct task_struct* tstruct, int externalI){
    for (int i = 0; i < tstruct->modeCount; i++){
        if (externalI <= tstruct->limit[i]){
            return i;
        }
    }
    return -1;
}
#endif
