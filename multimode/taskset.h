#include "helper.h"


void setTaskBegin(struct task_struct* tstruct, int taskCount, int amountTime){
    struct timespec start, beg;

    clock_gettime(CLOCK_MONOTONIC, &start);
    beg = add(start, intNsToTime(amountTime));

    for (int i = 0; i < taskCount; i++){
        tstruct[i].begin = beg;
    }
}

int getLimitCount(struct task_struct* tstruct, int taskCount){
    int sum = 0;

    for (int i = 0; i < taskCount; i++){
    	sum += tstruct[i].modeCount;
    }
    return sum;
}

void getLimits(struct task_struct* tstruct, int taskCount, int* arr){
	int counter = 0;

    for (int i = 0; i < taskCount; i++){
    	for (int j = 0; j < tstruct[i].modeCount; j++){
    		arr[counter] = tstruct[i].limit[j];
    		counter++;
    	}
    }
}

int smallestPeriod(struct task_struct* tstruct, int taskCount, int* mode, int min){
    int smallest = INT_MAX;

    for (int i = 0; i < taskCount; i++){
        int period = timeToIntNs(tstruct[i].period[mode[i]]);
        if (period < smallest && period > min){
            smallest = period;
        }
    }
    return smallest;
}

void rmAssign(struct task_struct* tstruct, struct task_struct* newTStruct, int taskCount){
    int min, assignedPriorities, curPrio, smallest;
    int changed = 0;

	int modeOfTasks[taskCount];
    int intervalCount = getLimitCount(tstruct, taskCount);
    int intervals[intervalCount];
   // struct task_struct newTStruct[taskCount];

    getLimits(tstruct, taskCount, intervals);
    //printf("%i\n", intervalCount);
    qsort(intervals, intervalCount, sizeof(int), compare);
   // printArr(intervals, intervalCount);
    //printf("\n");
    intervalCount = removeDuplicates(intervals, intervalCount);
    //printArr(intervals, intervalCount);

    int modeCount[taskCount];
    for (int i = 0; i < taskCount; i++){
    	modeCount[i] = intervalCount;
    }

    initTaskStruct(newTStruct, taskCount, modeCount);
    //printTasks(tstruct, taskCount);
    //printf("neu\n");
    //printTasks(newTStruct, taskCount);
    for (int i = 0; i < intervalCount; i++){
    	assignedPriorities = 0;
    	curPrio = 97;
    	min = 0;

        for (int k = 0; k < taskCount; k++){
            modeOfTasks[k] = getMode(&tstruct[k], intervals[i]);
        }
        printf("\nModes %i:\n", intervals[i]);
        printArr(modeOfTasks, taskCount);

        while (assignedPriorities < taskCount){
            smallest = smallestPeriod(tstruct, taskCount, modeOfTasks, min);
           // printf("smallest: %i\n", smallest);
            min = smallest;

            for (int j = 0; j < taskCount; j++){
            	struct task_struct curTask = tstruct[j];

                if (timeToIntNs(curTask.period[modeOfTasks[j]]) == smallest){
                	//printf("CFEWJRQERJQRJR");
                	//printTaskStruct(&curTask);
                    newTStruct[j].priority[i] = curPrio;
                    newTStruct[j].function[i] = curTask.function[modeOfTasks[j]];
                    newTStruct[j].period[i] = curTask.period[modeOfTasks[j]];
                    newTStruct[j].limit[i] = intervals[i];
                    assignedPriorities++;
                    changed = 1;
                }
            }

            if (changed){
                changed = 0;
                curPrio--;
            }
        }
    }
    //printf("\n\n\n");
    for (int i = 0; i < taskCount;i++)
        removeDuplicatesTask(&newTStruct[i]);

    freeTaskStruct(tstruct, taskCount);
   //*/
   // printTasks(newTStruct, taskCount);
   // printf("\n\n\n");
}

/*int smallestPeriod(struct task_struct* tstruct, int taskCount, int mode, int min){
    int smallest = INT_MAX; 

    for (int i = 0; i < taskCount; i++){
        int period = timeToIntNs(tstruct[i].period[mode]);
        if (period < smallest && period > min){
            smallest = period;
        }
    }
    return smallest;
}

void rmAssign(struct task_struct* tstruct, int taskCount){
    int min, assignedPriorities, curPrio, smallest;
    int changed = 0;

    for (int i = 0; i < tstruct[0].modeCount; i++){
    	assignedPriorities = 0;
    	curPrio = 97;
    	min = 0;

        while (assignedPriorities < taskCount){
            smallest = smallestPeriod(tstruct, taskCount, i, min);
            min = smallest;
            for (int j = 0; j < taskCount; j++){            
                if (timeToIntNs(tstruct[j].period[i]) == smallest){
                    tstruct[j].priority[i] = curPrio;
                    assignedPriorities++;
                    changed = 1;
                }
            }

            if (changed){
            	changed = 0;
            	curPrio--;
            }
        }

    }
}*/

