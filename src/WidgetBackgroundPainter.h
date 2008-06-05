/***************************************************************************
 *   Copyright (c) 2008  Nikolaj Hald Nielsen <nhnFreespirit@gmail.com>    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
 
#ifndef WIDGETBACKGROUNDPAINTER_H
#define WIDGETBACKGROUNDPAINTER_H

#include "amarok_export.h"

/**
A sinlgeton class for rendering the correct part of the main background for placement somewhere within a widget

	@author Nikolaj Hald Nielsen <nhnFreespirit@gmail.com>
*/

class QPixmap;
class QString;
class QWidget;

class AMAROK_EXPORT WidgetBackgroundPainter{
public:

    static WidgetBackgroundPainter * instance();

    ~WidgetBackgroundPainter() {};

    QPixmap getBackground( QWidget * parent, int x, int y, int width, int height, bool ignoreCache = false );
    QPixmap getBackground( const QString name, int globalAreaX, int globalAreaY, int x, int y, int width, int height, bool ignoreCache = false );

private:
    WidgetBackgroundPainter() {};

    static WidgetBackgroundPainter * m_instance;
    
};

#endif
