//
//  intqueue.c
//  TVI
//
//  Created by Anuj Jain on 11/12/17.
//  Copyright © 2017 Anuj Jain. All rights reserved.
//


#include "intqueue_conc.h"
#include <sys/time.h>

/* don't use floats with this function -- you'll run into precision
 problems!*/
/*double whenq( void ) {
    struct timeval tv;
    gettimeofday( &tv, NULL );
    return (double)(tv.tv_sec) + (double)(tv.tv_usec)*(double)(1e-6);
}
*/


int check_obj_present_in_q(queue* q, int obj)
{
    int items_scanned = 0;
    int i = q->start_item_ptr;
    for (items_scanned = 0; items_scanned < q->numitems; items_scanned++)
    {
        if (q->items[i] == obj)
            return i;
        else
            i = ((i + 1)  % q->maxitems);
    }
    
    return -1;
}


//
// ----------------------------------------------------------------------------
//

queue_conc *queue_conc_create( int maxitems, int max_val )
{
    queue_conc *q; int i = 0;

#ifdef BITQ_CONC
  bit_queue_conc *bq;
#endif

  q = (queue_conc *)malloc( sizeof(queue_conc) );
  if (q == NULL) {
    fprintf(stderr, "Out of memory!\n");
    exit(0);
  }

  q->maxitems = maxitems;
  q->numitems = 0;

  q->items = (int *)malloc( (maxitems + 2) * sizeof(int) );
  if (q->items == NULL) {
    fprintf(stderr, "Out of memory!\n");
    exit(0);
  }
    q->start_item_ptr = 0;
    q->end_item_ptr = 1;
    q->items[q->start_item_ptr] = START_PLACE;
    q->items[q->end_item_ptr] = END_PLACE;
    for (i = 2; i < (maxitems + 2); i++)
    {
        q->items[i] = EMPTYVAL;
    }
#ifdef BITQ_CONC
    bq = create_bit_queue_conc(max_val);
    q->bitqueue_conc = bq;
#endif
    q->add_time = 0.0;
    q->pop_time = 0.0;

  return q;
}

void destroy_queue(queue *q)
{
#ifdef BITQ
    destroy_bit_queue(q->bitqueue);
#endif
    free(q->items);
    free(q);
}


int empty_queue_conc( queue_conc *q)
{
    q->numitems = 0;
    q->end_item_ptr = 1;
    q->start_item_ptr = 0;
    q->items[q->start_item_ptr] = START_PLACE;
    q->items[q->end_item_ptr] = END_PLACE;
#ifdef BITQ_CONC
    empty_bit_queue_conc(q->bitqueue_conc);
#endif
    return 0;
}

//
// ----------------------------------------------------------------------------
//
// dump the object at the end of the array.
//

int queue_conc_add( queue_conc *q, int obj )
{
//    double t_start, t_end;
//    t_start = whenq();

    bool ok = false;
#ifndef BITQ_CONC
    pos = check_obj_present_in_q(q, obj);
    if (pos != -1)
        return 1;     //Item already present at position pos.
#else
    if (check_bit_obj_present_conc(q->bitqueue_conc, obj))
        return 1;
#endif
    
    if ( q->numitems >= q->maxitems )
    {
        fprintf(stderr, "Hey!  Queue's full!\n");
        return 0;
    }

    while (!ok)
    {
#pragma omp flush
        ok = __sync_bool_compare_and_swap(&(q->items[ ((q->end_item_ptr + 1 ) % q->maxitems) ]), EMPTYVAL, END_PLACE);
        if (ok)
        {
            if (check_bit_obj_present_conc(q->bitqueue_conc, obj) )
            { //The obj that this thread wanted to add was already added by antother thread or some other threads already made the q full.
                q->items[ ((q->end_item_ptr + 1 ) % q->maxitems) ] = EMPTYVAL;
                return 1;
            }
        }
        else if (check_bit_obj_present_conc(q->bitqueue_conc, obj) || (q->numitems >= q->maxitems) )
            return 1;
    }
    q->items[ q->end_item_ptr ] = obj;
    queue_conc_add_bit(q->bitqueue_conc, obj);
    q->numitems++;
    q->end_item_ptr = ((q->end_item_ptr + 1 ) % q->maxitems);       //Finally increment the end_item_ptr. This will release next thread
#pragma omp flush
    
    return 0;
}

//
// ----------------------------------------------------------------------------
//

