/*
 * CDialog.cpp
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#include "CDialog.h"

#include <stdio.h> // Must be changed, when Graphics Class is implemented
#include "../funcdefs.h" // This class should not depend on those headers
#include <string.h>

#include "../keen.h"
#include "../sdl/CTimer.h"
#include "../include/gamedo.h"
#include "../CGraphics.h"

#include <iostream>
using namespace std;


#define FONT_TWIRL              9
#define FONT_UPARROW		   15
#define FONT_DOWNARROW		   19
#define TWIRL_TIME             25
#define TWIRL_NUM_FRAMES        5

CDialog::CDialog() {
	x = y = w = h = 0;

	number_of_options = 0;
	selection = -1;
	twirlframe = 0;
	twirltime = 0;
	scrollpos = 0;

	isanimated = false;
	isVisible = false;

	OptionTextList = NULL;
}

CDialog::~CDialog() {

	// If there is a list of optional text data, it must be freed!
	if(OptionTextList != NULL)
		freeTextListData();
}
void CDialog::freeTextListData(void)
{
	stTextList* curTextptr;
	stTextList* prevTextptr;

	curTextptr = OptionTextList;
	prevTextptr = NULL;

	while(OptionTextList->nextElement)
	{
		if(curTextptr->nextElement == NULL)
		{
			delete curTextptr;
			prevTextptr->nextElement = NULL;
			curTextptr = OptionTextList;
		}
		else
		{
			prevTextptr = curTextptr;
			curTextptr = (stTextList*)curTextptr->nextElement;
		}
	}
	delete OptionTextList;
}

void CDialog::addSeparator(void)
{
	addOptionText("");
}

void CDialog::addOptionText(const char *text)
{
	char buf[TEXT_LENGTH];
	memset(buf,0,TEXT_LENGTH);
	// This algorithm is similar to one pointer session and
	// list implementation. TextList is the head.
	if(OptionTextList == NULL)
	{
		OptionTextList = new stTextList;
		OptionTextList->nextElement = NULL;

		memset(OptionTextList->text,0,TEXT_LENGTH);

		strcpy(buf,text);

		unsigned int length;
		length = strlen(buf);

		// before the text is copied, check if that string is too long.
		if(length > w-4)
		{
			copy(text,text+w-7,OptionTextList->text);
			strcat(OptionTextList->text,"...");
		}
		else
		{
			strcpy(OptionTextList->text,text);
		}

		number_of_options = 1;
		selection = 0;
	}
	else
	{
		stTextList *curTextList;

		curTextList = OptionTextList;

		while (curTextList->nextElement != NULL)
		{
			curTextList = (stTextList*) curTextList->nextElement;
		}

		curTextList->nextElement = new stTextList;

		curTextList = (stTextList*) curTextList->nextElement;

		memset(curTextList->text,0, TEXT_LENGTH);

		number_of_options++;

		strcpy(buf,text);

		unsigned int length;
		length = strlen(buf);

		// before the text is copied, check if that string is too long.
		if(length > w-4)
		{
			copy(text,text+w-7,curTextList->text);
			strcat(curTextList->text,"...");
		}
		else
		{
			strcpy(curTextList->text,text);
		}
		curTextList->nextElement = NULL;
	}
}

void CDialog::setOptionText(unsigned int pos, const char *text)
{
	unsigned int i;
	stTextList *curTextList;

	curTextList = OptionTextList;

	for(i=0 ; i<pos ; i++)
		curTextList = (stTextList*) curTextList->nextElement;

	memset(curTextList->text,0,TEXT_LENGTH);
	strcpy(curTextList->text,text);
}

void CDialog::setDimensions(int rectx, int recty, int rectw, int recth)
{
	x = rectx;
	y = recty,
	w = rectw-1;
	h = recth;
}

void CDialog::setVisible(bool value) {	isVisible = value; }

void CDialog::renderDialog()
{
	unsigned int i;
	stTextList *curTextList;

	if(!isVisible) return;

	curTextList = OptionTextList;

	// Open the Dialog animation
	if(isanimated)
		renderOpenDialogAnimation(x,y,w,h);

	// draw the empty Dialog
	drawDialogbox(x,y,w,h);

	// draw the objects (text. items, etc.)

	// scroll as many times necessary
	for(i=0 ; i<scrollpos ; i++)
		curTextList = (stTextList*) curTextList->nextElement;

	i=0;

	while(curTextList != NULL)
	{
		g_pGraphics->sb_font_draw((unsigned char*)(curTextList->text), (x+3)<<3, (y+i+1)<<3);
		curTextList = (stTextList*) curTextList->nextElement;
		i++;
		if(i >= h-2)
			break;
	}


	// Draw the twirl at the right position
	// Render the smooth twirl
	if(twirltime < TWIRL_TIME)
		twirltime++;
	else
	{
		twirlframe++;
		twirltime=0;
	}

	if(twirlframe > TWIRL_NUM_FRAMES)
		twirlframe=0;

	g_pGraphics->sb_drawCharacter((x+1)<<3, (y+selection-scrollpos+1)<<3, FONT_TWIRL+twirlframe);

	// Now draw the up and down arrows if the list is going to be scrolled
	if(scrollpos > h-5)
		g_pGraphics->sb_drawCharacter((x+w-1)<<3, (y+1)<<3, FONT_UPARROW);

	if(scrollpos < (number_of_options-h+2) && number_of_options > (int)(h-2))
		g_pGraphics->sb_drawCharacter((x+w-1)<<3, (y+h-2)<<3, FONT_DOWNARROW);
}

// The original Game had a open Dialog Animation. This is what that function does...
void CDialog::animateDialogBox(bool value){	isanimated = value; }
void CDialog::renderOpenDialogAnimation(int x,int y, int w, int h)
{
	if(!isanimated || !isVisible)
		return;

	int timer=0;

	int mx, my, mw, mh;

	mw = mh = 0;

	// Calculate the center
	mx = x + (w>>1);
	my = y + (h>>1);

	do
	{
		if(timer == 10)
		{
			if(mx > x)
				mx--;
			if(my > y)
				my--;
			if(mh < h)
			{
				if(mh+1 == h)
					mh++;
				else
					mh+=2;
			}
			if(mw < w)
			{
				if(mw+1 == w)
					mw++;
				else
					mw+=2;
			}

			timer = 0;
		}


		if(mx <= x && my <= y && mh >= h && mw >= w)
			break;

		drawDialogbox(mx,my,mw,mh);

	    gamedo_AnimatedTiles();
	    gamedo_frameskipping(NULL);
	    g_pTimer->SpeedThrottle();

	    timer++;
	}while(1);

	isanimated = false;
}

char *CDialog::getOptionString(unsigned int pos)
{
	unsigned int i;
	stTextList *curTextList;

	curTextList = OptionTextList;

	for(i=0 ; i<pos ; i++)
		curTextList = (stTextList*) curTextList->nextElement;

	return curTextList->text;
}

bool CDialog::setSelection(int value)
{
	if(value < number_of_options && value >= 0)
	{
		selection = value;

		if(selection < (int)scrollpos)
			scrollpos = selection;

		if(selection >= (int)(scrollpos+h-2) )
			scrollpos = selection-h+3;

		return true;
	}
	return false;
}

int CDialog::getSelection(void)
{
	return selection;
}

bool CDialog::setNextSelection()
{
	int i=0;
	if(selection+1 < number_of_options)
	{
		while(strcmp(getOptionString(selection+i+1),"") == 0)
			i++;

		selection += i;
	}

	return setSelection(selection+1);
}

bool CDialog::setPrevSelection()
{
	int i=0;
	if(selection-1 > 0)
	{
		while(strcmp(getOptionString(selection-i-1),"") == 0)
			i++;

		selection -= i;
	}

	return setSelection(selection-1);
}

void CDialog::drawDialogbox(int x1, int y1, int w, int h)
{
	int px,py,pi,pj;

	g_pGraphics->sb_drawCharacter(x1*8, y1*8, 1);
	g_pGraphics->sb_drawCharacter((x1+w)*8, y1*8, 3);
	for(px=(x1*8)+8,pi=0;pi<w-1;pi++)
	{
		g_pGraphics->sb_drawCharacter(px, y1*8, 2);
		px+=8;
	}
	py=(y1+1)*8;
	for(pj=0;pj<h-2;pj++)
	{
		for(px=(x1*8),pi=0;pi<=w;pi++)
		{
			if (pi==0) g_pGraphics->sb_drawCharacter(px, py, 4);
			else if (pi==w) g_pGraphics->sb_drawCharacter(px, py, 5);
			else g_pGraphics->sb_drawCharacter(px, py, ' ');
			px+=8;
		}
		py+=8;
	}
    for(px=(x1*8),pi=0;pi<=w;pi++)
    {
    	if (pi==0) g_pGraphics->sb_drawCharacter(px, py, 6);
    	else if (pi==w) g_pGraphics->sb_drawCharacter(px, py, 8);
    	else g_pGraphics->sb_drawCharacter(px, py, 7);
    	px+=8;
    }
}
