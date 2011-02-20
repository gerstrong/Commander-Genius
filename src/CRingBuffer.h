/*
 * CRingBuffer.h
 *
 *  Created on: 20.02.2011
 *      Author: gerstrong
 *
 *  This small class handles a data structure called ring buffer.
 *  This is very useful for audio implementations, because in many
 *  cases you have a buffer which is read and converted to another buffer
 *  which is the waveform for the audio hardware. As those buffers
 *  have different sizes, there are a least two pointers and/or one
 *  size variable we need to handle the data transfer correctly.
 *
 *  Those pointers must be verified all the in order to avoid overflows
 *  The ring buffer implementation ensures that this never happens.
 *
 *  This class manages those pointers and also the memory allocation, deallocation.
 *
 *  Now there are two uses of this class. One is similar to the list structure.
 *  You read one element and if it gets to the end of the buffer
 *  it will return to the start.
 *
 *  The second choice is to read arrays from the buffer.
 *  In that case you have to read slices. (See below)
 */

#ifndef CRINGBUFFER_H_
#define CRINGBUFFER_H_

template <typename T>
class RingBuffer {
public:
	RingBuffer(unsigned int size);
	~RingBuffer();

	/**
	 * This will get the next Element in the ring. Similar to the front function of std::list, but it's a ring.
	 * This will copy the data to the data type variable you are using, so please don't use too big datastructures for this
	 * if you need fast code.
	 */
	T getNextElement();

	/**
	 * This function will give you a pointer the current data position within the ring
	 * It will also tell you how many Elements of that data type (n_elem) you can use to read.
	 * If it could read all the elements it will return the same number you entered.
	 * If not, it will return the number of read elements, which will be smaller.
	 * That happens, when the ring buffer gets near to the pointers end and might overflow.
	 * This function will avoid such danger. If the function didn't read the desired elements, just use it again
	 * with the number of remaining elements.
	 *
	 * This function will not read data, because it might slow down the code. For reading and writing the
	 * using the read pointer you must care yourself.
	 */
	unsigned int getSlicePtr(T *&data, const unsigned int n_elem);

	/**
	 * Just makes the current pointer in the ring buffer go further...
	 */
	void operator+=(const unsigned int n_elem);

	/**
	 * increment pointer by one element or go the start if cur == end ptr
	 */
	void operator++();

private:
	T *mp_start, *mp_cur, *mp_end;
	unsigned int m_size;
};

#endif /* CRINGBUFFER_H_ */