int queue_pop(queue *q, int *result )
{
//    double t_start, t_end;
//    t_start = whenq();
    bool ok = false;
    *result = -1;
    if ( (q->numitems <= 0) || (q->start_item_ptr < 0) || (q->items[((q->start_item_ptr + 1 ) % q->maxitems)] == END_PLACE))
    {
        //fprintf( stderr, "Hey! queue's empty!\n" );
        return 0;
    }
    
    while (!ok)
    {
#pragma omp flush
        ok = __sync_bool_compare_and_swap(&(q->items[q->start_item_ptr]), START_PLACE, START_TRANS);
        if (ok)
        {
            if ( (q->numitems <= 0) || (q->start_item_ptr < 0) || (q->items[((q->start_item_ptr + 1 ) % q->maxitems)] == END_PLACE))
            {
                //fprintf( stderr, "Hey! queue's empty!\n" );
                q->items[q->start_item_ptr] = START_PLACE;
                return 0;
            }
        }
        else if ( (q->numitems <= 0) || (q->start_item_ptr < 0) || (q->items[((q->start_item_ptr + 1 ) % q->maxitems)] == END_PLACE))
        {
            return 0;
        }
    }
    *result = q->items[((q->start_item_ptr + 1 ) % q->maxitems)];
    q->items[((q->start_item_ptr + 1 ) % q->maxitems)] = START_PLACE;
    q->items[q->start_item_ptr] = EMPTYVAL;
    q->numitems--;
    bit_queue_conc_pop(q->bitqueue_conc, *result);
    q->start_item_ptr = ((q->start_item_ptr + 1 ) % q->maxitems);   //This will release other threads waiting to pop.
#pragma omp flush

//    t_end = whenq();
//    q->pop_time += (t_end - t_start);
    return 1;
}



int queue_has_items(queue *q)
{
    return q->numitems;
}

int bit_queue_has_items(bit_queue *bq)
{
    return bq->num_items;
}


/*********----------------------------------------------------**************/

bit_queue *create_bit_queue( int max_items)
{
    bit_queue *bq;
    int num_bit_arrays, i;
    bq = (bit_queue *)malloc(sizeof(bit_queue));
    if (bq == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(0);
    }
    
    //q->maxitems = max_items;
    
    num_bit_arrays = (max_items / BIT_ARRAY_SIZE) + 1;
    bq->bit_arrays = (unsigned long *)malloc(sizeof(unsigned long) * (num_bit_arrays) );
    if (bq->bit_arrays == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(0);
    }
    for (i = 0; i < num_bit_arrays; i++)
        bq->bit_arrays[i] = 0;
    bq->max_bit_arrays = num_bit_arrays;
    bq->num_items = 0;
    return bq;
    
}

int empty_bit_queue(bit_queue *bq)
{
    //int i;
    memset(bq->bit_arrays, 0, sizeof(unsigned long)*bq->max_bit_arrays);
/*    for (i = 0; i < bq->max_bit_arrays; i++)
    {
        bq->bit_arrays[i] = 0;
    }*/
    bq->num_items = 0;
    return 0;
}

unsigned long check_bit_obj_present( bit_queue *bq, int obj )
{
    int index_bit_array = obj/BIT_ARRAY_SIZE;       //index of the bit array to use.
    int set_bit = obj - (index_bit_array * BIT_ARRAY_SIZE);     //The index of bit to be set in the chosen bit array
    unsigned long number_bitset = 0x1 << set_bit;        //The number with the required bit set.
    if ( (set_bit > BIT_ARRAY_SIZE) || (index_bit_array >= bq->max_bit_arrays))
    {
        fprintf(stderr, "Bit manip problem!\n");
        exit(0);
    }
    //will return non-zero if the bit is already set else zero.
    return (bq->bit_arrays[index_bit_array] & number_bitset);
}

int bit_queue_pop( bit_queue *bq, int obj )
{
    int index_bit_array = obj/BIT_ARRAY_SIZE;       //index of the bit array to use.
    int set_bit = obj - (index_bit_array * BIT_ARRAY_SIZE);     //The index of bit to be set in the chosen bit array
    unsigned long number_bit_unset = 0x1 << set_bit;        //The number with the required bit set.
    if (!(bq->bit_arrays[index_bit_array] & number_bit_unset))
        return 1;           //bit not in the queue so nothing to do.
    number_bit_unset = (~number_bit_unset) & MAX_DEB_SEQ_SIZE;
    if ( (set_bit > BIT_ARRAY_SIZE) || (index_bit_array >= bq->max_bit_arrays) )
    {
        fprintf(stderr, "Bit manip problem!\n");
        exit(0);
    }
    bq->bit_arrays[index_bit_array] &= number_bit_unset;  //set the bit corresponding to obj as 0
    bq->num_items--;
    return 1;
    
}

int bit_queue_last_item(bit_queue *bq)
{
    int i = 0, least_bit_set = -1;
    while (i < bq->max_bit_arrays)
    {
        least_bit_set = least_bit(bq->bit_arrays[i]);
        if ((least_bit_set >= 0 ) && (least_bit_set < BIT_ARRAY_SIZE))
            return (least_bit_set + i*BIT_ARRAY_SIZE);
        i++;
    }
    return -1;
}

int least_bit(unsigned long map)
{
    unsigned long x;//unsigned long only_least_bit = 0;
    int least_bit_pos = 0;
    if (map <= 0)
        return -1;
    for (least_bit_pos=0; least_bit_pos < BIT_ARRAY_SIZE; least_bit_pos++)
    {
        x = map >> least_bit_pos;
        if (x & 1)
            return least_bit_pos;
    }
    return -1;

/*    only_least_bit = map & ~(map-1);
    least_bit_pos = DEBRUIJNBITPOS[ ((only_least_bit * DEBSEQ) & MAX_DEB_SEQ_SIZE) >> DEB_SEQ_REM_WINDOW];
    return least_bit_pos;*/
}

