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
    //typedef boost::property_tree::ptree ptree;


    typedef std::string path_type;
    typedef std::string str_type;    

    class ptree
    {
        typedef ptree self_type;
        typedef std::multimap< path_type, self_type > children_type;

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

        template <class Type>
        Type get(const path_type &path,
                 const Type &default_value) const
        {
            auto val = get<Type>(path, default_value, identity<Type>());
            return val;
        }

        template <class Type>
        Type get(const path_type &path,
                 const Type &default_value,
                 identity<Type>) const
        {
            str_type defStr = std::to_string(default_value);
            const str_type res = get_internal(path, defStr);
            if(res.empty())
            {
                return default_value;
            }
            else
            {
                return convert_from_str<Type>(res, identity<Type>());
            }
        }

        template <class Type>
        Type convert_from_str(const str_type &str,
                              identity<Type>) const
        {
            return Type(std::stoi(str));
        }

        template <class>
        bool convert_from_str(const str_type &str,
                              identity<bool>) const
        {
            return (str=="true" || str=="1");
        }


        template <class>
        str_type get(const path_type &path,
               const str_type &default_value,
               identity<str_type>) const
        {
            const str_type res = get_internal(path, default_value);
            return res;
        }

        const self_type &get_child(const path_type &path) const;

        self_type &get_child(const path_type &path);


        const self_type &add_child(const path_type &path,
                                   const self_type &tree);

        str_type &data()
        {
            return mData;
        }

        const str_type &data() const
        {
            return mData;
        }

        const children_type &children() const
        {
            return mChildren;
        }

        children_type &children()
        {
            return mChildren;
        }

    private:

        const self_type &get_child_internal(const path_type &path,
                                            const self_type &tree) const;

        self_type &get_child_internal(const path_type &path,
                                      self_type &tree);

        str_type get_internal(const path_type &path,
                              const str_type &default_value) const;


        children_type mChildren;
        str_type mData;
    };

}


#endif // GSKIT_PROPERTY_TREE
