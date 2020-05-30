/*
 GsKit

 property tree utilities

 code under LGPL
 created 30-05-2020
 by Gerhard Stein
 */
#ifndef GSKIT_PROPERTY_TREE
#define GSKIT_PROPERTY_TREE

#include <boost/property_tree/ptree.hpp>
#include <string>

namespace GsKit
{
    typedef boost::property_tree::ptree ptree;

/*
    typedef std::string path_type;

    class ptree
    {
        typedef ptree self_type;

        template <class ItType>
        class iteratorTpl
        {
        public:

            bool operator!=(const iteratorTpl &it) const
            {
                return (mIterator != it.mIterator);
            }

            iteratorTpl& operator++()
            {
                mIterator++;
                return *this;
            }

            iteratorTpl& operator*()
            {
                return *this;
            }


            ItType mIterator;
        };

        typedef iteratorTpl<boost::property_tree::ptree::iterator> iterator;
        typedef iteratorTpl<boost::property_tree::ptree::const_iterator> const_iterator;
        //typedef iteratorTpl<boost::property_tree::ptree::const_iterator> reverse_iterator;
        //typedef iteratorTpl<boost::property_tree::ptree::const_reverse_iterator> const_reverse_iterator;

        iterator iterator_begin;
        iterator iterator_end;
        const_iterator const_iterator_begin;
        const_iterator const_iterator_end;

    public:

        iterator begin()
        {
            return iterator_begin;
            //return mPtree.begin();
        }
        const_iterator begin() const
        {
            return const_iterator_begin;
            //return mPtree.begin();
        }
        iterator end()
        {
            return iterator_end;
            //return mPtree.end();
        }
        const_iterator end() const
        {
            return const_iterator_end;
            //return mPtree.end();
        }

        template<class Type>
        self_type &add(const path_type &path, const Type &value)
        {
            return add<Type>(path, value);
        }


        template<class Type>
        self_type &put(const path_type &path, const Type &value)
        {
            //return mPtree.put<Type>(path, value);
        }

        template <class Ch>
        Ch get(const path_type &path, const Ch *default_value) const
        {
            //return mPtree.get<Ch>(path, default_value);
        }

        template <class Ch>
        Ch get(const path_type &path, const Ch &default_value) const
        {
            return mPtree.get<Ch>(path, default_value);
        }

        template<class Type>
        Type get(const path_type &path) const
        {
            return mPtree.get<Type>(path);
        }

        self_type &get_child(const path_type &path)
        {
            return *this;
        }

        const self_type &get_child(const path_type &path) const
        {
            return *this;
        }

        // TODO: Should maybe become private
        boost::property_tree::ptree mPtree;
    };
*/
}


#endif // GSKIT_PROPERTY_TREE
