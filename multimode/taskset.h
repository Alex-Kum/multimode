#include "helper.h"

int getTaskCount(){
    return 10;
}

int getRandExecTime(int wcet){
    double factor = randn(0.8, 0.4);
    if (factor < 0.5)
        factor = 0.5;
    if (factor > 1)
        factor = 1;
    
    return wcet * factor;
}

void functionWithExecTime(void* wcet){
    struct timespec beginExec, now;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &beginExec);
    int time = getRandExecTime(*(int*)wcet);
    do{
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);
    } while (timeToIntNs(diff(beginExec, now)) < time);
    
    #ifdef print
        struct timespec endSleep;
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &endSleep);
        printf("echt: %i---soll:%i\n", timeToIntNs(diff(beginExec, endSleep)), time);
    #endif
}

void setTaskBegin(struct task_struct* tstruct, int taskCount, int amountTime){
    struct timespec start, beg;

    clock_gettime(CLOCK_MONOTONIC, &start);
    beg = add(start, intNsToTime(amountTime));

    for (int i = 0; i < taskCount; i++){
        tstruct[i].begin = beg;
    }
}

int getModeCount(struct task_struct* tstruct){
    int sum = 0;
    int taskCount = getTaskCount();

    for (int i = 0; i < taskCount; i++){
    	sum += tstruct[i].modeCount;
    }
    return sum;
}

void UUniFast(double* USet, double u){
    int taskCount = getTaskCount();
    double sumU = u;
    double nextSum;

    srand(time(NULL));
    for (int i = 0; i < taskCount-1; i++){
        nextSum = sumU * pow( ((double)rand() / RAND_MAX), (double)1 / (taskCount-i));
        USet[i] = sumU-nextSum;
        sumU = nextSum;
    }
    USet[taskCount-1] = sumU;
}

int getRandPeriod(){
    int rand = getRand(0, 100);
    int fac = 1000;
    if (rand < 25)
        return 1000*fac;
    if (rand < 50)
        return 2000*fac;
    if (rand < 75)
        return 5000*fac;
    return 10000*fac;
}

void CSetGenerate(struct task_struct* tstruct, double* USet){
    int taskCount = getTaskCount();
    int j = 0;

    for (int i = 0; i < taskCount; i++){
        tstruct[i].period[0] = intNsToTime(getRandPeriod());
        int execT =(int)(USet[i] * timeToIntNs(tstruct[i].period[0]));
        tstruct[i].execTime[0] = execT;
        tstruct[i].function[0] = functionWithExecTime;
        j++;
    }
}

void makeMultiMode(struct task_struct* tstruct){
    int taskCount = getTaskCount();    

    for (int i = 0; i < taskCount; i++){
        int rand = getRand(7,9);
    	float factor = rand/10.0;
        for (int j = 1; j < tstruct[i].modeCount; j++){
            int p = timeToIntNs(tstruct[i].period[j-1]) * factor;
            tstruct[i].period[j] = intNsToTime(p);
            tstruct[i].execTime[j] = tstruct[i].execTime[j-1] * factor;
            tstruct[i].function[j] = tstruct[i].function[j-1];
        }
    }

    int inputLimit = 9000;
    for (int i = 0; i < taskCount; i++){
    	int sum = 0;
    	int amount = inputLimit / tstruct[i].modeCount;
    	for (int j = 0; j < tstruct[i].modeCount-1; j++){
            sum += amount;
            tstruct[i].limit[j] = sum;
    	}
    	tstruct[i].limit[tstruct[i].modeCount-1] = inputLimit;
    }
}

int smallestPeriodFPT(struct task_struct* tstruct, int min){
    int smallest = INT_MAX;
    int taskCount = getTaskCount();

    for (int i = 0; i < taskCount; i++){
        int period = timeToIntNs(tstruct[i].period[0]);
        if (period < smallest && period > min){
            smallest = period;
        }
    }
    return smallest;
}

int smallestPeriodFPM(struct task_struct* tstruct, int min){
    int smallest = INT_MAX;
    int taskCount = getTaskCount();

    for (int i = 0; i < taskCount; i++){
        for (int k = 0; k < tstruct[i].modeCount; k++){
            int period = timeToIntNs(tstruct[i].period[k]);
            if (period < smallest && period > min){
                smallest = period;
            }
        }
    }
    return smallest;
}

void rmAssignFPT(struct task_struct* tstruct){
    int taskCount = getTaskCount();
    int min, assignedPriorities, curPrio, smallest;
    int changed = 0;

    assignedPriorities = 0;
    curPrio = 97;
    highestPrio = curPrio;
    min = 0;
    while(assignedPriorities < taskCount){
        smallest = smallestPeriodFPT(tstruct, min);
        min = smallest;

        for (int j = 0; j < taskCount; j++){
            if (timeToIntNs(tstruct[j].period[0]) == smallest){
                tstruct[j].priority[0] = curPrio;
                assignedPriorities++;
                changed = 1;
            }
        }
        if (changed){
            changed = 0;
            curPrio--;
        }
    }
    lowestPrio = curPrio+1;
    for (int i = 0; i < taskCount; i++){
        for (int j = 1; j < tstruct[i].modeCount; j++){
            tstruct[i].priority[j] = tstruct[i].priority[0];
        }
    }
    #ifdef print
        printf("H: %i  L: %i\n", highestPrio, lowestPrio);
    #endif
}

void rmAssignFPM(struct task_struct* tstruct){
    int taskCount = getTaskCount();
    int modes = getModeCount(tstruct);
    int min, assignedPriorities, curPrio, smallest;
    int changed = 0;

    assignedPriorities = 0;
    curPrio = 97;
    min = 0;
    while(assignedPriorities < modes){
        smallest = smallestPeriodFPM(tstruct, min);

        for (int j = 0; j < taskCount; j++){
            for (int k = 0; k < tstruct[j].modeCount; k++){
                if (timeToIntNs(tstruct[j].period[k]) == smallest){
                    tstruct[j].priority[k] = curPrio;
                    assignedPriorities++;
                }
            }
        }
        min = smallest;
        curPrio--;
    }
    lowestPrio = curPrio+1;
    #ifdef print
        printf("H: %i  L: %i\n", highestPrio, lowestPrio);
    #endif
}


void generateTasks(struct task_struct* tstruct, double u){
    int taskCount = getTaskCount();
    int modes[taskCount];
    double USet[taskCount];

    for (int i = 0; i < taskCount; i++){
    	if (i < taskCount/2)
            modes[i] = 1;
    	else{
            modes[i] = 7;
    	}
    }

    initTaskStruct(tstruct, taskCount, modes);
    UUniFast(USet, u);
    CSetGenerate(tstruct, USet);
    makeMultiMode(tstruct);
}

