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

#ifndef MCCAMERA_HH
#define MCCAMERA_HH

#include "mcbbox.hh"
#include "mcmacros.hh"

/*! \class MCCamera.
 *  \brief Scene camera window.
 *
 * The Camera represents a visible window to the game scene. 
 * The Camera window is moved along a human player and it can
 * translate global scene coordinates of objects into relative
 * "camera coordinates". Those camera coordinates are used when
 * rendering objects onto the screen to get the effect of a scrolling
 * background.
 */
class MCCamera
{
public:
    //! Default constructor.
    MCCamera();

    /*! \brief Constructor.
     *  \param w Width of the window.
     *  \param h Height of the window.
     *  \param x Center x of the window.
     *  \param y Center y of the window.
     *  \param maxX Maximum allowed x for the window.
     *  \param maxY Maximum allowed y for the window.
     */
    MCCamera(float w, float h, float x, float y, float maxX, float maxY);

    /*! \brief Re-init.
     *  \param w Width of the window.
     *  \param h Height of the window.
     *  \param x Center x of the window.
     *  \param y Center y of the window.
     *  \param maxX Maximum allowed x for the window.
     *  \param maxY Maximum allowed y for the window.
     */
    void init(float w, float h, float x, float y, float maxX, float maxY);

    /*! \brief Set Camera location in the scene.
     *  \param x Center x of the window.
     *  \param y Center y of the window.
     */
    void setPos(float x, float y);

    /*! \brief Set Camera location in the scene.
     *  \param pos Center location of the window.
     */
    void setPos(const MCVector2dF & pos);

    const MCVector2dF & pos() const;

    float x() const;

    float y() const;

    //! Get bounding box
    MCBBox<float> bbox() const;

    //! Get width
    float width() const;

    //! Get height
    float height() const;

    //! Translate given scene x-coordinate into Camera coordinate
    float mapXToCamera(float x) const
    {
        return x - m_pos.i() + m_halfW;
    }

    //! Translate given scene y-coordinate into Camera coordinate
    float mapYToCamera(float y) const
    {
        return y - m_pos.j() + m_halfH;
    }

    //! Translate given scene coordinates in-place into Camera coordinates
    void mapToCamera(float & x, float & y)
    {
        x = x - m_pos.i() + m_halfW;
        y = y - m_pos.j() + m_halfH;
    }

    //! Translate given Camera x-coordinate into scene coordinate
    float mapXToScene(float x) const
    {
        return x + m_pos.i() - m_halfW;
    }

    //! Translate given Camera y-coordinate into scene coordinate
    float mapYToScene(float y) const
    {
        return y + m_pos.j() - m_halfH;
    }

    //! Translate given Camera coordinates in-place into scene coordinates
    void mapToScene(float & x, float & y)
    {
        x = x + m_pos.i() - m_halfW;
        y = y + m_pos.j() - m_halfH;
    }

    //! Test if given BBox is visible through the current
    //! camera window
    bool isVisible(const MCBBox<float> & r) const
    {
        const float w2 = r.width() / 2;
        const float h2 = r.height() / 2;

        // Give some space to possible shadows, that's why we're adding w2 and h2.
        return MCBBox<float>(
                 m_pos.i() - m_halfW - w2,
                 m_pos.j() - m_halfH - h2,
                 m_pos.i() + m_halfW + w2,
                 m_pos.j() + m_halfH + h2)
          .intersects(r);
    }

private:
    //! Disable copy constructor
    DISABLE_COPY(MCCamera);

    //! Disable assignment
    DISABLE_ASSI(MCCamera);

    //! Width and height of the camera window
    float m_w, m_h;

    //! Width and height of the camera window divided by 2
    float m_halfW, m_halfH;

    //! Coordinates of the center of the camera window
    MCVector2dF m_pos;

    //! Maximum camera coordinates
    float m_maxX, m_maxY;
};

#endif // MCCAMERA_HH
