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

#ifndef EDITORSCENE_HPP
#define EDITORSCENE_HPP

#include <QGraphicsScene>

//! Graphics scene of the editor.
class EditorScene : public QGraphicsScene
{
    Q_OBJECT

public:

    //! Constructor.
    explicit EditorScene(QObject * parent = nullptr);
};

#endif // EDITORSCENE_HPP
