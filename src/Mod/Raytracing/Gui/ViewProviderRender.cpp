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
#include <Gui/Document.h>
#include <Gui/SoFCSelection.h>
#include <Gui/Selection.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventor.h>
#include <QMenu>
#include <QDropEvent>
#include <QMimeData>
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
    // ensure that we are in sketch only selection mode
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

bool ViewProviderRender::mouseMove(const SbVec3f &pos, const SbVec3f &norm, const SoPickedPoint* pp) {

    return true;
}

void ViewProviderRender::setupContextMenu(QMenu *menu, QObject *receiver, const char *member)
{
    menu->addAction(QObject::tr("Edit Render"), receiver, member);
}

bool ViewProviderRender::setEdit(int ModNum)
{
   // When double-clicking on the item for this sketch the
    // object unsets and sets its edit mode without closing
    // the task panel
//     Gui::TaskView::TaskDialog *dlg = Gui::Control().activeDialog();
//     TaskDlgAppearances *sketchDlg = qobject_cast<TaskDlgEditSketch *>(dlg);
//     if (sketchDlg && sketchDlg->getSketchView() != this)
//         sketchDlg = 0; // another sketch left open its task panel
//     if (dlg && !sketchDlg) {
//         QMessageBox msgBox;
//         msgBox.setText(QObject::tr("A dialog is already open in the task panel"));
//         msgBox.setInformativeText(QObject::tr("Do you want to close this dialog?"));
//         msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//         msgBox.setDefaultButton(QMessageBox::Yes);
//         int ret = msgBox.exec();
//         if (ret == QMessageBox::Yes)
//             Gui::Control().closeDialog();
//         else
//             return false;
//     }

    // clear the selection (convenience)
    Gui::Selection().clearSelection();


    // start the edit dialog
//     if (sketchDlg)
//         Gui::Control().showDialog(sketchDlg);
//     else
//         Gui::Control().showDialog(new TaskDlgEditSketch(this));
// 

    return true;
}

bool ViewProviderRender::doubleClicked(void)
{
    return true;
}

