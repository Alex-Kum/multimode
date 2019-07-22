#include "helper.h"

void changeThreadPriority(int prio){
    int policy;
    struct sched_param param;

    pthread_getschedparam(pthread_self(), &policy, &param);
    param.sched_priority = prio;
    pthread_setschedparam(pthread_self(), SCHED_RR, &param);
}

void* multimodeExecuter(void* args){
    struct thread_info tinfo = *(struct thread_info*)args;
    struct sched_param param;
    struct timespec beginPeriod, endPeriod, endSleep, responseT;
    int policy, input, curMode;

    changeThreadPriority(tinfo.tstruct->priority[0]);

    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &tinfo.tstruct->begin, NULL);

    while(*tinfo.running){
        clock_gettime(CLOCK_MONOTONIC, &beginPeriod);
        input = *tinfo.input;
        curMode = getMode(tinfo.tstruct, input);
        endPeriod = add(beginPeriod, tinfo.tstruct->period[curMode]);
        changeThreadPriority(tinfo.tstruct->priority[curMode]);       

        pthread_getschedparam(pthread_self(), &policy, &param);
        #ifdef print
            printf("Input: %i\n", input);        
            printf("Priority: %i\n", param.sched_priority);
            printf("Execute task in mode %i\n", curMode);   
        #endif
        
        if (counter < size){
            execOrder[counter] = tinfo.threadId;
            counter++;
        }        
        tinfo.tstruct->function[curMode]((void*)&tinfo.tstruct->execTime[curMode]);
        clock_gettime(CLOCK_MONOTONIC, &responseT);
        responseT = diff(beginPeriod, responseT);
        #ifdef print
            printTimespec("responseTime: ", responseT);
        #endif
        if (timeToIntNs(responseT) > wcrt){
            wcrt = timeToIntNs(responseT);
        }

        if (counter < size){
            execOrder[counter] = tinfo.threadId;
            counter++;
        }     
        
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &endPeriod, NULL);
        clock_gettime(CLOCK_MONOTONIC, &endSleep);
        #ifdef print
            printTimespec("Period: ", diff(beginPeriod, endSleep));
            printf("\n\n");
        #endif
    }
    #ifdef print
        printf("end\n");
    #endif
    pthread_exit(NULL);
 
}

int createThreads(struct thread_info* tinfo, pthread_t* thread, int taskCount, pthread_attr_t* attr){
    int ret;

    for (int i = 0; i < taskCount; i++){
        ret = pthread_create(&thread[i], attr, &multimodeExecuter, (void*)&tinfo[i]);
        if (ret) {
            printf("create pthread failed%i\n",ret);
            return ret;
        }
    }
    return 0;
}

void joinThreads(pthread_t* thread, int taskCount){
    int ret;

    for (int i = 0; i < taskCount; i++){
        ret = pthread_join(thread[i], NULL);

        if (ret){
            printf("join pthread failed: %m\n");
        }
    }
}
