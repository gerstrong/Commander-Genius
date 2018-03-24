#ifndef VIDEOASPECT_H
#define VIDEOASPECT_H

static const char* aspectList[] =
{
"disabled",
"4:3",
"16:9",
"16:10",
"5:4",
};

const unsigned int NUM_ASPECTS = sizeof(aspectList)/sizeof(char*);

#endif // VIDEOASPECT_H
