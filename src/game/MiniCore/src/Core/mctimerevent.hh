// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#ifndef MCTIMEREVENT_HH
#define MCTIMEREVENT_HH

#include "mcevent.hh"

class MCTimerEventImpl;

/*! Event that is send to all MCObject's that have subscribed to the event
    by calling MCWorld::subscribeTimerEvent(MCObject &).
 */
class MCTimerEvent : public MCEvent
{
public:
    /*! Constructor.
     *  \param frequency The frequency of the timer.
     */
    explicit MCTimerEvent(unsigned int frequency);

    //! Destructor.
    ~MCTimerEvent();

    //! Return the frequency.
    unsigned int frequency() const;

    //! Return the typeId.
    static unsigned int typeId();

    //! \reimp
    virtual unsigned int instanceTypeId() const;

private:
    DISABLE_COPY(MCTimerEvent);
    DISABLE_ASSI(MCTimerEvent);

    static unsigned int m_typeId;
    MCTimerEventImpl * const m_pImpl;
};

#endif // MCTIMEREVENT_HH
