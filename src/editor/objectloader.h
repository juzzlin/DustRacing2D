// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#ifndef TILELOADER_H
#define TILELOADER_H

#include <QtCore>

#include "objectdata.h"

//! Class that loads and manages object models used to
//! build race tracks.
class ObjectLoader
{
public:

    typedef QVector<ObjectData> ObjectDataVector;

    //! Constructor.
    ObjectLoader();

    //! Load data from the given file.
    //! \param filePath The config file to be used.
    //! \return true If failed succeeded.
    bool load(QString path);

    //! Get all objects of the given category.
    ObjectDataVector getObjectsByCategory(QString category) const;

    //! Get all objects of the given role.
    ObjectDataVector getObjectsByRole(QString role) const;

    //! Get all objects.
    ObjectDataVector objects() const;

private:

    Q_DISABLE_COPY(ObjectLoader);

    ObjectDataVector m_objects;
};

#endif // TILELOADER_H
