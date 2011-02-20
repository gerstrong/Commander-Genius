/*
 * CRingBuffer.cpp
 *
 *  Created on: 20.02.2011
 *      Author: gerhard
 */

#include "CRingBuffer.h"

RingBuffer::RingBuffer(unsigned int size) :
mp_start(NULL),
mp_cur(NULL),
mp_end(NULL),
m_size(size)
{
	if(m_size == 0)
		return;
	mp_cur = new T[m_size];
	mp_start = mp_cur;
	mp_end = mp_start + m_size;
}

RingBuffer::~RingBuffer() {
	if(mp_start)
		delete [] mp_start;
}

T RingBuffer::getNextElement()
{
	const T data = *mp_cur;
	mp_cur++;
	// hey we are a ring. End of the data? Go to the start!
	this->operator ++;
	if(mp_cur == mp_end )
		mp_cur = mp_start;
	return data;
}

unsigned int RingBuffer::getSlicePtr(T *&data, const unsigned int n_elem)
{
	data = mp_cur;

	if(mp_cur + n_elem <= mp_end) // All elements can be read
		return n_elem;
	else // you cannot read all the elements, return a lower number of readable elements
		return mp_end-mp_cur;
}


void RingBuffer::operator+=(const unsigned int n_elem)
{
	if(mp_cur + n_elem < mp_end)
		mp_cur += n_elem;
	else
	{
		// Hey, we are a ring. End of the data? Go to the start!
		const unsigned int newpos = n_elem-(mp_end-mp_cur);
		mp_cur = mp_start + newpos;
	}
}

void RingBuffer::operator++()
{
	if(mp_cur == mp_end )
		mp_cur = mp_start;
	else
		mp_cur++;
}


