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

#ifndef CRASHOVERLAY_HPP
#define CRASHOVERLAY_HPP

#include "overlaybase.hpp"

#include <memory>

class MCSurface;
class Car;

class CrashOverlay : public OverlayBase
{
public:
    CrashOverlay();

    //! \reimp
    virtual void render() override;

    //! \reimp
    virtual void setDimensions(size_t width, size_t height) override;

    //! \reimp
    virtual bool update() override;

    void setCarToFollow(Car & car);

private:
    std::shared_ptr<MCSurface> m_surface;

    float m_alpha;

    Car * m_car;

    bool m_isTriggered;
};

#endif // CRASHOVERLAY_HPP
