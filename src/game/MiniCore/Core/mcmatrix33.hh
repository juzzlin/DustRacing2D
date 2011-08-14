// MCMatrix33 -- A generalized 3x3-matrix class
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

#ifndef MCMATRIX33_HH
#define MCMATRIX33_HH

#include <iostream>

using std::ostream;
using std::endl;

#include "mcvector3d.hh"

template <typename T>
class MCMatrix33
{
public:

  //! Constructor
  inline MCMatrix33();

  /*! Constructor
   *
   * [[_11, _12, _13]
   *  [_21, _22, _23]
   *  [_31, _32, _33]]
   */
  inline MCMatrix33(T _11, T _12, T _13,
                  T _21, T _22, T _23,
                  T _31, T _32, T _33);

  //! Copy constructor
  template <typename U>
  inline MCMatrix33(const MCMatrix33<U> & r);

  //! Destructor
  inline ~MCMatrix33() {}

  //! Assignment operator
  template <typename U>
  inline MCMatrix33<T> & operator = (const MCMatrix33<U> & r);

  //! Multiplication with a scalar
  template <typename U>
  inline MCMatrix33<T> operator * (U x) const;

  //! Division with a scalar
  template <typename U>
  inline MCMatrix33<T> operator / (U x) const;

  //! Addition with a scalar
  template <typename U>
  inline MCMatrix33<T> operator + (U x) const;

  //! Entrywise addition with another MCMatrix33
  template <typename U>
  inline MCMatrix33<T> operator + (const MCMatrix33<U> & r) const;

  //! Subtraction with a scalar
  template <typename U>
  inline MCMatrix33<T> operator - (U x) const;

  //! Entrywise subtraction with another MCMatrix33
  template <typename U>
  inline MCMatrix33<T> operator - (const MCMatrix33<U> & r) const;

  //! Matrix multiplication with a MCVector3d
  template <typename U>
  inline MCVector3d<T> operator * (const MCVector3d<U> & r) const;

  //! Matrix multiplication with another MCMatrix33
  template <typename U>
  inline MCMatrix33<T> operator * (const MCMatrix33<U> & r) const;

  //! Multiplication-assignment with a scalar
  template <typename U>
  inline MCMatrix33<T> & operator *= (U x);

  //! Division-assignment with a scalar
  template <typename U>
  inline MCMatrix33<T> & operator /= (U x);

  //! Addition-assignment with a scalar
  template <typename U>
  inline MCMatrix33<T> & operator += (U x);

  //! Entrywise addition-assignment with another MCMatrix33
  template <typename U>
  inline MCMatrix33<T> & operator += (const MCMatrix33<U> & r);

  //! Subtraction-assignment with a scalar
  template <typename U>
  inline MCMatrix33<T> & operator -= (U x);

  //! Entrywise subtraction-assignment with another MCMatrix33
  template <typename U>
  inline MCMatrix33<T> & operator -= (const MCMatrix33<U> & r);

  //! Matrix multiplication-assignment with another MCMatrix33
  template <typename U>
  inline MCMatrix33<T> & operator *= (const MCMatrix33<U> & r);

  //! Determinant
  T det() const;

  //! Set the matrix as an identity matrix
  void setIdentity();

  //! Return transposed matrix
  MCMatrix33<T> transpose() const;

  //! Set transposed matrix
  MCMatrix33<T> & setTranspose();

  //! Return adjugate matrix
  MCMatrix33<T> adjugate() const;

  //! Set adjugate matrix
  MCMatrix33<T> & setAdjugate();

  //! Write to stream
  friend ostream& operator<<(ostream & out, const MCMatrix33<T> & r) 
  {
    out << "[[\t" << r.m_11 << "\t" << r.m_12 << "\t" << r.m_13 << "\t] " << endl;
    out << " [\t" << r.m_21 << "\t" << r.m_22 << "\t" << r.m_23 << "\t] " << endl;
    out << " [\t" << r.m_31 << "\t" << r.m_32 << "\t" << r.m_33 << "\t]]" << endl;
    return out;
  }

