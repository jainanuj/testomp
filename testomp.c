//
//  testomp.c
//  dummylib
//
//  Created by Anuj Jain on 12/24/19.
//  Copyright © 2019 Anuj Jain. All rights reserved.
//

#include "testomp.h"
#include "stdlib.h"
#include <omp.h>
#include "intqueue_conc.h"
#include "intqueue.h"

int processedI = 0, left = 0;
#pragma omp threadprivate(processedI, left)

int least_bit(unsigned long map)
{
/*
    unsigned long only_least_bit = 0;
    int least_bit_pos = -1;
    if (map <= 0)
        return -1;
    only_least_bit = map & ~(map-1);
    least_bit_pos = DEBRUIJNBITPOS[ ((only_least_bit * DEBSEQ) & MAX_DEB_SEQ_SIZE) >> DEB_SEQ_REM_WINDOW];
    return least_bit_pos;
*/
///*
    int i, x;
    for (i=0; i < 32; i++)
    {
        x = map >> i;
        if (x & 1)
            return i;
    }
    return -1;
 //*/
}
void fn()
{
    int TID;
    omp_set_max_active_levels(2);
    int numlevels = omp_get_max_active_levels();
    int n_socks = omp_get_num_places();
    printf("Nested parallelism is %s\n", omp_get_nested() ? "supported" : "not supported");
    printf("Num levels supported =%d; Num sockets=%d\n",numlevels, n_socks);
    
    int n_procs = omp_get_place_num_procs(0);
    printf("Num procs=%d\n", n_procs);
#pragma omp parallel private(TID) proc_bind(close)
    {
        TID = omp_get_thread_num();
        printf("Thread %d executes the outer parallel region\n",TID);
/*#pragma omp parallel num_threads(2) firstprivate(TID)
        {
            printf("TID %d: Thread %d executes inner parallel region\n", TID,omp_get_thread_num());
        } --Endofinnerparallelregion--*/
    }   //End of outer parallel.
}

main(int argc, char *argv[]) {
    
    int nthreads, tid, maxtid = 0; // gv_toupdate = 0;
    int i = 12, myi, chunk=3, result, delta, bit_pos = -1;
//    unsigned long x = 806092800;
    
//    bit_pos = least_bit(x);
//    printf("The bit pos for %lu was:%d\n\n",x,bit_pos);
    //omp_set_num_threads(8);
    //fn();
    
//    printf("Will be testing sections\n");
    //sectioning();
    
    //printf("Will be testing tasks:\n");
    tasking();
    //printf("Will be testing parallel for:\n");
    //fn2();
    
//    simplyParll();
     // printbindinginfo();
/*
#pragma omp parallel for default(shared) private(i, delta, tid)  \
    schedule(dynamic,chunk)      \
    reduction(max:result)
    for (i=20;i >= 0;i--)
    {
        tid = omp_get_thread_num();
        delta = (i % 5) * tid;
        result = (delta > result) ? delta: result;
        printf("Thread:%d,Itr:%d,delta=%d,result=%d\n",tid,i,delta,result);
    }
    printf("Final result = %d\n",result);
 */
}

void fn2()
{   /* Fork a team of threads with each thread having a private tid variable */
    int nthreads, tid, maxtid = 0; // gv_toupdate = 0;
    int i = 20, myi, chunk=3, result;
    #pragma omp parallel default(shared) private(tid)
    {
        while (i > 0)
        {
            /* Obtain and print thread id */
            tid = omp_get_thread_num();
            testparallelfn(tid);

            if (tid > maxtid)
#pragma omp atomic write
                maxtid = tid;
                //= tid>maxtid ? tid : maxtid ;

#pragma omp critical
            {
                if (i> 0)
                {
                    myi = i--;
                    processedI++;
//                    left = 0;
                }
                else
                    left = 1;
            }
//                        printf("Thread = %d, Left i = %d out and will exit", tid, i);*/
            /*
            if (left == 0)
                printf("thread = %d. Picked i = %d\n", tid, myi);
            else
                printf("Thread = %d, Left i = %d out and will exit\n", tid, i);
             */

            
            /* Only master thread does this
            if (tid == 0)
            {
                nthreads = omp_get_num_threads();
//                printf("\n\nI am Thread - %d. Number of threads = %d\n", tid, nthreads);
            }*/
        }
        printf("Thread %d exited while loop. Processed I = %d num times\n", tid, processedI);
        
    }  /* All threads join master thread and terminate */
    
    printf("\nMaximum thread Id is: %d", maxtid);
    printf("\n\nThis should be printed only by Thread %d\n\n", tid);
    printf("Another prarallel region will start and will do workshare.\n-----\n");
    
    #pragma omp parallel default(shared) private(tid, i)
    {
        tid = omp_get_thread_num();
        #pragma omp for schedule(dynamic,chunk) nowait
        for (i = 0; i <28; i++)
        {
            printf("This is Thread %d. This got to processI = %d\n", tid, i);
        }
    }
}

