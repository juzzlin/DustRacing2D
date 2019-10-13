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

#ifndef OBJECTBASE_HPP
#define OBJECTBASE_HPP

#include <QPointF>
#include <QString>

#include <memory>

//! Base class for freely placeable objects in the editor.
class ObjectBase
{
public:
    //! Constructor.
    ObjectBase(QString category, QString role);

    ObjectBase(ObjectBase & other) = delete;

    ObjectBase & operator=(ObjectBase & other) = delete;

    //! Destructor.
    virtual ~ObjectBase();

    //! Get location in world / scene.
    virtual QPointF location() const;

    //! Set coordinates in the world.
    virtual void setLocation(QPointF newLocation);

    //! Get category.
    QString category() const;

    //! Get role.
    QString role() const;

    bool forceStationary() const;

    void setForceStationary(bool forceStationary);

private:
    //! Category of this object.
    QString m_category;

    //! Role of this object.
    QString m_role;

    //! Coordinates in the world.
    QPointF m_location;

    bool m_forceStationary;
};

using ObjectBasePtr = std::shared_ptr<ObjectBase>;

#endif // OBJECTBASE_HPP
