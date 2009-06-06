/*
 * lzexe.c
 *
 *  Created on: 24.01.2009
 *      Author: gerstrong
 */

#include <stdlib.h>
#include <stdio.h>

int get_bit(int *p_bit_count, FILE **fin)
{
  static unsigned short bits;
  int bit;

  bit = bits & 1;
  (*p_bit_count)--;

  if ((*p_bit_count) <= 0)
  {
    bits = getc(*fin) | getc(*fin) << 8;

    if ((*p_bit_count) == -1) /* special case for first bit word */
    {
      bit = bits & 1;
      bits >>= 1;
    }

    (*p_bit_count) += 16;
  }
  else
    bits >>= 1;

  return bit;
}

int getEXEVersion(int episode, int bufsize)
{
    switch (bufsize)
    {
		case 99762:
			if(episode != 1)
				return -1;
			else
				return 110;
		case 99972:
			if(episode != 1)
				return -1;
			else
				return 131;

		case 398:
			if(episode != 1)
				return -1;
			else
				return 134;

		case 118114:
			if(episode != 2)
				return -1;
			else
				return 100;

		case 118160:

			if(episode != 2)
				return -1;
			else
				return 131;

		case 127086:

			if(episode != 3)
				return -1;
			else
				return 100;

		case 127104:
			if(episode != 3)
				return -1;
			else
				return 131;

		default: return -2;
    }

}

// return how much was unpacked or zero if nothing was unpacked
int unlzexe(FILE *fin, unsigned char *outbuffer)
{
	  int bit_count;
	  short offset;
	  int pos, repeat;

	  pos = 0;
	  bit_count = 0;

      /* skip header */
		  fseek(fin, 32, SEEK_SET);

		  while (1)
		  {

			  if (get_bit(&bit_count, &fin))
			  {
				  outbuffer[pos++] = getc(fin);
			  }
			  else
			  {
				  if (get_bit(&bit_count, &fin))
				  {
					  unsigned char tmp[2];
					  fread(tmp, 1, 2, fin);
					  repeat = (tmp[1] & 0x07);

					  offset = ((tmp[1] & ~0x07) << 5) | tmp[0] | 0xE000;

					  if (repeat == 0)
					  {
						  repeat = getc (fin);


						  if (repeat == 0)
							  break;
						  else if (repeat == 1)
							  continue;
						  else
							  repeat++;
					  }
					  else
						  repeat += 2;
				  }
				  else
				  {
					  repeat = ((get_bit(&bit_count, &fin) << 1) | get_bit(&bit_count, &fin)) + 2;
					  offset = getc(fin) | 0xFF00;
				  }

				  while (repeat > 0)
				  {
					  outbuffer[pos] = outbuffer[pos + offset];
					  pos++;
					  repeat--;
				  }
			}
		  }

		  return pos;
}
