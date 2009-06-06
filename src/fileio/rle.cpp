/*
 * rle.c
 *
 *  Created on: 29.01.2009
 *      Author: gerstrong
 */

#include <stdio.h>
#include "../funcdefs.h"


int unRLEW(FILE *fp, unsigned int *filebuf)
{

	int t,i, howmany, cursize, finsize;
  /*
1.) If implemented, get the first dword in the file, [Final Length]
2.) If [Length of data so far] < [Final Length] then:
 3.) Get a word
 2.) Is this word $FEFE?
     -> If yes;
        Get the next two words (Word1 and Word2)
        Copy Word1 [Word2] times
        Move forward three words and got to 2.)
     -> If no;
        Copy the word
        Move forward a word and go to 2.)
   */

	cursize = 0;

	rewind(fp);


	while(!feof(fp)) // Detect, if the file is really RLEW compressed!
	{
		t = fgeti(fp);
		if(t == 0xFEFE)
		{
			cursize = 1;
			break;
		}
	}


	if(cursize == 0)
	{
		return -1; // This file is not RLEW compressed!
	}
	rewind(fp);

	finsize = fgeti(fp);


    while( cursize < finsize )
    {
     t = fgeti(fp);
     if (t == 0xFEFE)
     {
       howmany = fgeti(fp);
       t = fgeti(fp);
       for(i=0;i<howmany;i++)
       {
    	   filebuf[cursize] = t;
    	   cursize++;
       }
     }
     else
     {
    	 filebuf[cursize] = t;
    	 cursize++;
     }
    }

    return cursize;
}
