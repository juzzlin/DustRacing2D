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

#ifndef FADEANIMATION_HPP
#define FADEANIMATION_HPP

#include <QObject>
#include <QTimer>

class FadeAnimation : public QObject
{
    Q_OBJECT

public:
    explicit FadeAnimation(int updateFps = 60);

    bool isFading() const;

signals:

    void fadeInFinished();

    void fadeOutFinished();

    void fadeValueChanged(float);

public slots:

    void beginFadeIn(int preDelayMSec, int msec, int postDelayMSec);

    void beginFadeOut(int preDelayMSec, int msec, int postDelayMSec);

    void beginFadeOutFlash(int preDelayMSec, int msec, int postDelayMSec);

private slots:

    void updateAnimation();

private:
    QTimer m_timer;
    float m_updateFps;
    float m_fadeValue;
    float m_step;
    int m_preDelayMSec;
    int m_postDelayMSec;
    bool m_fadeIn;
};

#endif // FADEANIMATION_HPP
