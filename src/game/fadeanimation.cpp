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

#include "fadeanimation.hpp"
#include <cmath>

FadeAnimation::FadeAnimation(int updateFps)
  : m_updateFps(updateFps)
  , m_fadeValue(0.0)
  , m_fadeIn(false)
{
    m_timer.setInterval(1000 / updateFps);
    connect(&m_timer, &QTimer::timeout, this, &FadeAnimation::updateAnimation);
}

bool FadeAnimation::isFading() const
{
    return m_timer.isActive();
}

void FadeAnimation::beginFadeIn(int preDelayMSec, int msec, int postDelayMSec)
{
    m_preDelayMSec = preDelayMSec / m_updateFps;
    m_postDelayMSec = postDelayMSec / m_updateFps;
    m_fadeValue = 0.0;
    m_step = 1.0 / (msec / m_updateFps);
    m_fadeIn = true;

    emit fadeValueChanged(m_fadeValue);

    m_timer.start();
}

void FadeAnimation::beginFadeOut(int preDelayMSec, int msec, int postDelayMSec)
{
    m_preDelayMSec = preDelayMSec / m_updateFps;
    m_postDelayMSec = postDelayMSec / m_updateFps;
    m_fadeValue = 1.0;
    m_step = m_fadeValue / (msec / m_updateFps);
    m_fadeIn = false;

    emit fadeValueChanged(m_fadeValue);

    m_timer.start();
}

void FadeAnimation::beginFadeOutFlash(int preDelayMSec, int msec, int postDelayMSec)
{
    m_preDelayMSec = preDelayMSec / m_updateFps;
    m_postDelayMSec = postDelayMSec / m_updateFps;
    m_fadeValue = 10.0;
    m_step = m_fadeValue / (msec / m_updateFps);
    m_fadeIn = false;

    emit fadeValueChanged(m_fadeValue);

    m_timer.start();
}

void FadeAnimation::updateAnimation()
{
    if (m_preDelayMSec)
    {
        m_preDelayMSec--;
    }
    else if (m_fadeIn && m_fadeValue < 1.0)
    {
        m_fadeValue += m_step;
#ifdef Q_OS_ANDROID
        emit fadeValueChanged(fmin(m_fadeValue, 1.0));
#else
        emit fadeValueChanged(std::fmin(m_fadeValue, 1.0));
#endif
    }
    else if (!m_fadeIn && m_fadeValue > 0.0)
    {
        m_fadeValue -= m_step;
#ifdef Q_OS_ANDROID
        emit fadeValueChanged(fmax(m_fadeValue, 0.0));
#else
        emit fadeValueChanged(std::fmax(m_fadeValue, 0.0));
#endif
    }
    else if (m_postDelayMSec)
    {
        m_postDelayMSec--;
    }
    else
    {
        m_timer.stop();

        if (m_fadeIn)
        {
            emit fadeInFinished();
        }
        else
        {
            emit fadeOutFinished();
        }
    }
}