  //! Values
  T m_11, m_12, m_13;
  T m_21, m_22, m_23;
  T m_31, m_32, m_33;
};

template <typename T>
MCMatrix33<T>::MCMatrix33() :
  m_11(0), m_12(0), m_13(0),
  m_21(0), m_22(0), m_23(0),
  m_31(0), m_32(0), m_33(0)
{}

template <typename T>
MCMatrix33<T>::MCMatrix33(T _11, T _12, T _13, 
                      T _21, T _22, T _23,
                      T _31, T _32, T _33) :

  m_11(_11), m_12(_12), m_13(_13),
  m_21(_21), m_22(_22), m_23(_23),
  m_31(_31), m_32(_32), m_33(_33)
{}

template <typename T>
template <typename U>
MCMatrix33<T>::MCMatrix33(const MCMatrix33<U> & r) :
  m_11(r.m_11), m_12(r.m_12), m_13(r.m_13),
  m_21(r.m_21), m_22(r.m_22), m_23(r.m_23),
  m_31(r.m_31), m_32(r.m_32), m_33(r.m_33)
{}

template <typename T>
template <typename U>
MCMatrix33<T> & MCMatrix33<T>::operator = (const MCMatrix33<U> & r)
{
  if (static_cast<const void *>(&r) != static_cast<void *>(this))
  {
    m_11 = r.m_11;
    m_12 = r.m_12;
    m_13 = r.m_13;
    m_21 = r.m_21;
    m_22 = r.m_22;
    m_23 = r.m_23;
    m_31 = r.m_31;
    m_32 = r.m_32;
    m_33 = r.m_33;
  }

  return *this;
}

template <typename T>
template <typename U>
MCMatrix33<T> MCMatrix33<T>::operator * (U x) const
{
  return MCMatrix33<T>(m_11 * x, m_12 * x, m_13 * x, 
                     m_21 * x, m_22 * x, m_23 * x,
                     m_31 * x, m_32 * x, m_33 * x);
}

template <typename T>
template <typename U>
MCMatrix33<T> MCMatrix33<T>::operator / (U x) const
{
  return MCMatrix33<T>(m_11 / x, m_12 / x, m_13 / x, 
                     m_21 / x, m_22 / x, m_23 / x,
                     m_31 / x, m_32 / x, m_33 / x);
}

template <typename T>
template <typename U>
MCMatrix33<T> MCMatrix33<T>::operator + (U x) const
{
  return MCMatrix33<T>(m_11 + x, m_12 + x, m_13 + x, 
                     m_21 + x, m_22 + x, m_23 + x,
                     m_31 + x, m_32 + x, m_33 + x);
}

template <typename T>
template <typename U>
MCMatrix33<T> MCMatrix33<T>::operator + (const MCMatrix33<U> & r) const
{
  return MCMatrix33<T>(m_11 + r.m_11, m_12 + r.m_12, m_13 + r.m_13,
                     m_21 + r.m_21, m_22 + r.m_22, m_23 + r.m_23,
                     m_31 + r.m_31, m_32 + r.m_32, m_33 + r.m_33);
}

template <typename T>
template <typename U>
MCMatrix33<T> MCMatrix33<T>::operator - (U x) const
{
  return MCMatrix33<T>(m_11 - x, m_12 - x, m_13 - x, 
                     m_21 - x, m_22 - x, m_23 - x,
                     m_31 - x, m_32 - x, m_33 - x);
}

template <typename T>
template <typename U>
MCMatrix33<T> MCMatrix33<T>::operator - (const MCMatrix33<U> & r) const
{
  return MCMatrix33<T>(m_11 - r.m_11, m_12 - r.m_12, m_13 - r.m_13,
                     m_21 - r.m_21, m_22 - r.m_22, m_23 - r.m_23,
                     m_31 - r.m_31, m_32 - r.m_32, m_33 - r.m_33);
}

