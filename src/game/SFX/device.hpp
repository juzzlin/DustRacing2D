// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef DEVICE_HPP
#define DEVICE_HPP

namespace SFX {

//! Singleton wrapper for SDL mixer.
class Device
{
public:

    //! Constructor.
    Device();

    //! Destructor.
    virtual ~Device();

    //! \returns The singleton instance.
    static Device & instance();

    //! Initializes SDL_Mixer. Requires, that SDL is initialized.
    bool init(int sampleRate = 44100, int numChannels = 2);

    //! \returns true if the device is initialized.
    bool inited() const;

private:

    //! Hidden copy constructor.
    Device(const Device &);

    //! Hidden assignment.
    Device & operator=(const Device &);

    bool m_inited;

    static Device * m_instance;
};

} // namespace SFX

#endif // DEVICE_HPP
