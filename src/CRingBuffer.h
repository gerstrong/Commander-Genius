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

// TODO: It is more of a container. We should rename that to Ring or RingComposite or similar.

template <typename T>
class RingBuffer {
public:

	RingBuffer():
        mp_start(nullptr),
        mp_cur(nullptr),
        mp_end(nullptr),
		m_size(0)
	{}

	~RingBuffer()
	{
		if(!empty())
			clear();
	}



    void operator=(RingBuffer &&second)
    {
        mp_start = second.mp_start;
        mp_cur = second.mp_start;
        mp_end = second.mp_end;
        m_size = second.m_size;

        second.mp_start = nullptr;
        second.mp_cur = nullptr;
        second.mp_end = nullptr;
        second.m_size = 0;
    }

	/**
	 * Allocates memory for the Ring buffer
	 */
	bool reserve(unsigned int size)
	{
		if(!empty())
		{
			// TODO: throw exception here! This must never happen!
			return false;
		}

		m_size = size;
		if(m_size == 0)
			return false;
		mp_cur = new T[m_size];
		mp_start = mp_cur;
		mp_end = mp_start + m_size;

		return true;
	}


	/**
	 * Clears the buffer without checking if ever was reserved. Be careful!
	 */
	void clear()
	{
		delete [] mp_start;
		mp_start = NULL;
		m_size = 0;
	}


	/**
	 * Just checks and tells if the ring buffer is actually empty or not reserved
	 */
	bool empty()
	{
		return (m_size == 0 || mp_start == NULL);
	}

	/**
	 * Just tell whether the buffer of the as at the initial pointer which must be the same as at the end, because
	 * until this call, the pointer was rewound.
	 */
	bool atStart()
	{	return (mp_cur == mp_start);	}

	/**
	 * Although a ring doesn't have a start, this function will set the pointer to first data chunk that on what it
	 * was initialized
	 */
	void gotoStart()
	{	mp_cur=mp_start;	}

	/**
	 * Just return the absolute start of the pointer.
	 */
	T *getStartPtr()
	{	return mp_start;	}

	/**
	 * Just return the absolute end of the pointer.
	 */
	T *getLastElem()
	{	return mp_end-1;	}


	/**
	 * This will get the next Element in the ring. Similar to the front function of std::list, but it's a ring.
	 * This will copy the data to the data type variable you are using, so please don't use too big datastructures for this
	 * if you need fast code.
	 */
	T getNextElement()
	{
		const T data = *mp_cur;
		this->operator ++();
		return data;
	}


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
	unsigned int getSlicePtr(T *&data, const unsigned int n_elem)
	{
		data = mp_cur;

		if(mp_cur + n_elem <= mp_end) // All elements can be read
			return n_elem;
		else // you cannot read all the elements, return a lower number of readable elements
			return mp_end-mp_cur;
	}


	/**
	 * Just makes the current pointer in the ring buffer go further...
	 */
	void operator+=(const unsigned int n_elem)
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

	/**
	 * increment pointer by one element or go the start if cur == end ptr
	 */
	void operator++()
	{
		mp_cur++;

		if( mp_cur == mp_end )
			mp_cur = mp_start;
	}


private:
	T *mp_start, *mp_cur, *mp_end;
	unsigned int m_size;
};

#endif /* CRINGBUFFER_H_ */
