// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Markus Rinne <markus.ka.rinne@gmail.com>
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

#ifndef TILETYPEUNDOSTACKITEM_HPP
#define TILETYPEUNDOSTACKITEM_HPP

#include "undostackitembase.hpp"

#include <QPoint>
#include <QString>
#include <vector>

//! An operation that changes the type of one or more tiles.
class TileTypeUndoStackItem : public UndoStackItemBase
{
public:

    //! Constructor
    TileTypeUndoStackItem(const std::vector<QPoint> & positions, const QString & oldType, const QString & newType);

    virtual void executeUndo(TrackData * track, const ObjectModelLoader & loader);

    virtual void executeRedo(TrackData * track, const ObjectModelLoader & loader);

private:

    Q_DISABLE_COPY(TileTypeUndoStackItem)

    void setTiles(TrackData * track, const ObjectModelLoader & loader, QString type);

    std::vector<QPoint> m_positions;
    QString m_oldType;
    QString m_newType;
};

#endif
