// MCVector2d -- A generalized 2d-vector class
// Copyright (C) 2009 Jussi Lind <jussi.lind@iki.fi>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General
// Public License along with this library; if not, write to the
// Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301 USA 
//

#ifndef MCVECTOR2D_HH
#define MCVECTOR2D_HH

#include "mctypes.hh"

#include <cmath>
#include <iostream>
#include <limits>

/*! 2-dimensional vector template. Template parameter represents
 *  the data type of the components. 
 *
 *  Examples: 
 *
 *  MCVector2d<int> a(1, 1);
 *  MCVector2d<int> b(-1, 2);
 *  MCVector2d<int> z = a + b;
 *
 *  // Cross product
 *  cout << a % b << endl;
 *
 *  // Dot product
 *  cout << a.dot(b) << endl;
 *
 */ 

template <typename T>
class MCVector2d
{
public:

    //! Constructor
    MCVector2d();

    //! Constructor
    explicit MCVector2d(T i, T j = 0);

    //! Copy constructor
    template <typename U>
    MCVector2d(const MCVector2d<U> & r);

    //! Move constructor
    template <typename U>
    MCVector2d(const MCVector2d<U> && r);

    //! Destructor
    inline ~MCVector2d() {}

    //! Assignment
    template <typename U>
    MCVector2d<T> & operator = (const MCVector2d<U> & r);

    //! Move assignment
    template <typename U>
    MCVector2d<T> & operator = (const MCVector2d<U> && r);

    //! Cross product. Returns the resulting component.
    template <typename U>
    T operator % (const MCVector2d<U> & r) const;

    //! Dot product
    template <typename U>
    T dot(const MCVector2d<U> & r) const;

    //! Component product
    template <typename U>
    MCVector2d<T> comp(const MCVector2d<U> & r) const;

    //! Component product + store
    template <typename U>
    MCVector2d<T> & compStore(const MCVector2d<U> & r);
    
    //! Multiplication
    template <typename S>
    MCVector2d<T> operator * (S n) const;

    //! Vector multiplication
    template <typename U>
    MCVector2d<T> operator * (const MCVector2d<U> & r) const;

    //! Division
    template <typename S>
    MCVector2d<T> operator / (S n) const;

    //! Vector division
    template <typename U>
    MCVector2d<T> operator / (const MCVector2d<U> & r) const;

    //! Vector sum
    template <typename U>
    MCVector2d<T> operator + (const MCVector2d<U> & r) const;

    //! Vector subtraction
    template <typename U>
    MCVector2d<T> operator - (const MCVector2d<U> & r) const;

    //! *=
    template <typename S>
    MCVector2d<T> & operator *= (S n);

    //! *=
    template <typename U>
    MCVector2d<T> & operator *= (const MCVector2d<U> & r);

    //! /=
    template <typename S>
    MCVector2d<T> & operator /= (S n);

    //! /=
    template <typename U>
    MCVector2d<T> & operator /= (const MCVector2d<U> & r);

    //! +=
    template <typename U>
    MCVector2d<T> & operator += (const MCVector2d<U> & r);

    //! -=
    template <typename U>
    MCVector2d<T> & operator -= (const MCVector2d<U> & r);

    //! Get length
    inline T length() const;

    //! Get length using a fast approximation.
    inline T lengthFast() const;

    //! Get length squared.
    inline T lengthSquared() const;

    //! Normalize
    void normalize();

    //! Normalize, inaccurate
    void normalizeFast();

    //! Return a normalized vector
    inline MCVector2d<T> normalized() const;

    //! Return a normalized vector. Inaccurate.
    inline MCVector2d<T> normalizedFast() const;

    //! Invert
    void invert();

    //! Return an inverted vector
    inline MCVector2d<T> inverted() const;

    //! Set the vector to zero vector.
    void setZero();

    //! Return true if a zero vector.
    bool isZero() const;

    //! Clamp to given length.
    void clamp(T maxLength);

    //! Clamp to given length. Inaccurate.
    void clampFast(T maxLength);

    //! \returns vector a projected on vector b.
    static MCVector2d<T> projection(const MCVector2d<T> & a, const MCVector2d<T> & b);

    //! Set components
    void set(T newI, T newJ = 0);

