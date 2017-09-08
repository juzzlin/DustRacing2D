// MCVector3d -- A generalized 3d-vector class
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

#ifndef MCVECTOR3D_HH
#define MCVECTOR3D_HH

#include "mcvector2d.hh"

#include <cmath>
#include <iostream>
#include <limits>

/*! 3-dimensional vector template. Template parameter represents
 *  the data type of the components. 
 *
 *  Examples: 
 *
 *  MCVector3d<int> a(1, 1, 1);
 *  MCVector3d<int> b(-1, 2, 0);
 *  MCVector3d<int> z = a + b;
 *
 *  // Cross product
 *  cout << a % b << endl;
 *
 *  // Dot product
 *  cout << a.dot(b) << endl;
 *
 */ 

template <typename T>
class MCVector3d
{
public:

    //! Constructor
    MCVector3d();

    //! Constructor
    explicit MCVector3d(T i, T j = 0, T k = 0);

    //! Constructor
    template <typename U>
    MCVector3d(const MCVector2d<U> & r, U k = 0);

    //! Copy constructor
    template <typename U>
    MCVector3d(const MCVector3d<U> & r);

    //! Move constructor
    template <typename U>
    MCVector3d(const MCVector3d<U> && r);

    //! Destructor
    inline ~MCVector3d() {}

    //! Type conversion to MCVector2d
    template <typename U>
    operator MCVector2d<U>() const;

    //! Assignment
    template <typename U>
    MCVector3d<T> & operator = (const MCVector3d<U> & r);

    //! Move assignment
    template <typename U>
    MCVector3d<T> & operator = (const MCVector3d<U> && r);

    //! Cross product. Returns the resulting vector.
    template <typename U>
    MCVector3d<T> operator % (const MCVector3d<U> & r) const;

    //! Dot product
    template <typename U>
    T dot(const MCVector3d<U> & r) const;

    //! Component product
    template <typename U>
    MCVector3d<T> comp(const MCVector3d<U> & r) const;

    //! Component product + store
    template <typename U>
    MCVector3d<T> & compStore(const MCVector3d<U> & r);
    
    //! Multiplication
    template <typename S>
    MCVector3d<T> operator * (S n) const;
    MCVector3d<T> operator * (const MCVector3d<T> & n) const;

    //! Division
    template <typename S>
    MCVector3d<T> operator / (S n) const;
    MCVector3d<T> operator / (const MCVector3d<T> & n) const;

    //! Vector sum
    template <typename U>
    MCVector3d<T> operator + (const MCVector3d<U> & r) const;

    //! Vector subtraction
    template <typename U>
    MCVector3d<T> operator - (const MCVector3d<U> & r) const;

    //! *=
    template <typename S>
    MCVector3d<T> & operator *= (S n);
    MCVector3d<T> & operator *= (const MCVector3d<T> & n);

    //! /=
    template <typename S>
    MCVector3d<T> & operator /= (S n);
    MCVector3d<T> & operator /= (const MCVector3d<T> & n);

    //! +=
    template <typename U>
    MCVector3d<T> & operator += (const MCVector3d<U> & r);

    //! -=
    template <typename U>
    MCVector3d<T> & operator -= (const MCVector3d<U> & r);

    //! Get length.
    inline T length() const;

    //! Get length using a fast approximation.
    inline T lengthFast() const;

    //! Get length squared.
    inline T lengthSquared() const;

    //! Normalize
    void normalize();

    //! Return normalized vector
    inline MCVector3d<T> normalized() const;

    //! Normalize
    void normalizeFast();

    //! Return normalized vector
    inline MCVector3d<T> normalizedFast() const;

    //! Invert vector
    void invert();

    //! Return inverted vector
    inline MCVector3d<T> inverted() const;

    //! Set the vector to zero vector
    void setZero();

    //! Return true if a zero vector
    bool isZero() const;

    //! Clamp to given length.
    void clamp(T maxLength);

    //! Clamp to given length. Inaccurate.
    void clampFast(T maxLength);

    //! \returns vector a projected on vector b.
    static MCVector3d<T> projection(const MCVector3d<T> & a, const MCVector3d<T> & b);

    //! Set components
    void set(T i, T j = 0, T k = 0);

    //! Set i-component
    void setI(T newI);

    //! Set j-component
    void setJ(T newJ);

    //! Set k-component
    void setK(T newK);

    //! Get i-component
    inline T i() const;

    //! Get j-component
    inline T j() const;

    //! Get k-component
    inline T k() const;

    //! Write to stream
    friend std::ostream & operator << (std::ostream & out, const MCVector3d<T> & v)
    {
        out << "[ " << v.i() << ", " << v.j() << ", " << v.k() << " ]";
        return out;
    }

private:

    //! Components
    T m_i, m_j, m_k, padding;
};