void f1(){
    printf("f1 executed\n");
}

void f2(){
    printf("f2 executed\n");
}

void f3(){
    printf("f3 executed\n");
}

void function_mode1(void* arg){
    f1();
    f2();
    f3();
}

void function_mode2(void* arg){
    f1();
    f2();
}

void function_mode3(void* arg){
    f1();
}

void function_mode11(void* arg){
    f3();
    f2();
    f1();
}

void function_mode22(void* arg){
    f3();
    f2();
}

void function_mode33(void* arg){
    f3();
}

void createModeStruct(struct task_struct* tstruct){
    tstruct->priority[0]  = 70;
    tstruct->period[0] = intNsToTime(1500000000);
    tstruct->limit[0] = 2000;
    tstruct->function[0] = &function_mode1;
    
    tstruct->priority[1] = 70;
    tstruct->period[1] = intNsToTime(1000000000);
    tstruct->limit[1] = 4000;
    tstruct->function[1] = &function_mode2;
    
    tstruct->priority[2] = 70;
    tstruct->period[2] = intNsToTime(300000000);
    tstruct->limit[2] = 9000;
    tstruct->function[2] = &function_mode3;
}

void createModeStruct2(struct task_struct* tstruct){
    tstruct->priority[0] = 70;
    tstruct->period[0] = intNsToTime(1400000000);
    tstruct->limit[0] = 3000;
    tstruct->function[0] = &function_mode11;
    
    tstruct->priority[1] = 70;
    tstruct->period[1] = intNsToTime(1100000000);
    tstruct->limit[1] = 6000;
    tstruct->function[1] = &function_mode22;
    
    tstruct->priority[2] = 70;
    tstruct->period[2] = intNsToTime(200000000);
    tstruct->limit[2] = 9000;
    tstruct->function[2] = &function_mode33;
}

int getTaskCount(){
    return 2;
}

int getTasks(struct task_struct* tasks){ 
    int taskCount = getTaskCount();
  
    int modeCount[taskCount];
    modeCount[0] = 3;
    modeCount[1] = 3;

    initTaskStruct(tasks, taskCount, modeCount);

    createModeStruct(&tasks[0]);
    createModeStruct2(&tasks[1]);

    return taskCount;
}


//EXAMPLE TASKS FROM PICTURE

void function1Ex(void* arg){
    struct timespec now;
    int a = 0;

    clock_gettime(CLOCK_MONOTONIC, &now);
    printTimespec("\n\nTHREAD1 START: ", now);
    
    for (int j=0; j < 10*25; j++){
        for (int i=0; i < 224000; i++){   
            a+=1;
        }
        printf("a");
    }
    clock_gettime(CLOCK_MONOTONIC, &now);
    printTimespec("THREAD1 END: ", now);
    printf("\n\n");
}

void function2Ex(void* arg){
    struct timespec now;
    int a = 0;

    clock_gettime(CLOCK_MONOTONIC, &now);
    printTimespec("\n\nTHREAD2 START: ", now);
    
    for (int j=0; j < 10*50; j++){
        for (int i=0; i < 224000; i++){    
            a+=1;
        }
        printf("b");
    }
    clock_gettime(CLOCK_MONOTONIC, &now);
    printTimespec("THREAD2 END: ", now);
    printf("\n\n");
}

void function3Ex(void* arg){
    struct timespec now;
    int a = 0;

    clock_gettime(CLOCK_MONOTONIC, &now);
    printTimespec("THREAD3 START: ", now);
    
    for (int j=0; j < 10*100; j++){
        for (int i=0; i < 224000; i++){    
            a+=1;
        }
        printf("c");
    }
    clock_gettime(CLOCK_MONOTONIC, &now);
    printTimespec("THREAD3 END: ", now);
    printf("\n\n");
}
  
void createExTask1(struct task_struct* tstruct){
    tstruct->priority[0] = 97;
    tstruct->period[0] = intNsToTime(600000000);
    tstruct->limit[0] = 9000;
    tstruct->function[0] = &function1Ex;
}

void createExTask2(struct task_struct* tstruct){    
    tstruct->priority[0] = 96;
    tstruct->period[0] = intNsToTime(800000000);
    tstruct->limit[0] = 9000;
    tstruct->function[0] = &function2Ex;
}

void createExTask3(struct task_struct* tstruct){  
    tstruct->priority[0] = 95;
    tstruct->period[0] = intNsToTime(1200000000);
    tstruct->limit[0] = 9000;
    tstruct->function[0] = &function3Ex;
}

void getExTasks(struct task_struct* tasks){
    createExTask1(&tasks[0]);
    createExTask2(&tasks[1]);
    createExTask3(&tasks[2]);

    setTaskBegin(tasks, 3, 2000000000);
}
