/* Reads a sequence of 24-bit ints from a file with Little-Endian ordering
 * and outputs them in a textual format such that they can be embedded in
 * C sources for array initialization, using the system endianness.
 *
 * WARNING: THIS PROGRAM MAY FAIL TO WORK AS EXPECTED AND DO MORE OR LESS
 * THAN INTENDED.
 *
 * THIS PROGRAM COMES WITH NO WARRANTY, EXPRESSED OR IMPLIED.
 */

#include <stdio.h>
#include <stdint.h>

int main (int argc, char **argv) {
	uint8_t vals[3];
	int counter = 0;
	if (argc <= 1) {
		printf("\nShould pass a filename as input via command line\n");
		return 0;
	}
	FILE *fp = fopen(argv[1], "rb");
	if (!fp) {
		printf("\nFailed to open file named \"%s\" for reading\n", argv[1]);
		return 0;
	}
	while (!feof(fp)) {
		if (!fread(vals, sizeof(vals), 1, fp)) {
			break;
		}
		printf(" BE_Cross_SwapGroup24LE(0x%02x, 0x%02x, 0x%02x)", vals[0], vals[1], vals[2]);
		if (++counter == 5) {
			counter = 0;
			printf("\n");
		}
	}
	fclose(fp);
	printf("\n");
	return 0;
}
