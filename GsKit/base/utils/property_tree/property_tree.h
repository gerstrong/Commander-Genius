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
#include <map>

template<typename T>
struct identity { typedef T type; };


namespace GsKit
{
    typedef boost::property_tree::ptree ptree;

/*
    typedef std::string path_type;
    typedef std::string str_type;

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

        auto begin()
        {
            return mChildren.begin();
        }
        auto end()
        {
            return mChildren.end();
        }

        auto begin() const
        {
            return mChildren.begin();
        }
        auto end() const
        {
            return mChildren.end();
        }


        // --------- Add Section ----------

        template<class Type>
        self_type &add(const path_type &path,
                       const Type &value)
        {
            return add<Type>(path, value);
        }

        // --------- Put Section ----------

        self_type &put(const path_type &path,
                       const str_type &value)
        {
            (void)path;
            mData = value;
            return *this;
        }

        self_type &put(const path_type &path,
                       const char *value)
        {
            (void)path;
            mData = value;
            return *this;
        }


        template<class Type>
        self_type &put(const path_type &path,
                       const Type &value)
        {
            return put(path, std::to_string(value));
        }


        // --------- Get Section ----------

        template<class Type>
        Type get(const path_type &path) const
        {
            Type val = get<Type>(path, Type());
            return val;
        }

        template <class T1, class T2>
        T1 get(const path_type &path,
               const T2 &default_value) const
        {
            auto val = get<T1>(path, default_value, identity<T2>());
            return val;
        }



        template <class T1, class T2>
        T1 get(const path_type &path,
               const T2 &default_value,
               identity<T2>) const
        {
            str_type defStr = std::to_string(default_value);
            const str_type res = get_internal(path, defStr);
            if(res.empty())
            {
                return T1(default_value);
            }
            else
            {
                return T1(std::stoi(res));
            }
        }


        template <class T1>
        T1 get(const path_type &path,
               const str_type &default_value,
               identity<str_type>) const
        {
            const str_type res = get_internal(path, default_value);
            //return T1(std::stoi(res));
            return T1();
        }


        str_type get_internal(const path_type &path,
                              const str_type &default_value) const
        {
            // TODO: Code for the path var
            if(mData.empty())
                return default_value;
            else
                return mData;
        }


        const self_type &get_child(const path_type &path) const
        {
            return *this;
        }


        self_type &get_child(const path_type &path)
        {
            return mChildren.find(path)->second;
        }


        const self_type &add_child(const path_type &path,
                                   const self_type &tree)
        {
            //mPtree.add_child(path, tree);
            mChildren[path] = tree;
            mChildren[path].add_child(path, tree);
            return mChildren[path];
        }


        // TODO: Should maybe become private
        boost::property_tree::ptree mPtree;

        std::map<path_type, self_type> mChildren;
        std::string mData;
    };
*/
}


#endif // GSKIT_PROPERTY_TREE
