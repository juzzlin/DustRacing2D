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
#include "trackdata.h"
#include "tiletypedialog.h"
#include "tileanimator.h"
#include "mainwindow.h"

#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QAction>

TrackTile * TrackTile::m_activeTile = NULL;
bool        TrackTile::m_routeMode  = false;

TrackTile::TrackTile(TrackData * trackData, QPointF location, QPoint matrixLocation,
                     const QString & type):
    m_trackData(trackData),
    m_size(QSizeF(TILE_W, TILE_H)),
    m_tileType(type),
    m_active(false),
    m_animator(new TileAnimator(this)),
    m_routeIndex(-1),
    m_matrixLocation(matrixLocation),
    m_routeDirection(TrackTile::RD_NONE)
{
    setPos(location);
    createContextMenu();
}

void TrackTile::setRouteMode(bool enable)
{
    TrackTile::m_routeMode = enable;
}

void TrackTile::setRouteIndex(int index)
{
    m_routeIndex = index;
    update();
}

int TrackTile::routeIndex() const
{
    return m_routeIndex;
}

void TrackTile::setRouteDirection(TrackTile::RouteDirection direction)
{
    m_routeDirection = direction;
    update();
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

    // Render the tile image
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

    // Render highlight
    if (m_active)
    {
        painter->fillRect(boundingRect(), QBrush(QColor(0, 0, 0, 64)));
    }

    // Render route index and arrow
    if (m_routeIndex >= 0)
    {
        pen.setColor(QColor(255, 255, 255, 127));
        painter->setPen(pen);

        // Cancel possible rotation so that the text is not
        // rotated.
        QTransform transform;
        transform.rotate(-rotation());
        painter->setTransform(transform, true);

        switch (m_routeDirection)
        {
        case RD_LEFT:
            {
                QPainterPath path;
                QPolygon triangle;
                triangle << QPoint(-m_size.width() / 2,  0)
                         << QPoint(-m_size.width() / 3,  m_size.height() / 3)
                         << QPoint(-m_size.width() / 3, -m_size.height() / 3);
                path.addPolygon(triangle);
                painter->fillPath(path, QBrush(QColor(255, 255, 255, 127)));
            }
            break;

        case RD_RIGHT:
            {
                QPainterPath path;
                QPolygon triangle;
                triangle << QPoint(m_size.width() / 2,  0)
                         << QPoint(m_size.width() / 3, -m_size.height() / 3)
                         << QPoint(m_size.width() / 3,  m_size.height() / 3);
                path.addPolygon(triangle);
                painter->fillPath(path, QBrush(QColor(255, 255, 255, 127)));
            }
            break;

        case RD_UP:
            {
                QPainterPath path;
                QPolygon triangle;
                triangle << QPoint(0, -m_size.height() / 2)
                         << QPoint( m_size.width() / 3, -m_size.height() / 3)
                         << QPoint(-m_size.width() / 3, -m_size.height() / 3);
                path.addPolygon(triangle);
                painter->fillPath(path, QBrush(QColor(255, 255, 255, 127)));
            }
            break;

        case RD_DOWN:
            {
                QPainterPath path;
                QPolygon triangle;
                triangle << QPoint(0, m_size.height() / 2)
                         << QPoint( m_size.width() / 3, m_size.height() / 3)
                         << QPoint(-m_size.width() / 3, m_size.height() / 3);
                path.addPolygon(triangle);
                painter->fillPath(path, QBrush(QColor(255, 255, 255, 127)));
            }
            break;

        default:
        case RD_NONE:
            break;
        }

        if (m_routeIndex == 0)
        {
            QFont font;
            font.setPixelSize(m_size.height() / 2);
            painter->setFont(font);
            painter->drawText(boundingRect(), Qt::AlignCenter, "S");
        }
    }

    painter->restore();
}

void TrackTile::setActive(bool active)
{
    m_active = active;

    if (active && TrackTile::m_activeTile != this)
    {
        if (TrackTile::m_activeTile)
            TrackTile::m_activeTile->setActive(false);

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
            activeTile()->setActive(false);

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

    // Handle right button click
    if (event->button() == Qt::RightButton)
    {
        // Show the context menu
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
    // Handle left button click
    else if (event->button() == Qt::LeftButton)
    {
        // User is defining the route
        if (TrackTile::m_routeMode)
        {
            // Push tile to the route
            m_routeIndex = m_trackData->route().push(this);

            // Check if we might have a loop => end
            if (m_routeIndex == 0 && m_trackData->route().length() > 1)
            {
                TrackTile::m_routeMode = false;
                MainWindow::instance()->endSetRoute();
            }
        }
        // User is setting the tile type
        else
        {
            if (QAction * action = MainWindow::instance()->currentToolBarAction())
                setTileType(action->data().toString());
        }
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

QPoint TrackTile::matrixLocation() const
{
    return m_matrixLocation;
}

TrackTile::~TrackTile()
{
    delete m_animator;
}
