// This file is part of Dust Rallycross (DustRAC).
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

#ifndef EDITOR_H
#define EDITOR_H

#include <QString>

/*! \class Editor
 *  \brief Editor model.
 */
class Editor
{
public:

    //! Constructor
    Editor();
    
    //! Destructor
    ~Editor();

    //! Load the given level file
    bool load(const QString & file);
    
    //! Save current data to the given file
    bool save(const QString & file);
    
private:
};

#endif // EDITOR_H


