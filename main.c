#include "taskset.h"
#include "multimode.h"


int main(int argc, char* argv[])
{
    int numberOfRuns = 100;
    int utilStepCount = 2;
    float startUtil = 0.7;
    float utilStep = 0.1;
    
    struct sched_param param;
    pthread_attr_t attr;
    int ret, run;
    int externalInput = 0;
    struct timespec begin, now;

    int taskCount = getTaskCount();
    struct task_struct cTasks[taskCount];  
    struct task_struct cTasks2[taskCount];
    pthread_t thread[taskCount];
    struct thread_info tinfo[taskCount];
    

    srand (time (NULL));
   // getTasks(oTasks);
    for (int k = 0; k < utilStepCount; k++){
        float util = startUtil + k*utilStep;

        char fptName[15] = "wcrtFPT";
		char fpmName[15] = "wcrtFPM";
		char buf[3];
		char buf2[2];
		int u = util*10;
		sprintf(buf, "%i", u);
		sprintf(buf2, "%i", taskCount);
		strcat(fptName, buf);
		strcat(fpmName, buf);
		strcat(fptName, buf2);
		strcat(fpmName, buf2);
		strcat(fptName, ".txt\0");
		strcat(fpmName, ".txt\0");
		printf("%s    %s\n", fptName, fpmName);

		//remove("wcrtFPT.txt");
		//remove("wcrtFPM.txt");

		for (int i = 0; i < numberOfRuns; i++){
			generateTasks(cTasks, util);
			copyTaskStruct(cTasks, cTasks2, taskCount);
			struct task_struct* tasks;
			char* fileName = 0;
			char* wcrtFile = 0;
			for (int j = 0; j < 2; j++){
				if (j == 0){
					tasks = cTasks;
					fileName = "fileFPT.txt";
			        wcrtFile = fptName;
					rmAssignFPT(tasks);
				}
				else{
					tasks = cTasks2;
					fileName="fileFPM.txt";
			        wcrtFile = fpmName;
					rmAssignFPM(tasks);
				}
                g_curMode = j;
                refreshFileNames(util, taskCount, i);

				setTaskBegin(tasks, taskCount, 1000000000);
				#ifdef print 
					printTasks(tasks, taskCount);
				#endif
				tasksToFile(tasks, taskCount, fileName);

				if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
					printf("mlockall failed: %m\n");
					freeTaskStruct(tasks, taskCount);
					exit(-2);
				}

				ret = pthread_attr_init(&attr);
				if (ret) {
					printf("init pthread attributes failed\n");
					freeTaskStruct(tasks, taskCount);
					exit(-2);
				}

				ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
				if (ret) {
					printf("pthread setstacksize failed\n");
					freeTaskStruct(tasks, taskCount);
					exit(-2);
				}

				ret = pthread_attr_setschedpolicy(&attr, SCHED_RR);
				if (ret) {
					printf("pthread setschedpolicy failed\n");
					freeTaskStruct(tasks, taskCount);
					exit(-2);
				}    
				 
				param.sched_priority = 98;

				ret = pthread_attr_setschedparam(&attr, &param);
				if (ret) {
					printf("pthread setschedparam failed\n");
					freeTaskStruct(tasks, taskCount);
					exit(-2);
				}

				ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
				if (ret) {
					printf("pthread setinheritsched failed\n");
					freeTaskStruct(tasks, taskCount);
					exit(-2);
				}

				fillThreadInfos(tinfo, tasks, taskCount, &run, &externalInput);
				run=1;

				ret = createThreads(tinfo, thread, taskCount, &attr);
				if (ret){
					freeTaskStruct(tasks, taskCount);
					exit(-2);
				}
                for (int m = 0; m < 2; m++){
                    clock_gettime(CLOCK_MONOTONIC, &begin);
					do{
                        externalInput = getRand(0,9000);
                        //printf("%i\n", externalInput);
                        mySleep(200000);
						clock_gettime(CLOCK_MONOTONIC, &now);                        
					} while (diff(begin, now).tv_sec < 30);
                    //printf("dkdlfgsersjgoierjgoserjgoesrjtgoserjteostjia");
                }
				run = 0;
				joinThreads(thread, taskCount);

				if (j==0)
					printf("FPT WCRT: %i\n", wcrt);
				else
					printf("FPM WCRT: %i\n", wcrt);
			        
                wcrtToFile(wcrt, wcrtFile);
				printf("\n");
				wcrt = 0;
			}
			freeTaskStruct(cTasks, taskCount);
			freeTaskStruct(cTasks2, taskCount);
			
        }
    }
    //for (int i = 0; i < size; i++)
    //    printf("%i", execOrder[i]);
    return ret;
}
