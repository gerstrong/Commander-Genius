/* LZ.C
 This file contains the functions which decompress the graphics
 data from Keen 1.
 */
#include "CLogFile.h"

#define LZ_STARTBITS        9
#define LZ_ERRORCODE        256
#define LZ_EOFCODE          257
#define LZ_DICTSTARTCODE    258

#define LZ_MAXSTRINGSIZE    72

unsigned char *lz_outbuffer;

typedef struct stLZDictionaryEntry
{
	int stringlen;
	unsigned char string[LZ_MAXSTRINGSIZE];
} stLZDictionaryEntry;

stLZDictionaryEntry **lzdict;

// reads a word of length numbits from file lzfile.
unsigned int lz_readbits(FILE *lzfile, unsigned char numbits, unsigned char reset)
{
	static int mask, byte;
	unsigned char bitsread;
	unsigned int dat;
	/*unsigned int posit, mult*/;
	
	if (reset)
	{
		mask = 0;
		byte = 0;
		return 0;
	}
	
	bitsread = 0;
	dat = 0;
	do
	{
        if (!mask)
        {
			byte = fgetc(lzfile);
			mask = 0x80;
        }
		
        if (byte & mask)
        {
			dat |= 1 << ((numbits - bitsread) - 1);
        }
		
        mask >>= 1;
        bitsread++;
	} while(bitsread<numbits);
	
	return dat;
}

// writes dictionary entry 'entry' to the output buffer
void lz_outputdict(int entry)
{
	int i;
	
	for(i=0;i<lzdict[entry]->stringlen;i++)
	{
		*lz_outbuffer = lzdict[entry]->string[i];
		lz_outbuffer++;
	}
}

// decompresses LZ data from open file lzfile into buffer outbuffer
// returns nonzero if an error occurs
char lz_decompress(FILE *lzfile, unsigned char *outbuffer)
{
	unsigned int i;
	unsigned int numbits;
	unsigned int decsize;
	unsigned short maxdictcodewords;
	unsigned int maxdictsize;
	unsigned int dictindex, maxdictindex;
	unsigned int lzcode,lzcode_save,lastcode;
	char addtodict;
	
	// Get the decompressed file-size
	decsize = fgetc(lzfile);
	decsize += fgetc(lzfile) << 8;
	decsize += fgetc(lzfile) << 16;
	decsize += fgetc(lzfile) << 24;
	
	// Get the length of the maximum dictionary size
	
	maxdictcodewords = fgetc(lzfile);
	maxdictcodewords += fgetc(lzfile) << 8;
	
	maxdictsize = ((1<<maxdictcodewords)+1);
	
	lzdict = new stLZDictionaryEntry*[maxdictsize];
	// allocate memory for the LZ dictionary
	for(i=0;i<maxdictsize;i++)
	{
		//lzdict[i] = (stLZDictionaryEntry*) malloc(sizeof(stLZDictionaryEntry));
		lzdict[i] = new stLZDictionaryEntry;
		if (!lzdict[i])
		{
			g_pLogFile->textOut("lz_decompress(): unable to allocate memory for dictionary!<br>");
			return 1;
		}
	}
	
	/* initilize the dictionary */
	
	// entries 0-255 start with a single character corresponding
	// to their entry number
	for(i=0;i<256;i++)
	{
		lzdict[i]->stringlen = 1;
		lzdict[i]->string[0] = i;
	}
	// 256+ start undefined
	for(i=256;i<maxdictsize;i++)
	{
		lzdict[i]->stringlen = 0;
	}
	
	// reset readbits
	lz_readbits(NULL, 0, 1);
	
	// set starting # of bits-per-code
	numbits = LZ_STARTBITS;
	maxdictindex = (1 << numbits) - 1;
	
	// point the global pointer to the buffer we were passed
	lz_outbuffer = outbuffer;
	
	// setup where to start adding strings to the dictionary
	dictindex = LZ_DICTSTARTCODE;
	addtodict = 1;                    // enable adding to dictionary
	
	// read first code
	lastcode = lz_readbits(lzfile, numbits, 0);
	lz_outputdict(lastcode);
	do
	{
		// read the next code from the compressed data stream
		lzcode = lz_readbits(lzfile, numbits, 0);
		lzcode_save = lzcode;
		
		if (lzcode==LZ_ERRORCODE || lzcode==LZ_EOFCODE)
			break;
		
		// if the code is present in the dictionary,
		// lookup and write the string for that code, then add the
		// last string + the first char of the just-looked-up string
		// to the dictionary at dictindex
		
		// if not in dict, add the last string + the first char of the
		// last string to the dictionary at dictindex (which will be equal
		// to lzcode), then lookup and write string lzcode.
		
		if (lzdict[lzcode]->stringlen==0)
			// code is not present in dictionary
			lzcode = lastcode;
		
		if (addtodict)     // room to add more entries to the dictionary?
		{
			// copies string lastcode to string dictindex, then
			// concatenates the first character of string lzcode.
			for(i=0 ; i< (unsigned int) lzdict[lastcode]->stringlen ; i++)
				lzdict[dictindex]->string[i] = lzdict[lastcode]->string[i];
			
			lzdict[dictindex]->string[i] = lzdict[lzcode]->string[0];
			lzdict[dictindex]->stringlen = (lzdict[lastcode]->stringlen + 1);
			
			// ensure we haven't overflowed the buffer
			if (lzdict[dictindex]->stringlen >= (LZ_MAXSTRINGSIZE-1))
			{
				g_pLogFile->ftextOut("lz_decompress(): lzdict[%d]->stringlen is too long...max length is %d<br>", dictindex, LZ_MAXSTRINGSIZE);
				return 1;
			}
			
			dictindex++;
			if (dictindex >= maxdictindex)
			{ // no more entries can be specified with current code bit-width
				if (numbits < maxdictcodewords)
				{  // increase width of codes
					numbits++;
					maxdictindex = (1 << numbits) - 1;
				}
				else
				{
					// reached maximum bit width, can't increase.
					// use the final entry (4095) before we shut off
					// adding items to the dictionary.
					if (dictindex>=(maxdictsize-1)) addtodict = 0;
				}
			}
		}
		
		// write the string associated with the original code read.
		// if the code wasn't present, it now should have been added.
		lz_outputdict(lzcode_save);
		
		lastcode = lzcode_save;
	} while(1);
	
	/* free the memory used by the LZ dictionary */
	for(i=0;i<maxdictsize;i++)
		delete lzdict[i];
	
	delete [] lzdict;
	
	return 0;
}

