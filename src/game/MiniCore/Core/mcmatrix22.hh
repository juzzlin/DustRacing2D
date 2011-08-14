// MCMatrix22 -- A generalized 2x2-matrix class
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

#ifndef MCMATRIX22_HH
#define MCMATRIX22_HH

#include <iostream>

using std::ostream;
using std::endl;

#include "mcvector2d.hh"

template <typename T>
class MCMatrix22
{
public:

  //! Consturctor
  inline MCMatrix22();

  /*! Consturctor
   *
   * [[_11, _12]
   *  [_21, _22]]
   */
  inline MCMatrix22(T _11, T _12, T _21, T _22);

  //! Copy constructor
  template <typename U>
  inline MCMatrix22(const MCMatrix22<U> & r);

  //! Destructor
  inline ~MCMatrix22() {}

  //! Assignment operator
  template <typename U>
  inline MCMatrix22<T> & operator = (const MCMatrix22<U> & r);

  //! Multiplication with a scalar
  template <typename U>
  inline MCMatrix22<T> operator * (U x) const;

  //! Division with a scalar
  template <typename U>
  inline MCMatrix22<T> operator / (U x) const;

  //! Addition with a scalar
  template <typename U>
  inline MCMatrix22<T> operator + (U x) const;

  //! Entrywise addition with another MCMatrix22
  template <typename U>
  inline MCMatrix22<T> operator + (const MCMatrix22<U> & r) const;

  //! Subtraction with a scalar
  template <typename U>
  inline MCMatrix22<T> operator - (U x) const;

  //! Entrywise subtraction with another MCMatrix22
  template <typename U>
  inline MCMatrix22<T> operator - (const MCMatrix22<U> & r) const;

  //! Matrix multiplication with a mcvector2d
  template <typename U>
  inline MCVector2d<T> operator * (const MCVector2d<U> & r) const;

  //! Matrix multiplication with another MCMatrix22
  template <typename U>
  inline MCMatrix22<T> operator * (const MCMatrix22<U> & r) const;

  //! Multiplication-assignment with a scalar
  template <typename U>
  inline MCMatrix22<T> & operator *= (U x);

  //! Division-assignment with a scalar
  template <typename U>
  inline MCMatrix22<T> & operator /= (U x);

  //! Addition-assignment with a scalar
  template <typename U>
  inline MCMatrix22<T> & operator += (U x);

  //! Entrywise addition-assignment with another MCMatrix22
  template <typename U>
  inline MCMatrix22<T> & operator += (const MCMatrix22<U> & r);

  //! Subtraction-assignment with a scalar
  template <typename U>
  inline MCMatrix22<T> & operator -= (U x);

  //! Entrywise subtraction-assignment with another MCMatrix22
  template <typename U>
  inline MCMatrix22<T> & operator -= (const MCMatrix22<U> & r);

  //! Matrix multiplication-assignment with another MCMatrix22
  template <typename U>
  inline MCMatrix22<T> & operator *= (const MCMatrix22<U> & r);

  //! Determinant
  T det() const;

  //! Set the matrix as an identity matrix
  void setIdentity();

  //! Return transposed matrix
  MCMatrix22<T> transpose() const;

  //! Set transposed matrix
  MCMatrix22<T> & setTranspose();

  //! Return adjugate matrix
  MCMatrix22<T> adjugate() const;

  //! Set adjugate matrix
  MCMatrix22<T> & setAdjugate();

  //! Write to stream
  friend ostream& operator<<(ostream & out, const MCMatrix22<T> & r) 
  {
    out << "[[\t" << r.m_11 << "\t" << r.m_12 << "\t] " << endl;
    out << " [\t" << r.m_21 << "\t" << r.m_22 << "\t]]" << endl;
    return out;
  }

  //! Values
  T m_11, m_12, m_21, m_22;
};

template <typename T>
MCMatrix22<T>::MCMatrix22() :
  m_11(0),
  m_12(0),
  m_21(0),
  m_22(0)
{}

template <typename T>
MCMatrix22<T>::MCMatrix22(T _11, T _12, T _21, T _22) :
  m_11(_11),
  m_12(_12),
  m_21(_21),
  m_22(_22)
{}

template <typename T>
template <typename U>
MCMatrix22<T>::MCMatrix22(const MCMatrix22<U> & r) :
  m_11(r.m_11),
  m_12(r.m_12),
  m_21(r.m_21),
  m_22(r.m_22)
{}

template <typename T>
template <typename U>
MCMatrix22<T> & MCMatrix22<T>::operator = (const MCMatrix22<U> & r)
{
  if (static_cast<const void *>(&r) != static_cast<void *>(this))
  {
    m_11 = r.m_11;
    m_12 = r.m_12;
    m_21 = r.m_21;
    m_22 = r.m_22;
  }
  return *this;
}

template <typename T>
template <typename U>
MCMatrix22<T> MCMatrix22<T>::operator * (U x) const
{
  return MCMatrix22<T>(m_11 * x, m_12 * x, m_21 * x, m_22 * x);
}

