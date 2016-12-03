// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2009 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCBBOX_HH
#define MCBBOX_HH

#include <algorithm>
#include "mcvector2d.hh"
#include "mcvector3d.hh"
#include "mctypes.hh"

//! Bounding box structure in 2d
template <typename T>
class MCBBox
{
public:

    //! Constructor
    MCBBox() :
        m_x1(0), m_y1(0),
        m_x2(0), m_y2(0)
    {}

    //! Constructor
    MCBBox(const MCVector2d<T> & v1_, T width_, T height_) :
        m_x1(v1_.i()), m_y1(v1_.j()),
        m_x2(m_x1 + width_), m_y2(m_y1 + height_)
    {}

    //! Constructor
    MCBBox(T x1_, T y1_, T x2_, T y2_) :
        m_x1(x1_), m_y1(y1_),
        m_x2(x2_), m_y2(y2_)
    {}

    //! Copy constructor
    template <typename U>
    MCBBox(const MCBBox<U> & r) :
        m_x1(r.x1()), m_y1(r.y1()),
        m_x2(r.x2()), m_y2(r.y2())
    {}

    //! Move constructor
    template <typename U>
    MCBBox(const MCBBox<U> && r) :
        m_x1(r.x1()), m_y1(r.y1()),
        m_x2(r.x2()), m_y2(r.y2())
    {}

    //! Get the leftmost x
    inline T x1() const {return m_x1;}

    //! Get the lower y
    inline T y1() const {return m_y1;}

    //! Get the rightmost x
    inline T x2() const {return m_x2;}

    //! Get the higher y
    inline T y2() const {return m_y2;}

    //! Get the leftmost vertex
    const MCVector2d<T> v1() const {return MCVector2d<T>(m_x1, m_y1);}

    //! Get the rightmost vertex
    const MCVector2d<T> v2() const {return MCVector2d<T>(m_x2, m_y2);}

    //! Get width
    inline T width() const {return m_x2 - m_x1;}

    //! Get height
    inline T height() const {return m_y2 - m_y1;}

    //! Set the leftmost x
    void setX1(T v) {m_x1 = v;}

    //! Set the lower y
    void setY1(T v) {m_y1 = v;}

    //! Set the rightmost x
    void setX2(T v) {m_x2 = v;}

    //! Set the higher y
    void setY2(T v) {m_y2 = v;}

    //! Assignment operator
    template <typename U>
    MCBBox<T> & operator =(const MCBBox<U> & r)
    {
        if (reinterpret_cast<void *>(const_cast<MCBBox<U> *>(&r)) != reinterpret_cast<void *>(this))
        {
            m_x1 = r.x1();
            m_x2 = r.x2();
            m_y1 = r.y1();
            m_y2 = r.y2();
        }

        return *this;
    }

    //! Move assignment operator
    template <typename U>
    MCBBox<T> & operator =(const MCBBox<U> && r)
    {
        m_x1 = r.x1();
        m_x2 = r.x2();
        m_y1 = r.y1();
        m_y2 = r.y2();

        return *this;
    }

    //! Return the MCBBox the union of two boxes
    static MCBBox<T> unionBBox(const MCBBox<T> & l, const MCBBox<T> & r)
    {
        return MCBBox<T>(std::min(l.m_x1, r.m_x1), std::min(l.m_y1, r.m_y1),
            std::max(l.m_x2, r.m_x2), std::max(l.m_y2, r.m_y2));
    }

    //! Test if MCBBox intersects another one
    template <typename U>
    bool intersects(const MCBBox<U> & r) const
    {
        if (r.x1() >= m_x2) {return false;}
        if (r.x2() <= m_x1) {return false;}
        if (r.y1() >= m_y2) {return false;}
        if (r.y2() <= m_y1) {return false;}
        return true;
    }

    //! Test if MCBBox contains another one
    template <typename U>
    bool contains(const MCBBox<U> & r) const
    {
        return r.x1() >= m_x1 && r.x2() <= m_x2 && r.y1() >= m_y1 && r.y2() <= m_y2;
    }

    //! Test if MCBBox contains given point
    template <typename U>
    bool contains(const MCVector2d<U> & r) const
    {
        return r.i() >= m_x1 && r.i() <= m_x2 && r.j() >= m_y1 && r.j() <= m_y2;
    }

    //! Test if MCBBox contains given point
    template <typename U>
    bool contains(const MCVector3d<U> & r) const
    {
        return r.i() >= m_x1 && r.i() <= m_x2 && r.j() >= m_y1 && r.j() <= m_y2;
    }

    //! Translate.
    template <typename U>
    void translate(const MCVector2d<U> & r)
    {
        m_x1 += r.i();
        m_y1 += r.j();
        m_x2 += r.i();
        m_y2 += r.j();
    }

    //! Translate.
    template <typename U>
    void translate(const MCVector3d<U> & r)
    {
        m_x1 += r.i();
        m_y1 += r.j();
        m_x2 += r.i();
        m_y2 += r.j();
    }

private:

    //! Vertex coordinates
    T m_x1, m_y1, m_x2, m_y2;
};

typedef MCBBox<MCFloat> MCBBoxF;

#endif // MCBBOX_HH