typedef MCVector3d<MCFloat> MCVector3dF;
typedef const MCVector3dF & MCVector3dFR;

template <typename T>
MCVector3d<T>::MCVector3d() :
    m_i(0),
    m_j(0),
    m_k(0)
{}

template <typename T>
MCVector3d<T>::MCVector3d(T newI, T newJ, T newK) :
    m_i(newI),
    m_j(newJ),
    m_k(newK)
{}

template <typename T>
template <typename U>
MCVector3d<T>::MCVector3d(const MCVector3d<U> & r) :
    m_i(r.i()),
    m_j(r.j()),
    m_k(r.k())
{}

template <typename T>
template <typename U>
MCVector3d<T>::MCVector3d(const MCVector3d<U> && r) :
    m_i(r.i()),
    m_j(r.j()),
    m_k(r.k())
{}

template <typename T>
template <typename U>
MCVector3d<T>::MCVector3d(const MCVector2d<U> & r, U k) :
    m_i(r.i()),
    m_j(r.j()),
    m_k(k)
{}

template <typename T>
template <typename U>
MCVector3d<T>::operator MCVector2d<U>() const
{
    return MCVector2d<T>(m_i, m_j);
}

template <typename T>
template <typename U>
MCVector3d<T> & MCVector3d<T>::operator = (const MCVector3d<U> & r)
{
    if (reinterpret_cast<const void *>(&r) != reinterpret_cast<void *>(this))
    {
        m_i = r.i();
        m_j = r.j();
        m_k = r.k();
    }

    return *this;
}

template <typename T>
template <typename U>
MCVector3d<T> & MCVector3d<T>::operator = (const MCVector3d<U> && r)
{
    m_i = r.i();
    m_j = r.j();
    m_k = r.k();

    return *this;
}

template <typename T>
template <typename U>
MCVector3d<T> MCVector3d<T>::operator % (const MCVector3d<U> & r) const
{
    T _i = m_j * r.k() - r.j() * m_k;
    T _j = m_i * r.k() - r.i() * m_k;
    T _k = m_i * r.j() - r.i() * m_j;

    return MCVector3d<T>(_i, -_j, _k);
}

template <typename T>
template <typename U>
T MCVector3d<T>::dot(const MCVector3d<U> & r) const
{
    return m_i * r.i() + m_j * r.j() + m_k * r.k();
}

template <typename T>
template <typename U>
MCVector3d<T> MCVector3d<T>::comp(const MCVector3d<U> & r) const
{
    return MCVector3d<T>(m_i * r.i(), m_j * r.j(), m_k * r.k());
}

template <typename T>
template <typename U>
MCVector3d<T> & MCVector3d<T>::compStore(const MCVector3d<U> & r)
{
    m_i = m_i * r.i();
    m_j = m_j * r.j();
    m_k = m_k * r.k();
    
    return *this;
}

template <typename T>
template <typename S>
MCVector3d<T> MCVector3d<T>::operator * (S n) const
{
    return MCVector3d<T>(m_i * n, m_j * n, m_k * n);
}

template <typename T>
MCVector3d<T> MCVector3d<T>::operator * (const MCVector3d<T> & n) const
{
    return MCVector3d<T>(m_i * n.m_i, m_j * n.m_j, m_k * n.m_k);
}

template <typename T>
template <typename S>
MCVector3d<T> & MCVector3d<T>::operator *= (S n)
{
    m_i *= n;
    m_j *= n;
    m_k *= n;
    return *this;
}

template <typename T>
MCVector3d<T> & MCVector3d<T>::operator *= (const MCVector3d<T> & n)
{
    m_i *= n.m_i;
    m_j *= n.m_j;
    m_k *= n.m_k;
    return *this;
}

template <typename T>
template <typename S>
MCVector3d<T> MCVector3d<T>::operator / (S n) const
{
    return MCVector3d<T>(m_i / n, m_j / n, m_k / n);
}

template <typename T>
MCVector3d<T> MCVector3d<T>::operator / (const MCVector3d<T> & n) const
{
    return MCVector3d<T>(m_i / n.m_i, m_j / n.m_j, m_k / n.m_k);
}

template <typename T>
template <typename S>
MCVector3d<T> & MCVector3d<T>::operator /= (S n)
{
    m_i /= n;
    m_j /= n;
    m_k /= n;
    return *this;
}

template <typename T>
MCVector3d<T> & MCVector3d<T>::operator /= (const MCVector3d<T> & n)
{
    m_i /= n.m_i;
    m_j /= n.m_j;
    m_k /= n.m_k;
    return *this;
}

template <typename T>
template <typename U>
MCVector3d<T> MCVector3d<T>::operator + (const MCVector3d<U> & r) const
{
    return MCVector3d<T>(m_i + r.i(), m_j + r.j(), m_k + r.k());
}

