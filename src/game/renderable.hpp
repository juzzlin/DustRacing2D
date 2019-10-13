// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

//! Base class for items and overlays that are rendered on top of the game scene.
class Renderable
{
public:
    //! Constructor.
    Renderable()
      : m_x(0)
      , m_y(0)
      , m_w(0)
      , m_h(0)
    {
    }

    //! Destructor.
    virtual ~Renderable()
    {
    }

    //! Render.
    virtual void render() = 0;

    //! Set centered position.
    void setPos(int x, int y)
    {
        m_x = x;
        m_y = y;
    }

    //! Get x.
    int x() const
    {
        return m_x;
    }

    //! Get y.
    int y() const
    {
        return m_y;
    }

    //! Set dimensions.
    virtual void setDimensions(int width, int height)
    {
        m_w = width;
        m_h = height;
    }

    //! Get width.
    int width() const
    {
        return m_w;
    }

    //! Get height.
    int height() const
    {
        return m_h;
    }

private:
    int m_x;

    int m_y;

    int m_w;

    int m_h;
};

#endif // RENDERABLE_HPP
