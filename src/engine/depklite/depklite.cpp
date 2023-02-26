// depklite, a derivative of OpenTESArena's ExeUnpacker.
// Used for decompressing DOS executables compressed with PKLITE.
// Based on ReflectionHLE: https://github.com/ReflectionHLE/ReflectionHLE

#include <cstdlib>
#include <cstring>
#include <base/TypeDefinitions.h>
#include <base/GsLogging.h>
#include <cstring>

#include "depklite.h"

// Replacement for Debug::check
static void Debug_check(bool expression, const char *msg)
{
	if (!expression)
	{
        gLogging << msg;
	}
}

/*** C++11 lambda emulation ***/

// Lambda for getting the next byte from compressed data.
typedef struct
{
	const uint8_t *compressedStart;
	int byteIndex;
} GetNextByte_Data;

static uint8_t getNextByte(GetNextByte_Data *data)
{
	return data->compressedStart[data->byteIndex++];
}

// Lambda for getting the next bit in the theoretical bit stream.
typedef struct
{
	uint16_t bitArray;
	int bitsRead;
	GetNextByte_Data getNextByteDat;
} GetNextBit_Data;

static bool getNextBit(GetNextBit_Data *data)
{
	const bool bit = (data->bitArray & (1 << data->bitsRead)) != 0;
	data->bitsRead++;

	// Advance the bit array if done with the current one.
	if (data->bitsRead == 16)
	{
		data->bitsRead = 0;

		// Get two bytes in little endian format.
		const uint8_t byte1 = getNextByte(&data->getNextByteDat);
		const uint8_t byte2 = getNextByte(&data->getNextByteDat);
		data->bitArray = byte1 | (byte2 << 8);
	}

	return bit;
};


/*** A simple binary tree for retrieving a decoded value, given a vector of bits. ***/

typedef struct Node
{
	int left;
	int right;
	int value;
} Node;

typedef Node BitTree;

// Returns a decoded value in the tree. Note that rather than getting
// an input vector of bits, this gets a pointer to a bits fetcher which
// is repeatedly called, once per bit.
static const int BitTree_get(const BitTree *bt, GetNextBit_Data *getNextBitDatPtr)
{
	const Node *node = bt;

	// Walk the tree.
	while (true)
	{
		const bool bit = getNextBit(getNextBitDatPtr);
		// Decide which branch to use.
		if (bit)
		{
			// Right.
			Debug_check(node->right != 0, "Bit Tree - No right branch.\n");
			node += node->right;
		}
		else
		{
			// Left.
			Debug_check(node->left != 0, "Bit Tree - No left branch.\n");
			node += node->left;
		}
		// Check if it's a leaf.
		if ((node->left == 0) && (node->right == 0))
		{
			return node->value;
		}
	}
}

#define ST(l,r) { l, r, -1 } // Node which isn't a leaf
#define LF(v) { 0, 0, v } // Leaf

// Bit table from pklite_specification.md, section 4.3.1 "Number of bytes".
// The decoded value for a given vector is (index + 2) before index 11, and
// (index + 1) after index 11.
//
// The array is illustrated as a binary tree, in which
// every non-leaf node is shown with its subtrees as follows
// (although the root node is "reversed" i.e., L and R are swapped):
//
// N
// 	R
// L
static const Node bitTree1[] =
{
	ST(4,1), // "Reversed" node
		ST(1,2),
			LF(2),
		LF(3),
	ST(1,6),
		ST(1,2),
			LF(4),
		ST(1,2),
			LF(5),
		LF(6),
	ST(1,6),
		ST(1,2),
			LF(7),
		ST(1,2),
			LF(8),
		LF(9),
	ST(1,6),
		ST(1,2),
			LF(10),
		ST(1,2),
			LF(11),
		LF(12),
	ST(1,6),
		ST(1,2),
			LF(25),
		ST(1,2),
			LF(13),
		LF(14),
	ST(1,6),
		ST(1,2),
			LF(15),
		ST(1,2),
			LF(16),
		LF(17),
	ST(1,6),
		ST(1,2),
			LF(18),
		ST(1,2),
			LF(19),
		LF(20),
	ST(1,4),
		ST(1,2),
			LF(21),
		LF(22),
	ST(1,2),
		LF(23),
	LF(24),
};

// Bit table from pklite_specification.md, section 4.3.2 "Offset".
// The decoded value for a given vector is simply its index.
//
// The array is illustrated in a similar manner as before.
static const Node bitTree2[] =
{
	ST(2,1), // "Reversed" node
		LF(0),
	ST(1,12),
		ST(1,4),
			ST(1,2),
				LF(1),
			LF(2),
		ST(1,4),
			ST(1,2),
				LF(3),
			LF(4),
		ST(1,2),
			LF(5),
		LF(6),
	ST(1,18),
		ST(1,8),
			ST(1,4),
				ST(1,2),
					LF(7),
				LF(8),
			ST(1,2),
				LF(9),
			LF(10),
		ST(1,4),
			ST(1,2),
				LF(11),
			LF(12),
		ST(1,2),
			LF(13),
		ST(1,2),
			LF(14),
		LF(15),
	ST(1,16),
		ST(1,8),
			ST(1,4),
				ST(1,2),
					LF(16),
				LF(17),
			ST(1,2),
				LF(18),
			LF(19),
		ST(1,4),
			ST(1,2),
				LF(20),
			LF(21),
		ST(1,2),
			LF(22),
		LF(23),
	ST(1,8),
		ST(1,4),
			ST(1,2),
				LF(24),
			LF(25),
		ST(1,2),
			LF(26),
		LF(27),
	ST(1,4),
		ST(1,2),
			LF(28),
		LF(29),
	ST(1,2),
		LF(30),
	LF(31),
};

