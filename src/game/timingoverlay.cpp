// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "timingoverlay.h"

#include "MiniCore/Core/MCCamera"
#include "MiniCore/Text/MCTextureFontManager"
#include "MiniCore/Text/MCTextureText"

TimingOverlay::TimingOverlay(MCTextureFontManager & fontManager)
  : m_fontManager(fontManager)
{
}

void TimingOverlay::render(MCCamera * pCamera)
{
    MCTextureFont & defaultMonospace = m_fontManager.font("default");
    MCTextureText test("1234567890\nabcdefghijklmnopqrstuvwxyz\nABCDEFGHIJKLMNOPQRSTUVWXYZ");
    test.render(300, 300, pCamera, defaultMonospace);
}

TimingOverlay::~TimingOverlay()
{
}