template <typename T>
template <typename U>
MCVector3d<T> MCMatrix33<T>::operator * (const MCVector3d<U> & r) const
{
  return MCVector3d<T>(m_11 * r.m_i + m_12 * r.m_j + m_13 * r.m_k,
                     m_21 * r.m_i + m_22 * r.m_j + m_23 * r.m_k,
                     m_31 * r.m_i + m_32 * r.m_j + m_33 * r.m_k);
}

template <typename T>
template <typename U>
MCMatrix33<T> MCMatrix33<T>::operator * (const MCMatrix33<U> & r) const
{
  return MCMatrix33<T>(m_11 * r.m_11 + m_12 * r.m_21 + m_13 * r.m_31,
                     m_11 * r.m_12 + m_12 * r.m_22 + m_13 * r.m_32,
                     m_11 * r.m_13 + m_12 * r.m_23 + m_13 * r.m_33,
                     m_21 * r.m_11 + m_22 * r.m_21 + m_23 * r.m_31,
                     m_21 * r.m_12 + m_22 * r.m_22 + m_23 * r.m_32,
                     m_21 * r.m_13 + m_22 * r.m_23 + m_23 * r.m_33,
                     m_31 * r.m_11 + m_32 * r.m_21 + m_33 * r.m_31,
                     m_31 * r.m_12 + m_32 * r.m_22 + m_33 * r.m_32,
                     m_31 * r.m_13 + m_32 * r.m_23 + m_33 * r.m_33);
}

template <typename T>
template <typename U>
MCMatrix33<T> & MCMatrix33<T>::operator *= (U x)
{
  m_11 *= x;
  m_12 *= x;
  m_13 *= x;
  m_21 *= x;
  m_22 *= x;
  m_23 *= x;
  m_31 *= x;
  m_32 *= x;
  m_33 *= x;

  return *this;
}

template <typename T>
template <typename U>
MCMatrix33<T> & MCMatrix33<T>::operator /= (U x)
{
  m_11 /= x;
  m_12 /= x;
  m_13 /= x;
  m_21 /= x;
  m_22 /= x;
  m_23 /= x;
  m_31 /= x;
  m_32 /= x;
  m_33 /= x;

  return *this;
}

template <typename T>
template <typename U>
MCMatrix33<T> & MCMatrix33<T>::operator += (U x)
{
  m_11 += x;
  m_12 += x;
  m_13 += x;
  m_21 += x;
  m_22 += x;
  m_23 += x;
  m_31 += x;
  m_32 += x;
  m_33 += x;

  return *this;
}

template <typename T>
template <typename U>
MCMatrix33<T> & MCMatrix33<T>::operator += (const MCMatrix33<U> & r)
{
  m_11 += r.m_11;
  m_12 += r.m_12;
  m_13 += r.m_13;
  m_21 += r.m_21;
  m_22 += r.m_22;
  m_23 += r.m_23;
  m_31 += r.m_31;
  m_32 += r.m_32;
  m_33 += r.m_33;

  return *this;
}

template <typename T>
template <typename U>
MCMatrix33<T> & MCMatrix33<T>::operator -= (U x)
{
  m_11 -= x;
  m_12 -= x;
  m_13 -= x;
  m_21 -= x;
  m_22 -= x;
  m_23 -= x;
  m_31 -= x;
  m_32 -= x;
  m_33 -= x;

  return *this;
}

template <typename T>
template <typename U>
MCMatrix33<T> & MCMatrix33<T>::operator -= (const MCMatrix33<U> & r)
{
  m_11 -= r.m_11;
  m_12 -= r.m_12;
  m_13 -= r.m_13;
  m_21 -= r.m_21;
  m_22 -= r.m_22;
  m_23 -= r.m_23;
  m_31 -= r.m_31;
  m_32 -= r.m_32;
  m_33 -= r.m_33;

  return *this;
}

