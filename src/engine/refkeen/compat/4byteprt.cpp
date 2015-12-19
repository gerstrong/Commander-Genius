/* Reads a sequence of 32-bit ints from a file with Little-Endian ordering
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
	uint32_t val;
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
		if (!fread(&val, sizeof(val), 1, fp)) {
			break;
		}
		printf(" 0x%08x,", val);
		if (++counter == 4) {
			counter = 0;
			printf("\n");
		}
	}
	fclose(fp);
	printf("\n");
	return 0;
}
