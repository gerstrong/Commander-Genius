/*
 * CSingleton.h
 *
 *  Created on: 19.04.2009
 *      Author: gerstrong
 *
 *  Modified on: 23.01.2011
 *  	GetNoPtrChk get. Only use that function in extreme cases.
 */

#ifndef CSINGLETON_H_
#define CSINGLETON_H_

#include <stdio.h>

template <class T>
class CSingleton {
	
	
protected:
	static T *m_pSingleton; // Static Object
	
public:
	//CSingleton();

	inline static T* GetNoPtrChk()
	{
		return (m_pSingleton);
	}

	inline static T* Get()
	{
		if(!m_pSingleton)
			m_pSingleton = new T;
		
		return (m_pSingleton);
	}
	
	static void Del()
	{

		if(m_pSingleton)
		{
			delete (m_pSingleton);
            m_pSingleton = nullptr;
		}
	}

	virtual ~CSingleton() // Unused, because this class is never constructed!!
	{}
};

template <class T>
T* CSingleton<T>::m_pSingleton = nullptr;

#endif /* CSINGLETON_H_ */
