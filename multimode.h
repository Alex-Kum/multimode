#include "helper.h"

void changeThreadPriority(int prio){
    pthread_setschedprio(pthread_self(), prio);
}

void* multimodeExecuter(void* args){
    struct thread_info tinfo = *(struct thread_info*)args;
    struct timespec beginPeriod, endPeriod, responseT;
    int input, curMode, prio;
    setTime(&endPeriod, &tinfo.tstruct->begin);
    prio = tinfo.tstruct->priority[0];
    changeThreadPriority(prio);
    curMode = 0;
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &endPeriod, NULL);

    if (tinfo.tstruct->modeCount == 1){
        while(*tinfo.running){
            setTime(&beginPeriod, &endPeriod);
            endPeriod = add(beginPeriod, tinfo.tstruct->period[curMode]);
            tinfo.tstruct->function[curMode]((void*)&tinfo.tstruct->execTime[curMode]);
            if (prio == lowestPrio){
                clock_gettime(CLOCK_MONOTONIC, &responseT);
                responseT = diff(beginPeriod, responseT);
                int resT = timeToIntNs(responseT);
                if (resT > wcrt)
                    wcrt = resT;
                if (counter < size){
                    pthread_mutex_lock(&mutex);
                    rTime[counter] = resT;
                    counter++;
                    pthread_mutex_unlock(&mutex);
                }
            }
             
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &endPeriod, NULL);
        } 
    }
    else{
        while(*tinfo.running){
	        setTime(&beginPeriod, &endPeriod);
	        input = *tinfo.input;
	        curMode = getMode(tinfo.tstruct, input);
	        endPeriod = add(beginPeriod, tinfo.tstruct->period[curMode]);
            if (g_curMode == 1){
	            prio = tinfo.tstruct->priority[curMode];
	            changeThreadPriority(prio);  
            }	            
	            
	        tinfo.tstruct->function[curMode]((void*)&tinfo.tstruct->execTime[curMode]);
            if (g_curMode == 1)
	            changeThreadPriority(98); 
            
	        if (prio == lowestPrio){
	            clock_gettime(CLOCK_MONOTONIC, &responseT);
	            responseT = diff(beginPeriod, responseT);
	            int resT = timeToIntNs(responseT);
	            if (resT > wcrt)
	                wcrt = resT;
	            if (counter < size){
	                pthread_mutex_lock(&mutex);
	                rTime[counter] = resT;
	                counter++;
	                pthread_mutex_unlock(&mutex);
	            }
	        }     
	        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &endPeriod, NULL);
            clock_gettime(CLOCK_MONOTONIC, &endSleep);

            #ifdef print
                struct timespec endSleep;
                printf("id%i\n",tinfo.threadId);
	            printTimespec("responseTime: ", responseT);
	            pthread_getschedparam(pthread_self(), &policy, &param);
	            printf("Input: %i\n", input);        
	            printf("Priority: %i\n", param.sched_priority);
	            printf("Execute task in mode %i\n", curMode);
	            printTimespec("Period: ", diff(beginPeriod, endSleep));
                printTimespec("soll Period: ", tinfo.tstruct->period[curMode]);
	            printf("\n\n");
	        #endif
        }
    }

    #ifdef print
        printf("end\n");
    #endif
    pthread_exit(NULL);
 
}

int createThreads(struct thread_info* tinfo, pthread_t* thread, int taskCount, pthread_attr_t* attr){
    int ret;

    for (int i = 0; i < taskCount; i++){
        ret = pthread_create(&thread[i], &attr[i], &multimodeExecuter, (void*)&tinfo[i]);
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