    //! Set i-component
    void setI(T newI);

    //! Set j-component
    void setJ(T newJ);

    //! Get i-component
    inline T i() const;

    //! Get j-component
    inline T j() const;

    //! Write to stream
    friend std::ostream & operator << (std::ostream & out, const MCVector2d<T> & v)
    {
        out << "[ " << v.m_i << ", " << v.m_j << " ]";
        return out;
    }

private:

    //! Components
    T m_i, m_j;
};

typedef MCVector2d<MCFloat> MCVector2dF;
typedef const MCVector2dF & MCVector2dFR;

template <typename T>
MCVector2d<T>::MCVector2d() :
    m_i(0),
    m_j(0)
{}

template <typename T>
MCVector2d<T>::MCVector2d(T newI, T newJ) :
    m_i(newI),
    m_j(newJ)
{}

template <typename T>
template <typename U>
MCVector2d<T>::MCVector2d(const MCVector2d<U> & r) :
    m_i(r.i()),
    m_j(r.j())
{}

template <typename T>
template <typename U>
MCVector2d<T>::MCVector2d(const MCVector2d<U> && r) :
    m_i(r.i()),
    m_j(r.j())
{}

template <typename T>
template <typename U>
MCVector2d<T> & MCVector2d<T>::operator = (const MCVector2d<U> & r)
{
    if (reinterpret_cast<const void *>(&r) != reinterpret_cast<void *>(this))
    {
        m_i = r.i();
        m_j = r.j();
    }

    return *this;
}

template <typename T>
template <typename U>
MCVector2d<T> & MCVector2d<T>::operator = (const MCVector2d<U> && r)
{
    m_i = r.i();
    m_j = r.j();

    return *this;
}

template <typename T>
template <typename U>
T MCVector2d<T>::operator % (const MCVector2d<U> & r) const
{
    return m_i * r.j() - m_j * r.i();
}

template <typename T>
template <typename U>
T MCVector2d<T>::dot(const MCVector2d<U> & r) const
{
    return m_i * r.i() + m_j * r.j();
}

template <typename T>
template <typename U>
MCVector2d<T> MCVector2d<T>::comp(const MCVector2d<U> & r) const
{
    return MCVector2d<T>(m_i * r.i(), m_j * r.j());
}

template <typename T>
template <typename U>
MCVector2d<T> & MCVector2d<T>::compStore(const MCVector2d<U> & r)
{
    m_i = m_i * r.i();
    m_j = m_j * r.j();

    return *this;
}

template <typename T>
template <typename S>
MCVector2d<T> MCVector2d<T>::operator * (S n) const
{
    return MCVector2d<T>(m_i * n, m_j * n);
}

template <typename T>
template <typename U>
MCVector2d<T> MCVector2d<T>::operator * (const MCVector2d<U> & r) const
{
    return MCVector2d<T>(m_i * r.i(), m_j * r.j());
}

template <typename T>
template <typename S>
MCVector2d<T> & MCVector2d<T>::operator *= (S n)
{
    m_i *= n;
    m_j *= n;
    return *this;
}

template <typename T>
template <typename U>
MCVector2d<T> & MCVector2d<T>::operator *= (const MCVector2d<U> & r)
{
    m_i *= r.i();
    m_j *= r.j();
    return *this;
}

template <typename T>
template <typename S>
MCVector2d<T> MCVector2d<T>::operator / (S n) const
{
    return MCVector2d<T>(m_i / n, m_j / n);
}

template <typename T>
template <typename U>
MCVector2d<T> MCVector2d<T>::operator / (const MCVector2d<U> & r) const
{
    return MCVector2d<T>(m_i / r.i(), m_j / r.j());
}

template <typename T>
template <typename S>
MCVector2d<T> & MCVector2d<T>::operator /= (S n)
{
    m_i /= n;
    m_j /= n;
    return *this;
}

template <typename T>
template <typename U>
MCVector2d<T> & MCVector2d<T>::operator /= (const MCVector2d<U> & r)
{
    m_i /= r.i();
    m_j /= r.j();
    return *this;
}

template <typename T>
template <typename U>
MCVector2d<T> MCVector2d<T>::operator + (const MCVector2d<U> & r) const
{
    return MCVector2d<T>(m_i + r.i(), m_j + r.j());
}

