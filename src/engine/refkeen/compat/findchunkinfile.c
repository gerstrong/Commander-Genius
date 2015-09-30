/* Program used for quickly finding an occurrence of one file's contents
 * (argv[2]) in another one (argv[1]).
 *
 * WARNING: THIS PROGRAM MAY FAIL TO WORK AS EXPECTED AND DO MORE OR LESS
 * THAN INTENDED.
 *
 * THIS PROGRAM COMES WITH NO WARRANTY, EXPRESSED OR IMPLIED.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int32_t BE_Cross_FileLengthFromHandle(FILE *fp)
{
	long int origOffset = ftell(fp);
	fseek(fp, 0, SEEK_END);
	long int len = ftell(fp);
	fseek(fp, origOffset, SEEK_SET);
	return len;
}

int main(int argc, char **argv) {
	if (argc < 3)
	{
		printf("Use command like this: %s <src1> <src2>\n", argv[0]);
		return;
	}
	FILE *fp1 = fopen(argv[1], "rb");
	if (!fp1)
	{
		printf("Cannot open %s for reading\n", argv[1]);
		return 0;
	}
	FILE *fp2 = fopen(argv[2], "rb");
	if (!fp2)
	{
		fclose(fp2);
		printf("Cannot open %s for reading\n", argv[2]);
		return 0;
	}
	int32_t fsize1 = BE_Cross_FileLengthFromHandle(fp1), fsize2 = BE_Cross_FileLengthFromHandle(fp2);
	unsigned char *buf1 = (unsigned char *)malloc(fsize1), *buf2 = (unsigned char *)malloc(fsize2);
	fread(buf1, fsize1, 1, fp1);
	fread(buf2, fsize2, 1, fp2);
	fclose(fp1);
	fclose(fp2);

	int buf1loopvar, buf2loopvar;
	for (buf1loopvar = 0; buf1loopvar <= fsize1-fsize2; ++buf1loopvar)
	{
		for (buf2loopvar = 0; buf2loopvar < fsize2; ++buf2loopvar)
		{
			if (buf1[buf1loopvar+buf2loopvar] != buf2[buf2loopvar])
				break;
		}
		if (buf2loopvar == fsize2)
		{
			printf("Match found! %s is in %s at offset %d (0x%x).\n", argv[2], argv[1], buf1loopvar, buf1loopvar);
			break;
		}
	}
	free(buf1);
	free(buf2);
	return 0;
}
