/*
 GsKit

 property tree utilities

 code under LGPL
 created 30-05-2020
 by Gerhard Stein
 */
#ifndef GSKIT_PROPERTY_TREE
#define GSKIT_PROPERTY_TREE

#include <string>
#include <map>

template<typename T>
struct identity { typedef T type; };


namespace GsKit
{
    typedef std::string path_type;
    typedef std::string str_type;    

    class ptree
    {
        typedef ptree self_type;
        typedef std::multimap< path_type, self_type > children_type;

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
            return add<Type>(path, value, identity<Type>());
        }


        template<class Type>
        self_type &add(const path_type &path,
                       const Type &value,
                       identity<Type>)
        {
            const str_type strVal =
                    convert_to_str(value, identity<Type>());
            return add_internal(path, strVal);
        }


        template<class>
        self_type &add(const path_type &path,
                       const str_type &value,
                       identity<str_type>)
        {
            return add_internal(path, value);
        }

        template<class>
        self_type &add(const path_type &path,
                       const char *&value,
                       identity<char*>)
        {
            return add_internal(path, value);
        }


        // --------- Put Section ----------

        self_type &put(const path_type &path,
                       const str_type &value)
        {
            return put_internal(path, value);
        }

        self_type &put(const path_type &path,
                       const char *value)
        {
            return put_internal(path, value);
        }

        self_type &put(const path_type &path,
                       const bool &value)
        {
            return put_internal(path, value ? "true" : "false");
        }

        template<class Type>
        self_type &put(const path_type &path,
                       const Type &value)
        {
            return put_internal(path, std::to_string(value));
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
            try
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
            catch (...)
            {
                return default_value;
            }
        }

        // ------------- Converters ---------------------

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

        str_type convert_to_str(const bool &value,
                                identity<bool>) const
        {
            return value ? "true" : "false";
        }

        str_type convert_to_str(const str_type &value,
                                identity<str_type>) const
        {
            return value;
        }

        str_type convert_to_str(const char value[1],
                                identity<char[1]>) const
        {
            return value;
        }



        // ---------------- Get --------------------------


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

        self_type &add_internal(const path_type &path,
                                const str_type &value);

        self_type &put_internal(const path_type &path,
                                const str_type &value);


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
