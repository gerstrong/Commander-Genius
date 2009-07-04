/*
 * story.c
 *
 *  Created on: 07.02.2009
 *      Author: gerstrong
 *
 *      This file handles the database that has be loaded in order to get show
 *      the story. It is only a textfile.. though
 */

#include "../keen.h"
#include "../include/fileio/lzexe.h"
#include "../include/fileio.h"
#include "../CLogFile.h"

int readStoryText(char **ptext, int episode, char *path)
{
	FILE *fp;
	char buf[256];
	char buf2[256];

	memset(buf,0,256*sizeof(char));

	formatPathString(buf2,path);

	sprintf(buf,"%sstorytxt.ck%d",buf2,episode);

	if((fp=fopen(buf,"rt"))==NULL)
	{
		sprintf(buf,"%skeen%d.exe",buf2,episode);

		if((fp=fopen(buf,"rb"))!=NULL)
		{
			unsigned char *filebuf;
			int bufsize;

			filebuf = (unsigned char*) malloc(500000*sizeof(unsigned char));

			bufsize = unlzexe(fp, filebuf);

			rewind(fp);

			if ( bufsize == 0 ) // Program was not unpacked, so read it normally
			{
				while(!feof(fp))
				{
					filebuf[bufsize] = getc(fp);
					bufsize++;
				}
			}
			fclose(fp);

			int startflag=0, endflag=0, version=0; // where story begins and ends!

			version = getEXEVersion(episode, bufsize);

			if(episode == 2)
			{
				startflag = 92864;
				endflag = 96088;
			}
			if(episode == 3)
			{
				startflag = 101328;
				endflag = 104435;
			}

			if(startflag == 0 || endflag == 0)
			{
				g_pLogFile->textOut(PURPLE,"Sorry, but your exe-file is not compatible for reading the story.<br>");
			}
			else
			{
				*ptext = (char*) malloc((endflag-startflag+10)*sizeof(char));
				strncpy((*ptext),(char*)filebuf+startflag,(endflag-startflag)*sizeof(char));
			}

			free(filebuf);

			return (endflag-startflag);
		}
		else
			return -1;
	}
	else
	{
		int pos;
		int filesize;

		pos = ftell (fp);
		fseek (fp, 0, SEEK_END);
		filesize = ftell (fp);
		fseek (fp, pos, SEEK_SET);

		pos = 0;

		*ptext = (char*) malloc((filesize+10)*sizeof(char));

		while(!feof(fp))
		{
			(*ptext)[pos] = fgetc(fp);
			pos++;
		}

		fclose(fp);

		return filesize;
	}
}

