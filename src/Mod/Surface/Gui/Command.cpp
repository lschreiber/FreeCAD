/***************************************************************************
 *   Copyright (c) 2014-2015 Nathan Miller    <Nathan.A.Mill[at]gmail.com> *
 *                           Balázs Bámer                                  *
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
#include <sstream>
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QLineEdit>
#include <QPointer>
#include <Standard_math.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <Inventor/events/SoMouseButtonEvent.h>
#endif

#include <Base/Console.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/Control.h>
#include <Gui/Document.h>
#include <Gui/FileDialog.h>
#include <Gui/MainWindow.h>
#include <Gui/Selection.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/WaitCursor.h>

#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>
#include <App/PropertyLinks.h>
#include "Mod/Part/App/PartFeature.h"
#include "BSurf.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdSurfaceFILLING THIS IS THE SURFACE FILLING COMMAND
//===========================================================================
DEF_STD_CMD(CmdSurfaceFilling);

CmdSurfaceFilling::CmdSurfaceFilling()
  :Command("Surface_Filling")
{
    sAppModule    = "Surface";
    sGroup        = QT_TR_NOOP("Surface");
    sMenuText     = QT_TR_NOOP("Surface Filling function");
    sToolTipText  = QT_TR_NOOP("Fills a series of boundary curves, constraint curves and verticies with a surface.");
    sWhatsThis    = QT_TR_NOOP("Surface Filling function");
    sStatusTip    = QT_TR_NOOP("Surface Filling function");
    sPixmap       = "Filling.svg";
    sAccel        = "CTRL+H";
}

void CmdSurfaceFilling::activated(int iMsg)
{
    Base::Console().Message("Hello, World!\n");
}

//===========================================================================
// CmdSurfaceCut THIS IS THE SURFACE CUT COMMAND
//===========================================================================
DEF_STD_CMD(CmdSurfaceCut);

CmdSurfaceCut::CmdSurfaceCut()
  :Command("Surface_Cut")
{
    sAppModule    = "Surface";
    sGroup        = QT_TR_NOOP("Surface");
    sMenuText     = QT_TR_NOOP("Surface Cut function");
    sToolTipText  = QT_TR_NOOP("Cuts a Shape with another Shape.\nReturns a modified version of the first shape");
    sWhatsThis    = QT_TR_NOOP("Surface Cut function");
    sStatusTip    = QT_TR_NOOP("Surface Cut function");
    sPixmap       = "Cut.svg";
    sAccel        = "CTRL+H";
}

void CmdSurfaceCut::activated(int iMsg)
{
/*    std::vector<Gui::SelectionObject> Sel = getSelection().getSelectionEx(0, Part::Feature::getClassTypeId());
    if (Sel.size() != 2) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Invalid selection"),
            QObject::tr("Select two shapes please."));
        return;
    }

    bool askUser = false;
    for (std::vector<Gui::SelectionObject>::iterator it = Sel.begin(); it != Sel.end(); ++it) {
        App::DocumentObject* obj = it->getObject();
        if (obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {
            const TopoDS_Shape& shape = static_cast<Part::Feature*>(obj)->Shape.getValue();
            if (!PartGui::checkForSolids(shape) && !askUser) {
                int ret = QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Non-solids selected"),
                    QObject::tr("The use of non-solids for boolean operations may lead to unexpected results.\n"
                                "Do you want to continue?"), QMessageBox::Yes, QMessageBox::No);
                if (ret == QMessageBox::No)
                    return;
                askUser = true;
            }
        }
    }

    std::string FeatName = getUniqueObjectName("Cut");
    std::string BaseName  = Sel[0].getFeatName();
    std::string ToolName  = Sel[1].getFeatName();

    openCommand("Part Cut");
    doCommand(Doc,"App.activeDocument().addObject(\"Part::Cut\",\"%s\")",FeatName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Base = App.activeDocument().%s",FeatName.c_str(),BaseName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Tool = App.activeDocument().%s",FeatName.c_str(),ToolName.c_str());
    doCommand(Gui,"Gui.activeDocument().hide('%s')",BaseName.c_str());
    doCommand(Gui,"Gui.activeDocument().hide('%s')",ToolName.c_str());
    copyVisual(FeatName.c_str(), "ShapeColor", BaseName.c_str());
    copyVisual(FeatName.c_str(), "DisplayMode", BaseName.c_str());
    updateActive();
    commitCommand();*/
}


//===========================================================================
// Bezier and BSpline surfaces
//===========================================================================
//DEF_STD_CMD_A(CmdSurfaceBSurf);
class CmdSurfaceBSurf : public Gui::Command, public Surface::ShapeValidator
{
public:
    CmdSurfaceBSurf();
    virtual ~CmdSurfaceBSurf(){}
    virtual const char* className() const
    { return "CmdSurfaceBSurf"; }
protected:
    virtual bool isActive(void);
    void createSurface(const char *surfaceNamePrefix, const char *commandName, const char *pythonAddCommand);
    void showError(const char *msg);
    virtual void activated(int iMsg);
};

