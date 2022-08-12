/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageSwapper(int pid, int* plist)
{
    int inOutList[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 4; i++)
        inOutList[plist[i]] = 1;
    for (int i = 0; i < 20; i++)
    {
        if (inOutList[i])
            pagein(pid, i);
        else
            pageout(pid, i);
    }
}

void pageit(Pentry q[MAXPROCESSES]) // Called anytime somthing interesting happens
{

    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int tick = 1; // artificial time

    /* Local vars */
    int pagetmp;

    /* 
        Treat like a Markov Process. Swap in 3 pages k with highest P(X_{n + 1} = k | X_n = i)
        These transition probabilities were determined from the pages.csv file
        Note that for the programs that we were given, only 15 pages were needed for each process
        as opposed to the maximum of 20, but I have coded in pages 16-20 under the assumption that 
        these will likely be accessed linearly like the higher numbered pages in tended to be in
        the simulation results I saw in see.R
        I can show the method that I used to get these transition probabilities on request :)
    */

    for(int pid = 0; pid < MAXPROCESSES; pid++)
    {
        if(q[pid].active)
        {
            int neededInPages[4] = {0,0,0,0};
            pagetmp = q[pid].pc / PAGESIZE;
            
            switch (pagetmp)
            {
                /*
                    Note that technically the Markov Transition probabilities said that I should have special cases where we load in page 4 when pages 0,1, and 2 are loaded, but this actually can cause some extra overhead because that page is already loaded when the next item is accessed in the sequence since that page is already loaded at 3, and p_04 and p_14 were incredibly tiny, so it was better to not include them
                */
                case 3:
                    neededInPages[0] = 0;
                    neededInPages[1] = 3;
                    neededInPages[2] = 4;
                    neededInPages[3] = 10;
                    break;
                case 10:
                    neededInPages[0] = 0;
                    neededInPages[1] = 9;
                    neededInPages[2] = 10;
                    neededInPages[3] = 11;
                    break;
                case 11:
                    neededInPages[0] = 0;
                    neededInPages[1] = 9;
                    neededInPages[2] = 11;
                    neededInPages[3] = 12;
                    break;
                case 12:
                    neededInPages[0] = 0;
                    neededInPages[1] = 9;
                    neededInPages[2] = 12;
                    neededInPages[3] = 13;
                    break;
                case 13:
                    neededInPages[0] = 0;
                    neededInPages[1] = 9;
                    neededInPages[2] = 13;
                    neededInPages[3] = 14;
                    break;
                default:
                    neededInPages[0] = 0;
                    neededInPages[1] = pagetmp;
                    neededInPages[2] = (pagetmp + 1) % 20;
                    neededInPages[3] = (pagetmp + 2) % 20;
                    break;
            }
            pageSwapper(pid, neededInPages);
        }
    }
    /* advance time for next pageit iteration */
    tick++;
}
