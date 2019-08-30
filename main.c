#include "taskset.h"
#include "multimode.h"


int main(int argc, char* argv[])
{
    int numberOfRuns = 100;
    int utilStepCount = 9;
    float startUtil = 0.3;
    float utilStep = 0.05;

    int taskCount = getTaskCount();
    struct sched_param param[taskCount];
    pthread_attr_t attr[taskCount];
    int ret, run;
    int externalInput = 0;
    struct timespec begin, now;
    
    
    srand (time (NULL));
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        printf("mlockall failed: %m\n");
        exit(-2);
    }

    for (int i = 0; i < taskCount; i++){
        ret = pthread_attr_init(&attr[i]);
        if (ret) {
            printf("init pthread attributes failed\n");
            exit(-2);
        }

        ret = pthread_attr_setstacksize(&attr[i], PTHREAD_STACK_MIN);
        if (ret) {
            printf("pthread setstacksize failed\n");
            exit(-2);
        }

        ret = pthread_attr_setschedpolicy(&attr[i], SCHED_FIFO);
        if (ret) {
            printf("pthread setschedpolicy failed\n");
            exit(-2);
        }    
         
        param[i].sched_priority = 98;

        ret = pthread_attr_setschedparam(&attr[i], &param[i]);
        if (ret) {
            printf("pthread setschedparam failed\n");
            exit(-2);
        }

        ret = pthread_attr_setinheritsched(&attr[i], PTHREAD_EXPLICIT_SCHED);
        if (ret) {
            printf("pthread setinheritsched failed\n");
            exit(-2);
        }
    }
    pthread_setschedprio(pthread_self(), 98);
    for (int k = 0; k < utilStepCount; k++){
        struct task_struct cTasks[taskCount];  
        struct task_struct cTasks2[taskCount];
        pthread_t thread[taskCount];
        struct thread_info tinfo[taskCount];

        float util = startUtil + k*utilStep;

        char fptName[20] = "wcrtFPT";
        char fpmName[20] = "wcrtFPM";   //17
        char buf[3];
        char buf2[3];
        int u = util*100;
        sprintf(buf, "%i", u);
        sprintf(buf2, "%i", taskCount);
        strcat(fptName, buf);
        strcat(fpmName, buf);
        strcat(fptName, buf2);
        strcat(fpmName, buf2);
        strcat(fptName, ".txt\0");
        strcat(fpmName, ".txt\0");
        //printf("%s    %s\n", fptName, fpmName);

        for (int i = 0; i < numberOfRuns; i++){
            counter = 0;
            initArr(rTime, size);
            generateTasks(cTasks, util);
            copyTaskStruct(cTasks, cTasks2, taskCount);
            struct task_struct* tasks;
            //char* fileName = 0;
            char* wcrtFile = 0;
            for (int j = 1; j < 2; j++){
                if (j == 0){
                    tasks = cTasks;
                    //fileName = "fileFPT.txt";
                    wcrtFile = fptName;
                    rmAssignFPT(tasks);
                }
                else{
                    tasks = cTasks2;
                    //fileName="fileFPM.txt";
                    wcrtFile = fpmName;
                    rmAssignFPM(tasks);
                }
                g_curMode = j;
                refreshFileNames(util, taskCount, i);

                setTaskBegin(tasks, taskCount, 1000000000);
                #ifdef print 
                    printTasks(tasks, taskCount);
                #endif
                //tasksToFile(tasks, taskCount, fileName);

                fillThreadInfos(tinfo, tasks, taskCount, &run, &externalInput);
                run=1;
                ret = createThreads(tinfo, thread, taskCount, attr);
                if (ret){
                    freeTaskStruct(tasks, taskCount);
                    exit(-2);
                }
                externalInput = 0;
                int dir = 0;
                for (int m = 0; m < 2; m++){
                    clock_gettime(CLOCK_MONOTONIC, &begin);
                    do{
                        //externalInput = getRand(0,9000);
                        if (dir == 0){
                            externalInput += 100;
                            if (externalInput == 9000)
                                dir = 1;
                        }
                        else{
                            externalInput -= 100;
                            if (externalInput == 0)
                                dir = 0;
                        }
                        mySleep(50000000);
                        clock_gettime(CLOCK_MONOTONIC, &now);                        
                    } while (diff(begin, now).tv_sec < 60);
                }
                run = 0;
                joinThreads(thread, taskCount);

                if (j==0)
                    printf("FPT WCRT: %i\n", wcrt);
                else
                    printf("FPM WCRT: %i\n", wcrt);
                    
                rTimeToFile();
                wcrtToFile(wcrt, wcrtFile);
                wcrt = 0;
            }
            freeTaskStruct(cTasks, taskCount);
            freeTaskStruct(cTasks2, taskCount);
			
        }
    }
    return ret;
}
