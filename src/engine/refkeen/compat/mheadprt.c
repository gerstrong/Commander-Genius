/* Reads a map header (either KDreams' MAPHEAD.KDR or a later game's MTEMP.TMP) 
 * from file and outputs its contents in a textual format such that they can be
 * embedded in C sources for array initialization, using the system endianness.
 * This should be compatible with original Keen Dreams and Catacomb 3-D data.
 *
 * WARNING: THIS PROGRAM MAY FAIL TO WORK AS EXPECTED AND DO MORE OR LESS
 * THAN INTENDED.
 *
 * THIS PROGRAM COMES WITH NO WARRANTY, EXPRESSED OR IMPLIED.
 */

#include <stdio.h>
#include <stdint.h>

int main (int argc, char **argv) {
	uint8_t vals[4];
	int counter = 0;
	int mapnum;
	if (argc <= 1) {
		printf("\nShould pass a filename as input via command line\n");
		return 0;
	}
	FILE *fp = fopen(argv[1], "rb");
	if (!fp) {
		printf("\nFailed to open file named \"%s\" for reading\n", argv[1]);
		return 0;
	}
	if (!feof(fp)) {
		if (fread(vals, 2, 1, fp)) {
			printf(" BE_Cross_SwapGroup16LE(0x%02x, 0x%02x)", vals[0], vals[1]);
			printf("\n");
		}
	}
	for (mapnum = 0; mapnum < 100; ++mapnum) {
		if (feof(fp) || !fread(vals, 4, 1, fp)) {
			printf("WARNING: Input file ended unexpectedly\n"); // Yes this is added to the output (stdout) file
			break; // Leave the loop
		}
		printf(" BE_Cross_SwapGroup32LE(0x%02x, 0x%02x, 0x%02x, 0x%02x)", vals[0], vals[1], vals[2], vals[3]);
		if (++counter == 4) {
			counter = 0;
			printf("\n");
		}
	}
	counter = 0;
	while (!feof(fp)) {
		if (!fread(vals, 1, 1, fp)) {
			break;
		}
		printf(" 0x%02x,", vals[0]);
		if (++counter == 16) {
			counter = 0;
			printf("\n");
		}
	}
	fclose(fp);
	printf("\n");
	return 0;
}
