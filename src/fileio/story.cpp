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
#include "../include/fileio.h"
#include "../fileio/CExeFile.h"
#include "../CLogFile.h"
#include "../StringUtils.h"

int readStoryText(char **ptext, int episode, const std::string& path)
{
	std::string buf2 = formatPathString(path);
	std::string buf = buf2 + "storytxt.ck" + itoa(episode);

	FILE *fp;
	if((fp=fopen(buf.c_str(),"rt"))==NULL)
	{
		buf = buf2 + "keen" + itoa(episode) + ".exe";

		if((fp=fopen(buf.c_str(),"rb"))!=NULL)
		{
			unsigned char *filebuf;
			int startflag=0, endflag=0; // where story begins and ends!

			CExeFile *ExeFile = new CExeFile(episode, buf2);
			ExeFile->readData();
			filebuf = ExeFile->getData();

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
			delete ExeFile;

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

