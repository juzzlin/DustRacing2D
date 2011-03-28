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

#include "tracktile.h"
#include "tiletypedialog.h"
#include "tileanimator.h"
#include "mainwindow.h"

#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QAction>

// Width of the highlight shown when the tile is active
const int HIGHLIGHT_WIDTH = 11;

TrackTile * TrackTile::m_activeTile = NULL;

TrackTile::TrackTile(QPointF location, const QString & type):
    m_size(QSizeF(TILE_W, TILE_H)),
    m_tileType(type),
    m_active(false),
    m_animator(new TileAnimator(this))
{
    setPos(location);
    createContextMenu();
}

void TrackTile::createContextMenu()
{
    QChar degreeSign(176);

    QString dummy1(QString(QWidget::tr("Rotate 90")) +
                   degreeSign + QWidget::tr(" CW.."));

    QAction * rotate90CW = new QAction(dummy1, &m_menu);
    QObject::connect(rotate90CW, SIGNAL(triggered()), m_animator, SLOT(rotate90CW()));

    QString dummy2(QString(QWidget::tr("Rotate 90")) +
                   degreeSign + QWidget::tr(" CCW.."));

    QAction * rotate90CCW = new QAction(dummy2, &m_menu);
    QObject::connect(rotate90CCW, SIGNAL(triggered()), m_animator, SLOT(rotate90CCW()));

    m_menu.addActions(QList<QAction *>() << rotate90CW << rotate90CCW);
}

QRectF TrackTile::boundingRect () const
{
    return QRectF(-m_size.width() / 2, -m_size.height() / 2,
                   m_size.width(), m_size.height());
}

void TrackTile::paint(QPainter * painter,
                      const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->save();

    QPen pen;
    pen.setJoinStyle(Qt::MiterJoin);

    if (m_tileType == "grass")
    {
        painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2,
                            m_size.width(), m_size.height(),
                            QPixmap(":/data/images/grass.png"));
    }
    else if (m_tileType == "straight")
    {
        painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2,
                            m_size.width(), m_size.height(),
                            QPixmap(":/data/images/straight.png"));
    }
    else if (m_tileType == "corner")
    {
        painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2,
                            m_size.width(), m_size.height(),
                            QPixmap(":/data/images/corner.png"));
    }
    else if (m_tileType == "finish")
    {
        painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2,
                            m_size.width(), m_size.height(),
                            QPixmap(":/data/images/finish.png"));
    }
    else if (m_tileType == "clear")
    {
        painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2,
                            m_size.width(), m_size.height(),
                            QPixmap(":/data/images/clear.png"));

        pen.setColor(QColor(0, 0, 0));
        painter->setPen(pen);
        painter->drawRect(-m_size.width() / 2, -m_size.height() / 2,
                           m_size.width(),      m_size.height());
    }

    if (m_active)
    {
        pen.setColor(QColor(0, 0, 255, 64));
        pen.setWidth(HIGHLIGHT_WIDTH);

        painter->setPen(pen);
        painter->drawRect(-m_size.width()  / 2 + HIGHLIGHT_WIDTH / 2,
                          -m_size.height() / 2 + HIGHLIGHT_WIDTH / 2,
                           m_size.width()      - HIGHLIGHT_WIDTH,
                           m_size.height()     - HIGHLIGHT_WIDTH);
    }

    painter->restore();
}

void TrackTile::setActive(bool active)
{
    m_active = active;

    if (active && TrackTile::m_activeTile != this)
    {
        if (TrackTile::m_activeTile)
        {
            TrackTile::m_activeTile->setActive(false);
        }

        TrackTile::m_activeTile = this;
    }

    update();
}

void TrackTile::setActiveTile(TrackTile * tile)
{
    if (tile)
    {
        tile->setActive(true);
    }
    else
    {
        if (activeTile())
        {
            activeTile()->setActive(false);
        }

        TrackTile::m_activeTile = NULL;
    }
}

TrackTile * TrackTile::activeTile()
{
    return TrackTile::m_activeTile;
}

void TrackTile::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    setActive(true);

    if (event->button() == Qt::RightButton)
    {
        if (scene() && !scene()->views().isEmpty())
        {
            if (QGraphicsView * view = scene()->views()[0])
            {
                QPoint posInView = view->mapFromScene(pos() + event->pos().toPoint());
                QPoint globalPos = view->mapToGlobal(posInView);

                // Show context menu
                m_menu.exec(globalPos);
            }
        }
    }
    else if (event->button() == Qt::LeftButton)
    {
        if (QAction * action = MainWindow::instance()->currentToolBarAction())
            setTileType(action->data().toString());
    }

    QGraphicsItem::mousePressEvent(event);
}

void TrackTile::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void TrackTile::setTileType(const QString & type)
{
    if (type != m_tileType)
    {
        m_tileType = type;

        update();
    }
}

const QString & TrackTile::tileType() const
{
    return m_tileType;
}

TrackTile::~TrackTile()
{
    delete m_animator;
}
