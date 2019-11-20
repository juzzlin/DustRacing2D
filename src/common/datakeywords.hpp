// This file is part of Dust Racing 2D.
// Copyright (C) 2019 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef DATAKEYWORDS_HPP
#define DATAKEYWORDS_HPP

/** Keywords used in the track data files. */
namespace DataKeywords {
namespace Header {
static constexpr auto version = "version";

static constexpr auto track = "track";

static constexpr auto name = "name";

static constexpr auto cols = "cols";

static constexpr auto rows = "rows";

static constexpr auto index = "index";

static constexpr auto user = "isUserTrack";
} // namespace Header

namespace Track {
static constexpr auto node = "n";

static constexpr auto tile = "t";

static constexpr auto object = "o";
} // namespace Track

namespace Node {
static constexpr auto index = "i";

static constexpr auto x = "x";

static constexpr auto y = "y";

static constexpr auto width = "w";

static constexpr auto height = "h";
} // namespace Node

namespace Tile {
static constexpr auto type = "t";

static constexpr auto i = "i";

static constexpr auto j = "j";

static constexpr auto orientation = "o";

static constexpr auto computerHint = "c";

static constexpr auto excludeFromMinimap = "e";
} // namespace Tile

namespace Object {
static constexpr auto role = "r";

static constexpr auto category = "c";

static constexpr auto x = "x";

static constexpr auto y = "y";

static constexpr auto orientation = "o";

static constexpr auto forceStationary = "fs";
} // namespace Object
} // namespace DataKeywords

#endif // DATAKEYWORDS_HPP
