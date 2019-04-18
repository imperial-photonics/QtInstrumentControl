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

#ifndef QLEDINDICATOR_H
#define QLEDINDICATOR_H

#include <QAbstractButton>
#include <QResizeEvent>
#include <QColor>
#include <QDebug>

enum LedStatus
{
   LedOK       = 0,
   LedWarning  = 1,
   LedCritical = 2
};

class QLedIndicator : public QWidget
{
    Q_OBJECT
    public:
        QLedIndicator(QWidget *parent = 0);
        void setStatus(LedStatus ledStatus);
        LedStatus getStatus();
    protected:
        virtual void paintEvent (QPaintEvent *event);
        virtual void resizeEvent(QResizeEvent *event);

    private:
        static const qreal scaledSize;  /* init in cpp */
        QPair<QColor, QColor> okColor, warningColor, criticalColor;
        QPixmap ledBuffer;
        LedStatus ledStatus = LedOK;
};

#endif // QLEDINDICATOR_H
