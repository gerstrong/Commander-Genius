/*
 * Cunlzexe.h
 *
 *  Created on: 29.12.2009
 *      Author: gerstrong
 *
 *  Embedded Extraction Code for all executable files which are lzexed
 *  Based on the code of Vesselin Bontchev v0.8.
 *  This class is able to extract every exe which is compressed with lzexe
 */

#ifndef CUNLZEXE_H_
#define CUNLZEXE_H_

#include <vector>
#include <stdint.h>

typedef uint16_t WORD_16BIT;
typedef uint8_t BYTE;


class Cunlzexe
{
public:
	Cunlzexe();
	bool decompress(BYTE *data, std::vector<BYTE> &outdata);
	unsigned long HeaderSize();
	virtual ~Cunlzexe();

private:

	struct bitstream{
			BYTE  *pdata;
			WORD_16BIT  buf;
	        BYTE  count;
	};

	void initbits(bitstream *p, BYTE *p_data, unsigned long &inpos);
	int getbit(bitstream *, unsigned long &);
	int rdhead(BYTE *data_ptr ,int *ver);
	void wrhead(std::vector<BYTE> &outdata);
	int reloc90(BYTE *p_data, std::vector<BYTE> &outdata, long fpos);
	int reloc91(BYTE *p_data, std::vector<BYTE> &outdata, long fpos);
	int mkreltbl(BYTE *p_data, std::vector<BYTE> &outdata,int ver);
	int unpack(BYTE *p_input, std::vector<BYTE> &outdata);

	WORD_16BIT get16bitWord(BYTE *p_data);
	void put16bitWord(WORD_16BIT value, std::vector<BYTE> &outdata);

	unsigned long m_headersize;
};

#endif /* CUNLZEXE_H_ */
