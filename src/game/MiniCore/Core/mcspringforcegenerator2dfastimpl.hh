// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCSPRINGFORCEGENERATOR2DFASTIMPL_HH
#define MCSPRINGFORCEGENERATOR2DFASTIMPL_HH

//! Implementation class for MCSpringForceGenerator2dFast
class MCSpringForceGenerator2dFastImpl
{
public:
  MCSpringForceGenerator2dFastImpl(MCObject * p2, MCFloat coeff, MCFloat length, MCFloat min, MCFloat max);
  virtual ~MCSpringForceGenerator2dFastImpl();
  inline void updateForce(MCObject * p1);

private:
  MCObject * m_p2;
  MCFloat m_coeff;
  MCFloat m_length;
  MCFloat m_min;
  MCFloat m_max;
  friend class MCSpringForceGenerator2dFast;
};

#endif // MCSPRINGFORCEGENERATOR2DFASTIMPL_HH
