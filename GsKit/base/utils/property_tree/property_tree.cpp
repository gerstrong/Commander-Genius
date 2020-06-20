#include "property_tree.h"

#include <stdexcept>

namespace GsKit
{

ptree::self_type &ptree::add_internal(const path_type &path,
                                      const str_type &value)
{
    path_type::size_type pos = path.find(".");
    const path_type segment = path.substr(0,pos);

    self_type newChild;

    // If there are subpaths, we need to add the value to the subnode
    if(pos != path.npos)
    {
        const path_type left = path.substr(pos+1);
        newChild.add_internal(left, value);
    }
    else
    {
        newChild.data() = value;
    }

    auto insertedChild = children().insert({segment, newChild});
    return insertedChild->second;
}

ptree::self_type &ptree::put_internal(const path_type &path,
                                      const str_type &value)
{
    path_type::size_type pos = path.find(".");
    const path_type segment = path.substr(0,pos);

    //self_type newChild;

    // If there are subpaths, we need to add the value to the subnode
    if(pos != path.npos)
    {
        const path_type left = path.substr(pos+1);

        auto searchChild = children().find(segment);
        if (searchChild != children().end())
        {
            searchChild->second.put_internal(left, value);
            return searchChild->second;
        }
        else
        {
            self_type newChild;
            newChild.put_internal(left, value);

            auto putChild = children().insert({segment, newChild});
            return putChild->second;
        }
    }
    else
    {
        auto searchChild = children().find(segment);
        if (searchChild != children().end())
        {
            searchChild->second.data() = value;
            return searchChild->second;
        }
        else
        {
            self_type newChild;
            newChild.data() = value;
            auto putChild = children().insert({segment, newChild});
            return putChild->second;
        }
    }
}


const ptree::self_type &ptree::add_child(const path_type &path,
                                         const self_type &tree)
{
    path_type::size_type pos = path.find(".");

    // No more of the subpath then this is the last segment
    if(pos == path.npos)
    {
        auto lastChild = children().insert({path, tree});
        return lastChild->second;
    }

    const path_type segment = path.substr(0,pos);
    const path_type left = path.substr(pos+1);

    auto child = children().insert({segment, tree});
    return add_child(left, child->second);

}

ptree::self_type &ptree::get_child_internal(const path_type &path,
                                            self_type &tree)
{
    try {
        path_type::size_type pos = path.find(".");

        // No more of the subpath then this is the last segment
        if(pos == path.npos)
        {
            auto &subChildren = tree.children();

            if(subChildren.empty())
                throw std::runtime_error("no children");

            const auto &childPairIt = subChildren.find(path);

            if(childPairIt == subChildren.end())
                throw std::runtime_error("children not found");

            return childPairIt->second;
        }

        const path_type segment = path.substr(0,pos);
        const path_type left = path.substr(pos+1);

        auto &children = tree.mChildren;
        if(children.empty())
            throw;

        auto it = children.find(segment);

        if(it == children.end())
            throw;

        auto &child = it->second;
        return get_child_internal(left, child);
    }
    catch(std::exception const&  ex)
    {
        throw ex;
    }
    catch (...) {
        throw;
    }
}

const ptree::self_type &ptree::get_child_internal(const path_type &path,
                                                  const self_type &tree) const
{
    try {
        return const_cast<ptree::self_type&>
                (*this).get_child_internal(path,
                                           const_cast<ptree::self_type&>(tree));

    }
    catch(std::exception const&  ex)
    {
        throw ex;
    }
    catch (...) {
        throw;
    }
}


const ptree::self_type &ptree::get_child(const path_type &path) const
{
    try {
        return get_child_internal(path, *this);
    }
    catch(std::exception const&  ex)
    {
        throw ex;
    }
    catch (...) {
        throw;
    }

}

ptree::self_type &ptree::get_child(const path_type &path)
{
    try {
    return get_child_internal(path, *this);
    }
    catch(std::exception const&  ex)
    {
        throw ex;
    }
    catch (...) {
        throw;
    }

}



str_type ptree::get_internal(const path_type &path,
                             const str_type &default_value) const
{
    auto &child = get_child(path);
    auto &data = child.data();

    if(data.empty())
    {
        return default_value;
    }
    else
    {
        return data;
    }
}


}

