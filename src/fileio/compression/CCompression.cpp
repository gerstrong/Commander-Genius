/*
 * CCompression.cpp
 *
 *  Created on: 31.05.2010
 *      Author: gerstrong
 */

#include "CCompression.h"

CCompression::CCompression()
{
	// TODO Auto-generated constructor stub

}

void CCompression::unpack(std::vector<byte>& dst, std::vector<byte>& src, word key )
{
    /*std::vector<byte> tmp;
    longword decarmacksize, derlesize;

    printf( "Plane Total Compressed Size:     %7d bytes\n", src.size() );

    decarmacksize = (src.at(1)<<8)+src.at(0);     // Byte has to be swapped
    printf( "Plane Uncompressed Carmack Size: %7d bytes (0x%.2X%.2X)", decarmacksize, src[1], src[0] );
    DeCarmarkize( tmp, src );

    if( decarmacksize == tmp.size() )
    {
        printf( " Size Verified Correct\n" );
        derlesize = (tmp[0]<<8)+tmp[1];           // Byte already swapped
        printf( "Plane Uncompressed RLE Size:     %7d bytes (0x%.2X%.2X)", derlesize, tmp[0], tmp[1] );
        DeRLEize( dst, tmp, key );

        if( derlesize == dst.size() )
        {
            printf( " Size Verified Correct\n" );
        }
        else
        {
            printf( "\nERROR Plane Uncompress RLE Size Failed: Actual %7d bytes Expected %7d bytes\n", dst.size(), derlesize );
            dst.clear();
        }
    }
    else
    {
        printf( "\nERROR Plane Uncompress Carmack Size Failed: Actual %7d bytes Expected %7d bytes\n", tmp.size(), decarmacksize );;
        dst.clear();
    }*/
}

CCompression::~CCompression()
{
	// TODO Auto-generated destructor stub
}
