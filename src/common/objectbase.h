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

#ifndef OBJECTBASE_H
#define OBJECTBASE_H

#include <QString>
#include <QPointF>

//! Base class for freely placeable objects in the editor.
class ObjectBase
{
public:

    //! Constructor.
    ObjectBase(QString category, QString role);

    //! Get location in world / scene.
    virtual QPointF location() const;

    //! Set coordinates in the world.
    virtual void setLocation(QPointF newLocation);

    //! Get category.
    QString category() const;

    //! Get role.
    QString role() const;

private:

    //! Category of this object.
    QString m_category;

    //! Role of this object.
    QString m_role;

    //! Coordinates in the world.
    QPointF m_location;
};

#endif // OBJECTBASE_H
