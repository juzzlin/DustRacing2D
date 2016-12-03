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

#ifndef MCBBOX3D_HH
#define MCBBOX3D_HH

#include "mcbbox.hh"
#include "mcvector3d.hh"
#include "mctypes.hh"

//! Bounding box structure in 3d
template <typename T>
class MCBBox3d
{
public:

    //! Constructor
    MCBBox3d() :
        m_x1(0), m_y1(0), m_z1(0),
        m_x2(0), m_y2(0), m_z2(0)
    {}

    //! Constructor
    MCBBox3d(T x1, T y1, T z1, T x2, T y2, T z2) :
        m_x1(x1), m_y1(y1), m_z1(z1),
        m_x2(x2), m_y2(y2), m_z2(z2)
    {}

    //! Copy constructor
    template <typename U>
    MCBBox3d(const MCBBox3d<U> & r) :
        m_x1(r.x1()), m_y1(r.y1()), m_z1(r.z1()),
        m_x2(r.x2()), m_y2(r.y2()), m_z2(r.z2())
    {}

    //! Move constructor
    template <typename U>
    MCBBox3d(const MCBBox3d<U> && r) :
        m_x1(r.x1()), m_y1(r.y1()), m_z1(r.z1()),
        m_x2(r.x2()), m_y2(r.y2()), m_z2(r.z2())
    {}

    //! Get the leftmost x
    inline T x1() const {return m_x1;}

    //! Get the lower y
    inline T y1() const {return m_y1;}

    //! Get the rightmost x
    inline T x2() const {return m_x2;}

    //! Get the higher y
    inline T y2() const {return m_y2;}

    //! Get the lower z
    inline T z1() const {return m_z1;}

    //! Get the higher z
    inline T z2() const {return m_z2;}

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

    //! Set the lower z
    void setZ1(T v) {m_z1 = v;}

    //! Set the higher z
    void setZ2(T v) {m_z2 = v;}

    //! Assignment operator
    template <typename U>
    MCBBox3d<T> & operator =(const MCBBox3d<U> & r)
    {
        if (reinterpret_cast<void *>(const_cast<MCBBox3d<U> *>(&r)) != reinterpret_cast<void *>(this))
        {
            m_x1 = r.x1();
            m_x2 = r.x2();
            m_y1 = r.y1();
            m_y2 = r.y2();
            m_z1 = r.z1();
            m_z2 = r.z2();
        }

        return *this;
    }

    //! Move assignment operator
    template <typename U>
    MCBBox3d<T> & operator =(const MCBBox3d<U> && r)
    {
        m_x1 = r.x1();
        m_x2 = r.x2();
        m_y1 = r.y1();
        m_y2 = r.y2();
        m_z1 = r.z1();
        m_z2 = r.z2();

        return *this;
    }

    //! Test if MCBBox3d intersects another one
    template <typename U>
    bool intersects(const MCBBox3d<U> & r) const
    {
        if (r.x1() >= m_x2) {return false;}
        if (r.x2() <= m_x1) {return false;}
        if (r.y1() >= m_y2) {return false;}
        if (r.y2() <= m_y1) {return false;}
        if (r.z1() >= m_z2) {return false;}
        if (r.z2() <= m_z1) {return false;}
        return true;
    }

    //! Test if MCBBox3d contains another one
    template <typename U>
    bool contains(const MCBBox3d<U> & r) const
    {
        return
            r.x1() >= m_x1 && r.x2() <= m_x2 &&
            r.y1() >= m_y1 && r.y2() <= m_y2 &&
            r.z1() >= m_z1 && r.z2() <= m_z2;
    }

    //! Test if MCBBox3d contains given point
    template <typename U>
    bool contains(const MCVector3d<U> & r) const
    {
        return
            r.i() >= m_x1 && r.i() <= m_x2 &&
            r.j() >= m_y1 && r.j() <= m_y2 &&
            r.k() >= m_z1 && r.k() <= m_z2;
    }

    //! Translate.
    template <typename U>
    void translate(const MCVector3d<U> & r)
    {
        m_x1 += r.i();
        m_y1 += r.j();
        m_z1 += r.k();
        m_x2 += r.i();
        m_y2 += r.j();
        m_z2 += r.k();
    }

    MCBBox<T> toBBox() const
    {
        return MCBBox<T>(m_x1, m_y1, m_x2, m_y2);
    }

private:

    //! Vertex coordinates
    T m_x1, m_y1, m_z1, m_x2, m_y2, m_z2;
};

typedef MCBBox3d<MCFloat> MCBBox3dF;

#endif // MCBBOX3D_HH