void sectioning()
{
#pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
                printf("This section is 1st section with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 2nd sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 3rd sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 4th sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 5th sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 6th sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 7th sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 8th sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 9th sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 10th sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 11th sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 12th sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 13th sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 13th sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 13th sec with thread:%d\n", omp_get_thread_num());
            #pragma omp section
                printf("This section is 13th sec with thread:%d\n", omp_get_thread_num());
        }
    }
}

#define QSIZE 500
#define QMAXVAL 505
void check_q_duplicates(queue_conc *qc)
{
    int items[QMAXVAL];
    int i = 0;
    unsigned int result;
    for (i=0; i < QMAXVAL; i++)
	items[i] = 0;
    printf("Num of items in q=%d\n",(qc->REAR - qc->FRONT));
    for (i=0; i< (qc->REAR - qc->FRONT); i++)
    {
        queue_conc_deq(qc, &result);
	if (result == EMPTYVAL)
		printf("Result returned at idx=%d is %d\n",i,result);
	else if (result > QMAXVAL)
		printf("Result returned at idx=%d is %d\n",i,result);
	else
	{
		if (items[result] == 0)
		    items[result] = 1;
		else
		    printf("At idx=%d, result=%d is duplicate\n",i, result);
	}
    }
    printf("Done checking for duplicates\n");
}

void consume(queue_conc *qc)
{
    int items[QMAXVAL+1];
    int i = 0;
    unsigned int result;
    int uniqueItems = 0;
    for (i=0; i <= QMAXVAL; i++)
        items[i] = 0;
#pragma omp parallel private(i, result) shared(qc, items)
    {
        while (queue_conc_has_items(qc) )
        {
                result = 0;
            queue_conc_deq(qc, &result);
    //        queue_pop(qc, &result);
            if (result == EMPTYVAL)
                printf("Result returned in thread=%d is %d\n",omp_get_thread_num(),result);
            else if (result > QMAXVAL)
                    printf("Result returned in thread=%d is %d\n",omp_get_thread_num(),result);
            else
            {
    #pragma omp critical
                   items[result]++;
            }
        }   //while loop.
    }       //Parallel section.
    for (i=0; i <= QMAXVAL; i++)
    {
        if (items[i] > 1)
	   printf("Item %d popped %d times",i, items[i]);
	else
	   uniqueItems += items[i];
    }
    printf("%d uniqueItems popped\n", uniqueItems);

}
void tasking()
{
    int i = 0, x=100, tid;
    int numThreads = 0;
    int *threadExecCounter = NULL;
    int taskCount = 40;
    int loopCount = 1010;
    int sum = 0;
    int sharedChangedByThread = -1;
    int ok = 0;
    unsigned int result;

    queue_conc *qc = queue_conc_create(QSIZE, QMAXVAL);
//    queue *qc = queue_create(QSIZE, QMAXVAL);
#pragma omp parallel shared(threadExecCounter, numThreads, qc) private(i,x)
    {
        #pragma omp single
        {
            numThreads = omp_get_num_threads();
            threadExecCounter = (int*) malloc(sizeof(int) * numThreads);
            memset(threadExecCounter,0,sizeof(int) * numThreads);
            printf("Number of threads is: %d; Number of tasks=%d\n", numThreads, taskCount);
            #pragma omp task untied
            {
                for (i = 0; i < loopCount; i++)
                {
                    x= i/2 % QMAXVAL;
                    if ((i % 100000) == 0)
                    {
                        tid = omp_get_thread_num();
                        printf("This is i=%d, on thread=%d\n",i,tid);
                    }
                    #pragma omp task private(tid) firstprivate(i,x) priority(5) shared(sharedChangedByThread)
                   {
                        tid = omp_get_thread_num();
                        ok =  __sync_bool_compare_and_swap(&sharedChangedByThread, -1, tid);
                        if (ok)
                            printf("Got ok for TID: %d\n",tid);
                        if ( (qc->REAR - qc->FRONT) < qc->maxitems)
                            queue_conc_enq(qc, x);
                        else
                        {
//                           queue_pop(qc, &result);
                            queue_conc_deq(qc, &result);
                            printf("Q was full. Item popped = %d\n", result);
                        }
                        //printf("Into task");
                        //if (i < taskCount)
                        //#pragma omp critical
                        processItem(x, &threadExecCounter);
                   }    //Task block
                }   //For loop
            }   //task for single thread block.
        }   //Single block.
    }   //parallel block.
    //int sum = 0;
    for (i=0; i< numThreads; i++)
    {
        printf("Thread %d executed %d items;  ", i, threadExecCounter[i]);
        sum += threadExecCounter[i];
    }
    printf("\n\nTotal items in q=%d\n",qc->REAR - qc->FRONT);
//    printf("Total items in bitq=%d\n",qc->bitqueue_conc->num_items);
    //check_q_duplicates(qc);
    printf("Shared var changed by thread: %d", sharedChangedByThread);
    printf("\nTotal items= %d;\n",sum);
    printf("Will now consume from the queue in parallel\n");
    consume(qc);
}

