/***************************************************************************
 *   Copyright (c) 2010 Thomas Anderson <ta@nextgenengineering>            *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/



#ifndef GUIAPPLICATIONNATIVEEVENTAWARE_H
#define GUIAPPLICATIONNATIVEEVENTAWARE_H

#include <QApplication>

class QMainWindow;

namespace Gui
{
    class GUIApplicationNativeEventAware : public QApplication
    {
        Q_OBJECT
    public:
        GUIApplicationNativeEventAware(int &argc, char *argv[]);
        ~GUIApplicationNativeEventAware();
        void initSpaceball(QMainWindow *window);
        bool processSpaceballEvent(QObject *object, QEvent *event);
#ifdef Q_WS_X11
        bool x11EventFilter(XEvent *event);
#endif

#ifdef Q_WS_WIN
        bool winEventFilter(MSG *msg, long *result);
#endif
        bool isSpaceballPresent(){return spaceballPresent;}
    private:
        bool spaceballPresent;
        QMainWindow *mainWindow;
    };
}
#endif // GUIAPPLICATIONNATIVEEVENTAWARE_H
