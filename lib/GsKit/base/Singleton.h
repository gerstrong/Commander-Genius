/*
 * CSingleton.h
 *
 *  Created on: 19.04.2009
 *      Author: gerstrong
 *
 *  Modified on: 16.12.2013
 *      The Singleton is now a static object. Similar approach to Doom3 afaik. I think this makes the stuff a bit faster.
 *      Disadvantage is of course the control in the memory, but in our cases we need those singletons all the time.
 *      with another class you still can make it dynamic of you really want that...
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_


template <class T>
class GsSingleton
{	
public:
    static T& get();

    GsSingleton() {}

private:    
    GsSingleton( const GsSingleton& );
};

template <class T>
T& GsSingleton<T>::get()
{
    static T instance;
    return instance;
}

#endif /* SINGLETON_H_ */