int least_bit_deb(unsigned long map)
{
    unsigned long only_least_bit = 0;
    int least_bit_pos = 0;
    if (map <= 0)
        return -1;
     only_least_bit = map & ~(map-1);
     least_bit_pos = DEBRUIJNBITPOS[ ((only_least_bit * DEBSEQ) & MAX_DEB_SEQ_SIZE) >> DEB_SEQ_REM_WINDOW];
     return least_bit_pos;
}





int queue_add_bit( bit_queue *bq, int obj )
{
    int index_bit_array = obj/BIT_ARRAY_SIZE;       //index of the bit array to use.
    int set_bit = obj - (index_bit_array * BIT_ARRAY_SIZE);     //The index of bit to be set in the chosen bit array
    unsigned long number_bitset = 0x1 << set_bit;        //The number with the required bit set.
    if ( (set_bit > BIT_ARRAY_SIZE) || (index_bit_array >= bq->max_bit_arrays) )
    {
        fprintf(stderr, "Bit manip problem!\n");
        exit(0);
    }
    //Setting the required bit in the appropriate bit array.
    if (!(bq->bit_arrays[index_bit_array] & number_bitset))     //Need to set if not present already.
    {
        bq->bit_arrays[index_bit_array] |= number_bitset;
        bq->num_items++;
    }
    return 1;
}

void Or_bit_queue(bit_queue *bq_dest, bit_queue *bq_to_add)
{
    int indexArray = 0;
    unsigned long temp = 0;
    int new_bits_added = 0;
    for (indexArray = 0; indexArray < bq_to_add->max_bit_arrays; indexArray++)
    {
        temp = ~(bq_dest->bit_arrays[indexArray]);
        temp = temp & bq_to_add->bit_arrays[indexArray];
        while (temp > 0)        //Count number of new bits added for this index.
        {
            temp &= (temp-1);
            new_bits_added++;
        }
        bq_dest->bit_arrays[indexArray] |= bq_to_add->bit_arrays[indexArray];
    }
    bq_dest->num_items += new_bits_added;
    return;
}

void destroy_bit_queue(bit_queue *bq)
{
    free(bq->bit_arrays);
    free (bq);
}

void print_bit_queue(bit_queue *bq, char *bit_queue_file)
{
    unsigned long number_bitset = 0x1, current_bitArray;        //The number with the required bit set.
    FILE *f_bitfile;
    int bit_index, count = 0, indexArray = 0;
    
    f_bitfile = fopen( bit_queue_file, "wb" );
    if ( f_bitfile == NULL ) {
        wlog( 1, "Error opening %s!\n", bit_queue_file );
        return;
    }
    for (indexArray = 0; indexArray < bq->max_bit_arrays; indexArray++)
    {
        current_bitArray = bq->bit_arrays[indexArray];
        for (bit_index=0; bit_index < BIT_ARRAY_SIZE; bit_index++)
        {
            number_bitset = 0x1 << bit_index;
            if (current_bitArray & number_bitset)
            {
                fprintf(f_bitfile, "%d:\n", (indexArray * BIT_ARRAY_SIZE) + bit_index );
                count++;
            }
            if (count >= bq->num_items)
                break;
        }
    }
    if (f_bitfile != NULL)
        fclose(f_bitfile);
}

/*
//Returns the partition number in result for which bit is popped.
int queue_pop_bit( bit_queue *q, int *result )
{
    int bit_array_index;
    unsigned long bit_array;
    int intermed_result;
    
    for (bit_array_index =0; bit_array_index < q->max_bit_arrays; bit_array_index++)
    {
        if (q->bit_arrays[bit_array_index] != 0)
        {
            bit_array = q->bit_arrays[bit_array_index];
            q->bit_arrays[bit_array_index] = bit_array & (bit_array - 1);  //Removing LSB - so popped the last partition.

            bit_array = bit_array - (bit_array & (bit_array-1));  //Only the bit indicating the partition number is set now.
            intermed_result = DEBRUIJNBITPOS[ ((bit_array * DEBSEQ) & MAX_DEB_SEQ_SIZE) >> DEB_SEQ_REM_WINDOW];
            
            *result = intermed_result + BIT_ARRAY_SIZE * bit_array_index;    //Adjusting result as per the bit array used.
            return 1;
        }
    }

    fprintf( stderr, "Hey! queue's empty!\n" );
    return 0;
    
}


int bit_queue_has_items(bit_queue *q)
{
    int bit_array_index = 0;
    for (bit_array_index =0; bit_array_index < q->max_bit_arrays; bit_array_index++)
    {
        if (q->bit_arrays[bit_array_index] != 0)
            return 1;
    }
    return 0;
}
*/
