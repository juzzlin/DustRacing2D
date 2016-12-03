// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#ifndef MCRECYCLER_HH
#define MCRECYCLER_HH

#include "mctypes.hh"

#include <vector>
#include <stack>

/*! \class MCRecycler
 *  \brief Generic object recycler.
 *
 *  This class can be used to store and recycle objects. It acts as a
 *  cache. Objects to be stored must have a default constructor.
 *  "Freed" Objects are moved to a free-list for fast creation later.
 *  All created Objects are automatically deleted in the destructor of MCRecycler. */
template <typename T>
class MCRecycler
{
public:

    //! Constructor.
    MCRecycler();

    //! Destructor.
    ~MCRecycler();

    /*! Return a new object of type T. If the free-list is empty
     *  then the default constructor is used. The newly created object is
     *  automatically deleted when Recycler gets of out scope. */
    T * newObject();

    /*! \brief Move given object to the free list.
     *         MCRecycler takes the ownership. */
    void freeObject(T * p);

    //! Move all active Objs to the free list
    void freeObjects();

private:

    MCUint deleteFreeObjects();

    MCUint deleteObjects();

    typedef std::vector<T *> ObjectPool;
    ObjectPool m_objs;

    typedef std::stack<T *> FreeObjectPool;
    FreeObjectPool m_freeObjs;
};

template <typename T>
MCRecycler<T>::MCRecycler()
{}

template <typename T>
T * MCRecycler<T>::newObject()
{
    T * p = nullptr;
    if (m_freeObjs.size()) {
        p = m_freeObjs.top();
        m_freeObjs.pop();
    } else {
        p = new T;
        m_objs.push_back(p);
    }
    return p;
}

template <typename T>
MCUint MCRecycler<T>::deleteObjects()
{
    MCUint count = 0;
    for (auto iter = m_objs.begin(); iter != m_objs.end(); iter++) {
        delete *iter;
        count++;
    }
    m_objs.clear();
    return count;
}

template <typename T>
void MCRecycler<T>::freeObject(T * p)
{
    m_freeObjs.push(p);
}

template <typename T>
void MCRecycler<T>::freeObjects()
{
    for (auto iter = m_objs.begin(); iter != m_objs.end(); iter++) {
        m_freeObjs.push(*iter);
    }
}

template <typename T>
MCRecycler<T>::~MCRecycler()
{
    deleteObjects();
}

#endif // MCRECYCLER_HH
