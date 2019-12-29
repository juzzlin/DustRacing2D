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

#include "objectfactory.hpp"

#include "mainwindow.hpp"
#include "object.hpp"
#include "objectmodel.hpp"
#include "objectmodelloader.hpp"

std::unique_ptr<Object> ObjectFactory::createObject(QString role)
{
    const auto model = MainWindow::instance()->objectModelLoader().getObjectModelByRole(role);

    size_t w = model.width;
    w = w > 0 ? w : static_cast<size_t>(model.pixmap.width());

    size_t h = model.height;
    h = h > 0 ? h : static_cast<size_t>(model.pixmap.height());

    return std::make_unique<Object>(
      model.category,
      role,
      QSizeF(w, h),
      model.pixmap);
}
