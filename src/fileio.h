#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

char loadstrings(const char *fname);
unsigned int fgeti(FILE *fp);
unsigned long fgetl(FILE *fp);
void formatPathString(char *output, const char *path);

#ifdef __cplusplus
}
#endif