template <typename T>
template <typename U>
MCMatrix22<T> MCMatrix22<T>::operator / (U x) const
{
  return MCMatrix22<T>(m_11 / x, m_12 / x, m_21 / x, m_22 / x);
}

template <typename T>
template <typename U>
MCMatrix22<T> MCMatrix22<T>::operator + (U x) const
{
  return MCMatrix22<T>(m_11 + x, m_12 + x, m_21 + x, m_22 + x);
}

template <typename T>
template <typename U>
MCMatrix22<T> MCMatrix22<T>::operator + (const MCMatrix22<U> & r) const
{
  return MCMatrix22<T>(m_11 + r.m_11, m_12 + r.m_12, m_21 + r.m_21, m_22 + r.m_22);
}

template <typename T>
template <typename U>
MCMatrix22<T> MCMatrix22<T>::operator - (U x) const
{
  return MCMatrix22<T>(m_11 - x, m_12 - x, m_21 - x, m_22 - x);
}

template <typename T>
template <typename U>
MCMatrix22<T> MCMatrix22<T>::operator - (const MCMatrix22<U> & r) const
{
  return MCMatrix22<T>(m_11 - r.m_11, m_12 - r.m_12, m_21 - r.m_21, m_22 - r.m_22);
}

template <typename T>
template <typename U>
MCVector2d<T> MCMatrix22<T>::operator * (const MCVector2d<U> & r) const
{
  return MCVector2d<T>(m_11 * r.m_i + m_12 * r.m_j,
                     m_21 * r.m_i + m_22 * r.m_j);
}

template <typename T>
template <typename U>
MCMatrix22<T> MCMatrix22<T>::operator * (const MCMatrix22<U> & r) const
{
  return MCMatrix22<T>(m_11 * r.m_11 + m_12 * r.m_21,
                     m_11 * r.m_12 + m_12 * r.m_22,
                     m_21 * r.m_11 + m_22 * r.m_21,
                     m_21 * r.m_12 + m_22 * r.m_22);
}

template <typename T>
template <typename U>
MCMatrix22<T> & MCMatrix22<T>::operator *= (U x)
{
  m_11 *= x;
  m_12 *= x;
  m_21 *= x;
  m_22 *= x;

  return *this;
}

template <typename T>
template <typename U>
MCMatrix22<T> & MCMatrix22<T>::operator /= (U x)
{
  m_11 /= x;
  m_12 /= x;
  m_21 /= x;
  m_22 /= x;

  return *this;
}

template <typename T>
template <typename U>
MCMatrix22<T> & MCMatrix22<T>::operator += (U x)
{
  m_11 += x;
  m_12 += x;
  m_21 += x;
  m_22 += x;

  return *this;
}

template <typename T>
template <typename U>
MCMatrix22<T> & MCMatrix22<T>::operator += (const MCMatrix22<U> & r)
{
  m_11 += r.m_11;
  m_12 += r.m_12;
  m_21 += r.m_21;
  m_22 += r.m_22;

  return *this;
}

template <typename T>
template <typename U>
MCMatrix22<T> & MCMatrix22<T>::operator -= (U x)
{
  m_11 -= x;
  m_12 -= x;
  m_21 -= x;
  m_22 -= x;

  return *this;
}

template <typename T>
template <typename U>
MCMatrix22<T> & MCMatrix22<T>::operator -= (const MCMatrix22<U> & r)
{
  m_11 -= r.m_11;
  m_12 -= r.m_12;
  m_21 -= r.m_21;
  m_22 -= r.m_22;

  return *this;
}

template <typename T>
template <typename U>
MCMatrix22<T> & MCMatrix22<T>::operator *= (const MCMatrix22<U> & r)
{
  m_11 = m_11 * r.m_11 + m_12 * r.m_21;
  m_12 = m_11 * r.m_12 + m_12 * r.m_22;
  m_21 = m_21 * r.m_11 + m_22 * r.m_21;
  m_22 = m_21 * r.m_12 + m_22 * r.m_22;

  return *this;
}

template <typename T>
T MCMatrix22<T>::det() const
{
  return m_11 * m_22 - m_12 * m_21;
}

template <typename T>
void MCMatrix22<T>::setIdentity()
{
  m_11 = 1;
  m_12 = 0;
  m_21 = 0;
  m_22 = 1;
}

template <typename T>
MCMatrix22<T> MCMatrix22<T>::transpose() const
{
  return MCMatrix22<T>(m_11, m_21, m_12, m_22);
}

template <typename T>
MCMatrix22<T> & MCMatrix22<T>::setTranspose()
{
  const T t(m_12); m_12 = m_21; m_21 = t;
  return *this;
}

template <typename T>
MCMatrix22<T> MCMatrix22<T>::adjugate() const
{
  return MCMatrix22<T>(m_22, -m_12, -m_21, m_11);
}

template <typename T>
MCMatrix22<T> & MCMatrix22<T>::setAdjugate()
{
  const T t(m_11); 

  m_11 = m_22; 
  m_22 = t;
  m_12 = -m_12;
  m_21 = -m_21;

  return *this;
}

#endif // MCMATRIX22_HH