CmdSurfaceBSurf::CmdSurfaceBSurf() : Command("Surface_BSurf")
{
    sAppModule    = "Surface";
    sGroup        = QT_TR_NOOP("Surface");
    sMenuText     = QT_TR_NOOP("Bezier or BSpline surface");
    sToolTipText  = QT_TR_NOOP("Creates a surface from 2, 3 or 4 Bezier or BSpline curves");
    sWhatsThis    = "Surface_BSurf";
    sStatusTip    = sToolTipText;
    sPixmap       = "BSplineSurf";
}

bool CmdSurfaceBSurf::isActive(void)
{
    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();
    long size = Sel.size();
    if(size < 1 || size > 4) {
        return false;
    }
    for (std::vector<Gui::SelectionSingleton::SelObj>::iterator it = Sel.begin(); it != Sel.end(); ++it)
    {
        if(!((*it).pObject->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))) {
            return false;
        }
    }
    return true;
}

void CmdSurfaceBSurf::createSurface(const char *surfaceNamePrefix, const char *commandName, const char *pythonAddCommand)
{
    std::string FeatName = getUniqueObjectName(surfaceNamePrefix);
    std::stringstream bspListCmd;
    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();
    bspListCmd << "FreeCAD.ActiveDocument.ActiveObject.BoundaryList = [";
    for (std::vector<Gui::SelectionSingleton::SelObj>::iterator it = Sel.begin(); it != Sel.end(); ++it)
    {
        Part::TopoShape ts = static_cast<Part::Feature*>((*it).pObject)->Shape.getShape();
        bspListCmd << "(App.activeDocument()." << it->FeatName;
        // if it is wire, add as wire
        if((!ts._Shape.IsNull()) && ts._Shape.ShapeType() == TopAbs_WIRE)
        {
            bspListCmd << ", \'Wire1\'),";
        }
        else
        {
            bspListCmd << ", \'Edge1\'),";
        }
    }
    bspListCmd << "]";

    openCommand(commandName);
    doCommand(Doc, pythonAddCommand, FeatName.c_str());
    // invalid fill type meaning the surface is just created and cancel should delete it
    doCommand(Doc, "FreeCAD.ActiveDocument.ActiveObject.FillType=0");
    doCommand(Doc, bspListCmd.str().c_str());
    doCommand(Doc, "Gui.ActiveDocument.setEdit('%s',0)", FeatName.c_str());
    updateActive();
}

void CmdSurfaceBSurf::showError(const char *msg)
{
    QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Error"),
        QObject::tr(msg));
}

void CmdSurfaceBSurf::activated(int iMsg)
{
    initValidator();
    std::vector<Gui::SelectionSingleton::SelObj> Sel = getSelection().getSelection();
    if (Sel.size() > 4) {
        showError("Too many selected objects.");
        return;
    }
    for (std::vector<Gui::SelectionSingleton::SelObj>::iterator it = Sel.begin(); it != Sel.end(); ++it)
    {
        Gui::SelectionSingleton::SelObj selObj = *it;
        if(!(selObj.pObject->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()))) {
            showError("Selected object is not a feature.");
            return;
        }

        Part::TopoShape ts = static_cast<Part::Feature*>(selObj.pObject)->Shape.getShape();
        try {
            checkAndAdd(ts, selObj.SubName);
        }
        catch(Standard_Failure sf) {
            showError(sf.GetMessageString());
            return;
        }
    }
    switch(edgeCount) {
    case 2:
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Warning"),
            QObject::tr("Surfaces with two edges may fail for some fill types."));
        break;
    case 3: // no message
    case 4:
        break;
    default:
        showError("This tool requires 2, 3 or 4 curves.");
        return;
    }

    // check wich was activated and perfom and clearing the flag
    if(willBezier)
    {
        createSurface("BezierSurface", "Create Bezier surface", "FreeCAD.ActiveDocument.addObject(\"Surface::BezSurf\",\"%s\")");
        willBezier = false;
    }
    if(willBSpline)
    {
        createSurface("BSplineSurface", "Create BSpline surface", "FreeCAD.ActiveDocument.addObject(\"Surface::BSplineSurf\",\"%s\")");
        willBSpline = false;
    }
}

void CreateSurfaceCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
/*    rcCmdMgr.addCommand(new CmdSurfaceFilling());
    rcCmdMgr.addCommand(new CmdSurfaceCut());*/
    rcCmdMgr.addCommand(new CmdSurfaceBSurf());
}