void simplyParll()
{
#pragma omp parallel
    {
        printf("Hello (%d)  ",omp_get_thread_num());
        printf("World (%d)  ", omp_get_thread_num());
    }
}

void printbindinginfo()
{
#pragma omp parallel num_threads(10) proc_bind(spread)
{
    int my_place = omp_get_place_num();
    int place_num_procs = omp_get_place_num_procs(my_place);
    int i;
    int TID = omp_get_thread_num();
    printf("(Place, thread): (%d, %d) consists of %d processors: \n", my_place, TID, place_num_procs);
#pragma omp flush
#pragma omp barrier

    int *place_processors = malloc(sizeof(int) * place_num_procs);
    omp_get_place_proc_ids(my_place, place_processors);
#pragma omp critical
    {
    for (i = 0; i< place_num_procs; i++)
        printf("%d ", place_processors[i]);
    printf("\n");
    }
   #pragma omp flush
   #pragma omp barrier
    #pragma omp parallel num_threads(2) firstprivate(TID) private(my_place, place_num_procs) proc_bind(close)
    {
#pragma omp critical
	    {
        my_place = omp_get_place_num();
        place_num_procs = omp_get_place_num_procs(my_place);
        printf("(Inner Place, Out_TID, in_thrd): (%d, %d, %d) consists of %d processors: ",my_place, TID, omp_get_thread_num(), place_num_procs);
        int *inner_place_processors = malloc(sizeof(int) * place_num_procs);
        omp_get_place_proc_ids(my_place, inner_place_processors);
          for (i = 0; i< place_num_procs; i++)
             printf("%d ", inner_place_processors[i]);
          printf("\n");
#pragma omp flush
	    }
    }
//free(place_processors);
}
}

void processItem(int i, int **threadExecCounter)
{
    int threadNum = omp_get_thread_num();
    int x=0; int j = 0;
//#pragma omp critical
    (*threadExecCounter)[threadNum]++;
    //sleep(1);
    for (j = 0; j <10000000; i++)
	    x++;
    //printf("Processing task for Item:%d in thread:%d.\n", i, threadNum);
}

void testparallelfn(int xid)
{
    float local_fn_var = 0.0;
    local_fn_var = xid * 3.0;
    printf("\nPrinting from testparfn. XID=%d; LocVar=%f\n", xid, local_fn_var);
}
