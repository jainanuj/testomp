//
//  testomp.c
//  dummylib
//
//  Created by Anuj Jain on 12/24/19.
//  Copyright © 2019 Anuj Jain. All rights reserved.
//

#include "testomp.h"
#include <omp.h>

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


main(int argc, char *argv[]) {
    
    int nthreads, tid, maxtid = 0; // gv_toupdate = 0;
    int i = 12, myi, chunk=3, result, delta, bit_pos = -1;
    unsigned long x = 806092800;
    
    bit_pos = least_bit(x);
    printf("The bit pos for %lu was:%d\n\n",x,bit_pos);
    omp_set_num_threads(8);
    
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
    
}

void fn2()
{   /* Fork a team of threads with each thread having a private tid variable */
    int nthreads, tid, maxtid = 0; // gv_toupdate = 0;
    int i = 12, myi, chunk=3, result;
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

void testparallelfn(int xid)
{
    float local_fn_var = 0.0;
    local_fn_var = xid * 3.0;
    printf("\nPrinting from testparfn. XID=%d; LocVar=%f\n", xid, local_fn_var);
}