template <typename T>
template <typename U>
MCVector3d<T> & MCVector3d<T>::operator += (const MCVector3d<U> & r)
{
    m_i += r.i();
    m_j += r.j();
    m_k += r.k();
    return *this;
}

template <typename T>
template <typename U>
MCVector3d<T> MCVector3d<T>::operator - (const MCVector3d<U> & r) const
{
    return MCVector3d<T>(m_i - r.i(), m_j - r.j(), m_k - r.k());
}

template <typename T>
template <typename U>
MCVector3d<T> & MCVector3d<T>::operator -= (const MCVector3d<U> & r)
{
    m_i -= r.i();
    m_j -= r.j();
    m_k -= r.k();
    return *this;
}

template <typename T>
T MCVector3d<T>::length() const
{
    return std::sqrt(m_i * m_i + m_j * m_j + m_k * m_k);
}

template <typename T>
T MCVector3d<T>::lengthFast() const
{
    const MCVector2d<T> ij(m_i, m_j);
    const T l = ij.lengthFast();
    const MCVector2d<T> lk(l, m_k);
    return lk.lengthFast();
}

template <typename T>
T MCVector3d<T>::lengthSquared() const
{
    return m_i * m_i + m_j * m_j + m_k * m_k;
}

template <typename T>
void MCVector3d<T>::normalize()
{
    if (!isZero())
    {
        const T l(length());
        m_i /= l;
        m_j /= l;
        m_k /= l;
    }
}

template <typename T>
void MCVector3d<T>::normalizeFast()
{
    if (!isZero())
    {
        const T l(lengthFast());
        m_i /= l;
        m_j /= l;
        m_k /= l;
    }
}

template <typename T>
MCVector3d<T> MCVector3d<T>::normalized() const
{
    if (!isZero())
    {
        const T l(length());
        return MCVector3d<T>(m_i / l, m_j / l, m_k / l);
    }

    return MCVector3d<T>();
}

template <typename T>
MCVector3d<T> MCVector3d<T>::normalizedFast() const
{
    if (!isZero())
    {
        const T l(lengthFast());
        return MCVector3d<T>(m_i / l, m_j / l, m_k / l);
    }

    return MCVector3d<T>();
}

template <typename T>
void MCVector3d<T>::invert()
{
    m_i = -m_i;
    m_j = -m_j;
    m_k = -m_k;
}

template <typename T>
MCVector3d<T> MCVector3d<T>::inverted() const
{
    return MCVector3d<T>(-m_i, -m_j, -m_k);
}

template <typename T>
MCVector3d<T> MCVector3d<T>::projection(const MCVector3d<T> & a, const MCVector3d<T> & b)
{
    return b * a.dot(b) / b.lengthSquared();
}

template <typename T>
void MCVector3d<T>::set(T newI, T newJ, T newK)
{
    m_i = newI;
    m_j = newJ;
    m_k = newK;
}

template <typename T>
void MCVector3d<T>::setI(T newI)
{
    m_i = newI;
}

template <typename T>
void MCVector3d<T>::setJ(T newJ)
{
    m_j = newJ;
}

template <typename T>
void MCVector3d<T>::setK(T newK)
{
    m_k = newK;
}

template <typename T>
T MCVector3d<T>::i() const
{
    return m_i;
}

template <typename T>
T MCVector3d<T>::j() const
{
    return m_j;
}

template <typename T>
T MCVector3d<T>::k() const
{
    return m_k;
}

template <typename T>
void MCVector3d<T>::setZero()
{
    m_i = 0;
    m_j = 0;
    m_k = 0;
}

template <typename T>
bool MCVector3d<T>::isZero() const
{
    if (std::numeric_limits<T>::is_exact)
    {
        return m_i == 0 && m_j == 0 && m_k == 0;
    }
    else
    {
        return
            std::abs(m_i) <= std::numeric_limits<T>::epsilon() &&
            std::abs(m_j) <= std::numeric_limits<T>::epsilon() &&
            std::abs(m_k) <= std::numeric_limits<T>::epsilon();
    }
}

template <typename T>
void MCVector3d<T>::clamp(T maxLength)
{
    const T l = length();
    if (l > maxLength)
    {
        m_i *= maxLength;
        m_i /= l;
        m_j *= maxLength;
        m_j /= l;
        m_k *= maxLength;
        m_k /= l;
    }
}

template <typename T>
void MCVector3d<T>::clampFast(T maxLength)
{
    const T l = lengthFast();
    if (l > maxLength)
    {
        m_i *= maxLength;
        m_i /= l;
        m_j *= maxLength;
        m_j /= l;
        m_k *= maxLength;
        m_k /= l;
    }
}

// Unary negation
template <typename T>
MCVector3d<T> operator - (const MCVector3d<T> & r)
{
    return MCVector3d<T>(-r.i(), -r.j(), -r.k());
}

#endif // MCVECTOR3D_HH
