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

#ifndef UNDOSTACKITEMBASE_HPP
#define UNDOSTACKITEMBASE_HPP

class ObjectModelLoader;
class TrackData;

//! An operation that can executed with 'undo' and 'redo'.
class UndoStackItemBase
{
public:

    UndoStackItemBase()
    {
    }

    virtual void executeUndo(TrackData * track, const ObjectModelLoader & loader) = 0;

    virtual void executeRedo(TrackData * track, const ObjectModelLoader & loader) = 0;

private:

    UndoStackItemBase(const UndoStackItemBase &);
    UndoStackItemBase & operator = (const UndoStackItemBase &);
};

#endif // UNDOSTACKITEMBASE_HPP
