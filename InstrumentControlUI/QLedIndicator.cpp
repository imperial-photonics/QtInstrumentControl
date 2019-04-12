/***************************************************************************
 *   Copyright (C) 2010 by Tn                                              *
 *   thenobody@poczta.fm                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QPainter>

#include "qledindicator.h"

const qreal QLedIndicator::scaledSize = 1000; /* Visual Studio static const mess */

QLedIndicator::QLedIndicator(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(22,22);

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    okColor = { QColor(0,255,0), QColor(0,192,0) };

    warningColor = { QColor(255,255,0), QColor(192,192,0) };
    criticalColor = { QColor(255,0,0), QColor(192,0,0) };
}

void QLedIndicator::setStatus(LedStatus ledStatus_)
{
   ledStatus = ledStatus_;
   update();
}

LedStatus QLedIndicator::getStatus()
{
   return ledStatus;
}

void QLedIndicator::resizeEvent(QResizeEvent *event) {
    update();
}

void QLedIndicator::paintEvent(QPaintEvent *event) {
    qreal realSize = qMin(width(), height());

    QRadialGradient gradient;
    QPainter painter(this);
    QPen     pen(Qt::black);
             pen.setWidth(1);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width()/2, height()/2);
    painter.scale(realSize/scaledSize, realSize/scaledSize);

    gradient = QRadialGradient (QPointF(-500,-500), 1500, QPointF(-500,-500));
    gradient.setColorAt(0, QColor(224,224,224));
    gradient.setColorAt(1, QColor(28,28,28));
    painter.setPen(pen);
    painter.setBrush(QBrush(gradient));
    painter.drawEllipse(QPointF(0,0), 500, 500);

    gradient = QRadialGradient (QPointF(500,500), 1500, QPointF(500,500));
    gradient.setColorAt(0, QColor(224,224,224));
    gradient.setColorAt(1, QColor(28,28,28));
    painter.setPen(pen);
    painter.setBrush(QBrush(gradient));
    painter.drawEllipse(QPointF(0,0), 450, 450);

    painter.setPen(pen);
    QPair<QColor, QColor> color;
    if (ledStatus == LedOK)
       color = okColor;
    else if (ledStatus == LedWarning)
       color = warningColor;
    else if (ledStatus == LedCritical)
       color = criticalColor;
    
    gradient = QRadialGradient(QPointF(-500, -500), 1500, QPointF(-500, -500));
    gradient.setColorAt(0, color.first);
    gradient.setColorAt(1, color.second);

    painter.setBrush(gradient);
    painter.drawEllipse(QPointF(0,0), 400, 400);
}