template <typename T>
template <typename U>
MCVector2d<T> & MCVector2d<T>::operator += (const MCVector2d<U> & r)
{
    m_i += r.i();
    m_j += r.j();
    return *this;
}

template <typename T>
template <typename U>
MCVector2d<T> MCVector2d<T>::operator - (const MCVector2d<U> & r) const
{
    return MCVector2d<T>(m_i - r.i(), m_j - r.j());
}

template <typename T>
template <typename U>
MCVector2d<T> & MCVector2d<T>::operator -= (const MCVector2d<U> & r)
{
    m_i -= r.i();
    m_j -= r.j();
    return *this;
}

template <typename T>
T MCVector2d<T>::length() const
{
    return std::sqrt(m_i * m_i + m_j * m_j);
}

template <typename T>
T MCVector2d<T>::lengthFast() const
{
    const T a = std::abs(m_i);
    const T b = std::abs(m_j);

    if (a > b)
    {
        return a + b / 2;
    }
    else
    {
        return b + a / 2;
    }
}

template <typename T>
T MCVector2d<T>::lengthSquared() const
{
    return m_i * m_i + m_j * m_j;
}

template <typename T>
void MCVector2d<T>::normalize()
{
    if (!isZero())
    {
        const T l(length());
        m_i /= l;
        m_j /= l;
    }
}

template <typename T>
void MCVector2d<T>::normalizeFast()
{
    if (!isZero())
    {
        const T l(lengthFast());
        m_i /= l;
        m_j /= l;
    }
}

template <typename T>
MCVector2d<T> MCVector2d<T>::normalized() const
{
    if (!isZero())
    {
        const T l(length());
        return MCVector2d<T>(m_i / l, m_j / l);
    }

    return MCVector2d<T>();
}

template <typename T>
MCVector2d<T> MCVector2d<T>::normalizedFast() const
{
    if (!isZero())
    {
        const T l(lengthFast());
        return MCVector2d<T>(m_i / l, m_j / l);
    }

    return MCVector2d<T>();
}

template <typename T>
void MCVector2d<T>::invert()
{
    m_i = -m_i;
    m_j = -m_j;
}

template <typename T>
MCVector2d<T> MCVector2d<T>::inverted() const
{
    return MCVector2d<T>(-m_i, -m_j);
}

template <typename T>
MCVector2d<T> MCVector2d<T>::projection(const MCVector2d<T> & a, const MCVector2d<T> & b)
{
    return b * a.dot(b) / b.lengthSquared();
}

template <typename T>
void MCVector2d<T>::set(T newI, T newJ)
{
    m_i = newI;
    m_j = newJ;
}

template <typename T>
void MCVector2d<T>::setI(T newI)
{
    m_i = newI;
}

template <typename T>
void MCVector2d<T>::setJ(T newJ)
{
    m_j = newJ;
}

template <typename T>
T MCVector2d<T>::i() const
{
    return m_i;
}

template <typename T>
T MCVector2d<T>::j() const
{
    return m_j;
}

template <typename T>
void MCVector2d<T>::setZero()
{
    m_i = 0;
    m_j = 0;
}

template <typename T>
bool MCVector2d<T>::isZero() const
{
    if (std::numeric_limits<T>::is_exact)
    {
        return m_i == 0 && m_j == 0;
    }
    else
    {
        return
            std::abs(m_i) <= std::numeric_limits<T>::epsilon() &&
            std::abs(m_j) <= std::numeric_limits<T>::epsilon();
    }
}

template <typename T>
void MCVector2d<T>::clamp(T maxLength)
{
    const T l = length();
    if (l > maxLength)
    {
        m_i *= maxLength;
        m_i /= l;
        m_j *= maxLength;
        m_j /= l;
    }
}

template <typename T>
void MCVector2d<T>::clampFast(T maxLength)
{
    const T l = lengthFast();
    if (l > maxLength)
    {
        m_i *= maxLength;
        m_i /= l;
        m_j *= maxLength;
        m_j /= l;
    }
}

// Unary negation
template <typename T>
MCVector2d<T> operator - (const MCVector2d<T> & r)
{
    return MCVector2d<T>(-r.i(), -r.j());
}

#endif // MCVECTOR2D_HH

