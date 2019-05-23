#include "helper.h"

void setTaskBegin(struct task_struct* tstruct, int taskCount, int amountTime){
    struct timespec start, beg;

    clock_gettime(CLOCK_MONOTONIC, &start);
    beg = add(start, intNsToTime(amountTime));

    for (int i = 0; i < taskCount; i++){
        tstruct[i].begin = beg;
    }
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

void rmAssign(struct task_struct* tstruct, int taskCount, int modes){
    int min = 0;
    int curPrio=97;
    int smallest;

    for (int i = 0; i < modes; i++){
        while (assignedPriorities < taskCount){
        smallest = smallestPeriod(tstruct, taskCount, i, min);
            for (int j = 0; j < taskCount; j++){            
                if (timeToIntNs(tstruct[j].period) == smallest){
                }
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
    tstruct[0].priority[0]  = 90;
    tstruct[0].period[0] = intNsToTime(1500000000);
    tstruct[0].limit[0] = 3000;
    tstruct[0].function[0] = &function_mode1;
    
    tstruct[0].priority[1] = 91;
    tstruct[0].period[1] = intNsToTime(1000000000);
    tstruct[0].limit[1] = 6000;
    tstruct[0].function[1] = &function_mode2;
    
    tstruct[0].priority[2] = 92;
    tstruct[0].period[2] = intNsToTime(300000000);
    tstruct[0].limit[2] = 9000;
    tstruct[0].function[2] = &function_mode3;
}

void createModeStruct2(struct task_struct* tstruct){
    tstruct[0].priority[0] = 93;
    tstruct[0].period[0] = intNsToTime(15000000);
    tstruct[0].limit[0] = 3000;
    tstruct[0].function[0] = &function_mode11;
    
    tstruct[0].priority[1] = 94;
    tstruct[0].period[1] = intNsToTime(1000000);
    tstruct[0].limit[1] = 6000;
    tstruct[0].function[1] = &function_mode22;
    
    tstruct[0].priority[2] = 95;
    tstruct[0].period[2] = intNsToTime(30000);
    tstruct[0].limit[2] = 9000;
    tstruct[0].function[2] = &function_mode33;
}

void getTasks(struct task_struct* tasks){
    createModeStruct(&tasks[0]);
    createModeStruct2(&tasks[1]);

    setTaskBegin(tasks, 3, 2000000000);
}


//EXAMPLE TASKS FROM PICTURE

void function1Ex(void* arg){
    struct timespec start, now;
    int a = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    printTimespec("\n\nTHREAD1 START: ", start);
    
    for (int j=0; j < 10*25; j++){
        for (int i=0; i < 224000; i++){   
            a+=1;
        }
        printf("a");
    }
    clock_gettime(CLOCK_MONOTONIC, &start);
    printTimespec("THREAD1 END: ", start);
    printf("\n\n");
}

void function2Ex(void* arg){
    struct timespec start, now;
    int a = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    printTimespec("\n\nTHREAD2 START: ", start);
    
    for (int j=0; j < 10*50; j++){
        for (int i=0; i < 224000; i++){    
            a+=1;
        }
        printf("b");
    }
    clock_gettime(CLOCK_MONOTONIC, &start);
    printTimespec("THREAD2 END: ", start);
    printf("\n\n");
}

void function3Ex(void* arg){
    struct timespec start, now;
    int a = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    printTimespec("THREAD3 START: ", start);
    
    for (int j=0; j < 10*100; j++){
        for (int i=0; i < 224000; i++){    
            a+=1;
        }
        printf("c");
    }
    clock_gettime(CLOCK_MONOTONIC, &start);
    printTimespec("THREAD3 END: ", start);
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