bool depklite_unpack(const std::vector<unsigned char> compData, std::vector<unsigned char> &decdata)
{

    gLogging << "depklite - Checking header...\n";

    // Check if the file is in PKLITE format
    // Is it an exe file
    if ((compData[0] == 0x4d && compData[1] == 0x54) ||
        (compData[0] == 0x54 && compData[1] == 0x4d))
    {
        gLogging << "Warning. This does not look like an EXE file!\n";
        return false;
    }

    if ( memcmp(compData.data()+0x1E, "PKLITE", 6) != 0 )
    {
        gLogging << "Not a PKLITE compressed file!\n";
        return false;
    }



    gLogging << "depklite - unpacking...\n";

    const int32_t fileSize = compData.size();
	// Offset of compressed data in the executable, which is 800
	// for Keen Dreams v1.00 (KDREAMS.EXE), and 752 for The Elder Scrolls Arena (A.EXE).
	const int compressedDataOffset = 800;

	Debug_check(fileSize >= compressedDataOffset, "depklite - Input file is unexpectedly too small!\n");

    uint8_t *compressedStart = new uint8_t[fileSize - compressedDataOffset];
	Debug_check(compressedStart, "depklite - Out of memory!\n");

    //fseek(fp, compressedDataOffset, SEEK_SET);
    //fread(compressedStart, fileSize - compressedDataOffset, 1, fp);
    memcpy(compressedStart, compData.data()+compressedDataOffset, fileSize - compressedDataOffset);

	GetNextBit_Data getNextBitData;

	getNextBitData.getNextByteDat.compressedStart = compressedStart;

	// Buffer for the decompressed data (also little endian).
    decdata.assign(fileSize*10, 0);

	// Current position for inserting decompressed data.
    unsigned char *decompPtr = decdata.data();
    auto decompBuffStart = decompPtr;

	// A 16-bit array of compressed data.
    getNextBitData.bitArray = GETWORD(getNextBitData.getNextByteDat.compressedStart);

	// Offset from start of compressed data (start at 2 because of the bit array).
	getNextBitData.getNextByteDat.byteIndex = 2;

	// Number of bits consumed in the current 16-bit array.
	getNextBitData.bitsRead = 0;

	// Continually read bit arrays from the compressed data and interpret each bit. 
	// Break once a compressed byte equals 0xFF in duplication mode.
	while (true)
	{
		// Decide which mode to use for the current bit.
		if (getNextBit(&getNextBitData))
		{
			// "Duplication" mode.
			// Calculate which bytes in the decompressed data to duplicate and append.
			int copy = BitTree_get(bitTree1, &getNextBitData);

			// Calculate the number of bytes in the decompressed data to copy.
			uint16_t copyCount = 0;

			// Check for the special bit vector case "011100".
			if (copy == 25) // Special value
			{
				// Read a compressed byte.
				const uint8_t encryptedByte = getNextByte(&getNextBitData.getNextByteDat);

				if (encryptedByte == 0xFE)
				{
					// Skip the current bit.
					continue;
				}
				else if (encryptedByte == 0xFF)
				{
					// All done with decompression.
					break;
				}
				else
				{
					// Combine the compressed byte with 25 for the byte count.
					copyCount = encryptedByte + 25;
				}
			}
			else
			{
				// Use the decoded value from the first bit table.
				copyCount = copy;
			}

			// Calculate the offset in decompressed data. It is a two byte value.
			// The most significant byte is 0 by default.
			uint8_t mostSigByte = 0;

			// If the copy count is not 2, decode the most significant byte.
			if (copyCount != 2)
			{
				// Use the decoded value from the second bit table.
				mostSigByte = BitTree_get(bitTree2, &getNextBitData);
			}

			// Get the least significant byte of the two bytes.
			const uint8_t leastSigByte = getNextByte(&getNextBitData.getNextByteDat);

			// Combine the two bytes.
			const uint16_t offset = leastSigByte | (mostSigByte << 8);

			// Finally, duplicate the decompressed data using the calculated offset and size.
			// Note that memcpy or even memmove is NOT the right way,
			// since overlaps are possible
			unsigned char *duplicateBegin = decompPtr - offset;
			unsigned char *duplicateEnd = duplicateBegin + copyCount;
			for (unsigned char *p = duplicateBegin; p < duplicateEnd; ++p, ++decompPtr)
			{
				*decompPtr = *p;
			}
		}
		else
		{
			// Usage of "Decryption" mode isn't required for Keen Dreams v1.00
			// Get next byte and then append it onto the decompressed data.
			*decompPtr++ = getNextByte(&getNextBitData.getNextByteDat);
		}
	}

    decdata.resize(decompPtr-decompBuffStart);

    delete [] compressedStart;

	return true;
}
