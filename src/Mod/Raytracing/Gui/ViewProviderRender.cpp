/***************************************************************************
 *   Copyright (c) Luke Parry          (l.parry@warwick.ac.uk)    2012     *
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

#include "PreCompiled.h"

#ifndef _PreComp_
# endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/SoFCSelection.h>
#include <Gui/Selection.h>
#include <Gui/MainWindow.h>
#include <QMenu>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProviderDocumentObjectGroup.h>

#include "ViewProviderRender.h"

using namespace RaytracingGui;

PROPERTY_SOURCE(RaytracingGui::ViewProviderRender, Gui::ViewProviderDocumentObject)


//**************************************************************************
// Construction/Destruction

ViewProviderRender::ViewProviderRender()
{
    sPixmap = "Page";
}

ViewProviderRender::~ViewProviderRender()
{
}

void ViewProviderRender::attach(App::DocumentObject *pcFeat)
{
    // call parent attach method
    ViewProviderDocumentObject::attach(pcFeat);
}

Raytracing::RenderFeature * ViewProviderRender::getRenderFeature(void) const
{
    return dynamic_cast<Raytracing::RenderFeature *>(pcObject);
}

void ViewProviderRender::setDisplayMode(const char* ModeName)
{
    ViewProviderDocumentObject::setDisplayMode(ModeName);
}

std::vector<std::string> ViewProviderRender::getDisplayModes(void) const
{
    // get the modes of the father
    std::vector<std::string> StrList = ViewProviderDocumentObject::getDisplayModes();
    StrList.push_back("Render");
    return StrList;
}

void ViewProviderRender::updateData(const App::Property* prop)
{

}

void ViewProviderRender::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    QAction* act;
    act = menu->addAction(QObject::tr("Show drawing"), receiver, member);
}

bool ViewProviderRender::setEdit(int ModNum)
{
    doubleClicked();
    return false;
}

bool ViewProviderRender::doubleClicked(void)
{
    return true;
}

