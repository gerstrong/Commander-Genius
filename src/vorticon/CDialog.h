/*
 * CDialog.h
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#ifndef CDIALOG_H_
#define CDIALOG_H_

#include <string>

struct stTextList
{
	std::string text;
	stTextList *nextElement;
};

class CDialog {
public:
	CDialog();
	virtual ~CDialog();

	void renderDialog();
	void drawDialogbox(int x1, int y1, int w, int h);
	void setDimensions(int rectx, int recty, int rectw, int recth);

	void addOptionText(const std::string& text);
	void setOptionText(unsigned int pos, const std::string& text);
	void addSeparator(void);

	bool setSelection(int value);
	bool setNextSelection();
	bool setPrevSelection();

	int getSelection(void);
	std::string getOptionString(unsigned int pos);

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