template <typename T>
template <typename U>
MCMatrix33<T> & MCMatrix33<T>::operator *= (const MCMatrix33<U> & r)
{
  m_11 = m_11 * r.m_11 + m_12 * r.m_21 + m_13 * r.m_31;
  m_12 = m_11 * r.m_12 + m_12 * r.m_22 + m_13 * r.m_32;
  m_13 = m_11 * r.m_13 + m_12 * r.m_23 + m_13 * r.m_33;
  m_21 = m_21 * r.m_11 + m_22 * r.m_21 + m_23 * r.m_31;
  m_22 = m_21 * r.m_12 + m_22 * r.m_22 + m_23 * r.m_32;
  m_23 = m_21 * r.m_13 + m_22 * r.m_23 + m_23 * r.m_33;
  m_31 = m_31 * r.m_11 + m_32 * r.m_21 + m_33 * r.m_31;
  m_32 = m_31 * r.m_12 + m_32 * r.m_22 + m_33 * r.m_32;
  m_33 = m_31 * r.m_13 + m_32 * r.m_23 + m_33 * r.m_33;

  return *this;
}

template <typename T>
T MCMatrix33<T>::det() const
{
  return m_11 * (m_22 * m_33 - m_23 * m_32) +
      m_12 * (m_23 * m_31 - m_21 * m_33) +
      m_13 * (m_21 * m_32 - m_22 * m_31);
}

template <typename T>
void MCMatrix33<T>::setIdentity()
{
  m_11 = 1;
  m_12 = 0;
  m_13 = 0;
  m_21 = 0;
  m_22 = 1;
  m_23 = 0;
  m_31 = 0;
  m_32 = 0;
  m_33 = 1;
}

template <typename T>
MCMatrix33<T> MCMatrix33<T>::transpose() const
{
  return MCMatrix33<T>(m_11, m_21, m_31,
                     m_12, m_22, m_32,
                     m_13, m_23, m_33);
}

template <typename T>
MCMatrix33<T> & MCMatrix33<T>::setTranspose()
{
  T
  t = m_12; m_12 = m_21; m_21 = t;
  t = m_13; m_13 = m_31; m_31 = t;
  t = m_23; m_23 = m_32; m_32 = t;

  return *this;
}

template <typename T>
MCMatrix33<T> MCMatrix33<T>::adjugate() const
{
  return MCMatrix33<T>(m_22 * m_33 - m_23 * m_32,
                     m_13 * m_32 - m_12 * m_33,
                     m_12 * m_23 - m_13 * m_22,
                     m_23 * m_31 - m_21 * m_33,
                     m_11 * m_33 - m_13 * m_31,
                     m_13 * m_21 - m_11 * m_23,
                     m_21 * m_32 - m_22 * m_31,
                     m_12 * m_31 - m_11 * m_32,
                     m_11 * m_22 - m_12 * m_21);
}

template <typename T>
MCMatrix33<T> & MCMatrix33<T>::setAdjugate()
{
  const T t_11 = m_22 * m_33 - m_23 * m_32;
  const T t_12 = m_13 * m_32 - m_12 * m_33;
  const T t_13 = m_12 * m_23 - m_13 * m_22;
  const T t_21 = m_23 * m_31 - m_21 * m_33;
  const T t_22 = m_11 * m_33 - m_13 * m_31;
  const T t_23 = m_13 * m_21 - m_11 * m_23;
  const T t_31 = m_21 * m_32 - m_22 * m_31;
  const T t_32 = m_12 * m_31 - m_11 * m_32;
  const T t_33 = m_11 * m_22 - m_12 * m_21;

  m_11 = t_11; m_12 = t_12; m_13 = t_13;
  m_21 = t_21; m_22 = t_22; m_23 = t_23;
  m_31 = t_31; m_32 = t_32; m_33 = t_33;

  return *this;
}

#endif // MCMATRIX33_HH
