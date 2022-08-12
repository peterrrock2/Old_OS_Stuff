/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proctmp;
    int pagetmp;
    int oldest = 0;
    int remove_ind = 0;

    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
		timestamps[proctmp][pagetmp] = 0; 
	    }
	}
	initialized = 1;
    }

    for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++) // run through all of the processes
    {
        if(q[proctmp].active) // check if the process is active
        {
            pagetmp = q[proctmp].pc / PAGESIZE; // page of current process

            if ( !q[proctmp].pages[pagetmp] && !pagein(proctmp, pagetmp) ) // page is swapped out or swapping out
            {
                oldest = tick; /* current time. Initialized so that when I go through the processes to find the oldest
                                  I will pick up all the processes*/

                for(int i = 0; i < MAXPROCESSES; i++) // read through to find the newest page
                {
                    if (q[proctmp].pages[i] && timestamps[proctmp][i] < oldest) // if older than current oldest
                    {
                        oldest = timestamps[proctmp][i];
                        remove_ind = i; // index of the page to be swapped out
                    }
                }

                pageout(proctmp, remove_ind);
                break;
            }
        }
    }


    /* advance time for next pageit iteration */
    tick++;
} 
