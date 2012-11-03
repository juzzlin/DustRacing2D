// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef OBJECTMODEL_HPP
#define OBJECTMODEL_HPP

#include <QPixmap>
#include <QString>

//! Structure used by ObjectLoader.
struct ObjectModel
{
    //! Path to the image representing this object.
    QPixmap pixmap;

    //! Category of this object.
    QString category;

    //! Role of this object.
    QString role;

    //! Width when added to the scene.
    //! Applies only to certain kind (category) of objects.
    unsigned int width;

    //! Height when added to the scene.
    //! Applies only to certain kind (category) of objects.
    unsigned int height;
};

#endif // OBJECTMODEL_HPP
