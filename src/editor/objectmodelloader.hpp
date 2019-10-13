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

#ifndef OBJECT_MODEL_LOADER_HPP
#define OBJECT_MODEL_LOADER_HPP

#include <QtCore>

#include "objectmodel.hpp"

//! Class that loads and manages object models used to
//! build race tracks.
class ObjectModelLoader
{
public:
    typedef QVector<ObjectModel> ObjectDataVector;

    //! Constructor.
    ObjectModelLoader();

    //! Load data from the given file.
    //! \param filePath The config file to be used.
    //! \return true If failed succeeded.
    bool load(QString path);

    //! Get all object models of the given category.
    ObjectDataVector getObjectModelsByCategory(QString category) const;

    //! Get object model of the given role.
    ObjectModel getObjectModelByRole(QString role) const;

    //! Get category by role.
    QString getCategoryByRole(QString role) const;

    //! Get all objects.
    ObjectDataVector objects() const;

    //! Get pixmap by role.
    QPixmap getPixmapByRole(QString role) const;

private:
    Q_DISABLE_COPY(ObjectModelLoader)

    ObjectDataVector m_objects;
};

#endif // OBJECT_MODEL_LOADER_HPP
