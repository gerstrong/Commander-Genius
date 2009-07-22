/*
 * CDialog.h
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#ifndef CDIALOG_H_
#define CDIALOG_H_

#define TEXT_LENGTH 256

typedef struct stTextList
{
	char text[TEXT_LENGTH];
	void *nextElement;
}stTextList;

class CDialog {
public:
	CDialog();
	virtual ~CDialog();

	void renderDialog();
	void drawDialogbox(int x1, int y1, int w, int h);
	void setDimensions(int rectx, int recty, int rectw, int recth);

	void addOptionText(const char *text);
	void setOptionText(unsigned int pos, const char *text);
	void addSeparator(void);

	bool setSelection(int value);
	bool setNextSelection();
	bool setPrevSelection();

	int getSelection(void);
	char *getOptionString(unsigned int pos);

	void renderOpenDialogAnimation(int x,int y, int w, int h);
	void animateDialogBox(bool value);

	void setVisible(bool value);

private:
	unsigned int x;
	unsigned int y;
	unsigned int w;
	unsigned int h;

	int number_of_options;
	int selection;
	unsigned int scrollpos;

	stTextList	*OptionTextList;

	int twirlframe;
	int twirltime;

	bool isanimated;
	bool isVisible;

	void freeTextListData(void);
};

#endif /* CDIALOG_H_ */
