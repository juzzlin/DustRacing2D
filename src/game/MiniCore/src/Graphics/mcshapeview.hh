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

#ifndef MCSHAPEVIEW_HH
#define MCSHAPEVIEW_HH

#include "mcbbox3d.hh"
#include "mcglscene.hh"
#include "mcglshaderprogram.hh"
#include "mcmacros.hh"
#include "mctyperegistry.hh"

#include "mcvector3d.hh"

#include <memory>

class MCCamera;
class MCGLObjectBase;

/*! \class MCShapeView.
 *  \brief MCShapeView is an abstract base class (2d) for view objects.
 *
 * MCShape-derived objects uses MCShapeView to actually draw themselves.
 */
class MCShapeView
{
public:
    /*! Constructor.
     *  \param handle Name shared between all views of the same kind.
     *  Used to batch objects during rendering. Internally the view name string is
     *  converted into an integer ID. */
    MCShapeView(const std::string & handle);

    //! Destructor.
    virtual ~MCShapeView();

    //! Set the shader program that is used when rendering.
    virtual void setShaderProgram(MCGLShaderProgramPtr shaderProgram);

    //! Set the shader program that is used when rendering the (fake) 2d shadow.
    virtual void setShadowShaderProgram(MCGLShaderProgramPtr shaderProgram);

    //! Return the default shader program.
    MCGLShaderProgramPtr shaderProgram() const;

    //! Return the default shadow shader program.
    MCGLShaderProgramPtr shadowShaderProgram() const;

    /*! \brief Enable/disable shadow.
     *  True is the default. */
    void setHasShadow(bool flag);

    //! \brief Return true if the view should draw a shadow (renderShadow() is called).
    bool hasShadow() const;

    /*! Render the shape at the current location using given camera window.
     * \param l Location.
     * \param angle Angle in degrees about the Z-axis.
     * \param p Camera window. If nullptr, then no translation or clipping done. */
    virtual void render(const MCVector3d<float> & l, float angle, MCCamera * p = nullptr);

    /*! Render shadow for the shape at the current location using given camera window.
     * \param l Location.
     * \param angle Angle in degrees about the Z-axis.
     * \param p Camera window. If nullptr, then no translation or clipping done. */
    virtual void renderShadow(const MCVector3d<float> & l, float angle, MCCamera * p = nullptr);

    /*! Return centered, non-rotated, non-translated bounding box of the view.
     *  This is used to optimize rendering. Shouldn't be used to detect
     *  collisions or anything that needs exact precision. */
    virtual const MCBBoxF & bbox() const = 0;

    virtual void bind() = 0;

    virtual void bindShadow() = 0;

    virtual void release() = 0;

    virtual void releaseShadow() = 0;

    //! Get the underlying MCGLObjectBase-derived object
    virtual MCGLObjectBase * object() const = 0;

    //! Return the view ID.
    unsigned int viewId() const;

    //! Set view handle (e.g. if surface changes). Changes also the viewId integer.
    void setHandle(std::string handle);

    //! Set color of the view.
    virtual void setColor(const MCGLColor & color);

    //! \return color of the view.
    const MCGLColor & color() const;

    //! Set scaling factors.
    virtual void setScale(const MCVector3dF & scale);

    //! Get scaling factors.
    const MCVector3dF & scale() const;

private:
    //! Disable copy constructor and assignment
    DISABLE_ASSI(MCShapeView);
    DISABLE_COPY(MCShapeView);

    static MCTypeRegistry m_typeRegistry;

    unsigned int m_viewId;

    MCGLShaderProgramPtr m_shaderProgram;

    MCGLShaderProgramPtr m_shadowShaderProgram;

    bool m_hasShadow;

    MCGLColor m_color;

    MCVector3dF m_scale;
};

typedef std::shared_ptr<MCShapeView> MCShapeViewPtr;

#endif // MCSHAPEVIEW_HH
