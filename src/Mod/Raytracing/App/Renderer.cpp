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
# include <BRep_Tool.hxx>
# include <BRepMesh_IncrementalMesh.hxx>
# include <GeomAPI_ProjectPointOnSurf.hxx>
# include <GeomLProp_SLProps.hxx>
# include <Poly_Triangulation.hxx>
# include <TopExp_Explorer.hxx>
# include <sstream>
# include <QString>
#include <boost/concept_check.hpp>
# include <TopoDS.hxx>
# include <TopoDS_Face.hxx>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include <App/ComplexGeoData.h>

#include <Base/Writer.h>
#include <Base/Reader.h>
#include "Renderer.h"

using namespace Raytracing;

TYPESYSTEM_SOURCE_ABSTRACT(Raytracing::Renderer, Base::BaseClass)

Renderer::Renderer(void)
{
    process = 0;
    camera  = 0;
    xRes = 0;
    yRes = 0;
  
}
Renderer::~Renderer(void)
{
    this->clear();
}

void Renderer::clear()
{
    delete this->camera;
    camera = 0;

    for (std::vector<RenderLight *>::iterator it = lights.begin(); it != lights.end(); ++it) {
      delete *it;
    }
    for (std::vector<RenderPart *>::iterator it = parts.begin(); it != parts.end(); ++it) {
      delete *it;
    }

    lights.clear();
    parts.clear();
}

void Renderer::addCamera(RenderCamera *cam) {
    this->camera = cam;
}

void Renderer::addLight(RenderLight *light) {
  this->lights.push_back(light);
}


void Renderer::addPart( RenderPart *part) {
  this->parts.push_back(part);
}

void Renderer::addObject(const char *PartName, const TopoDS_Shape &Shape, float meshDeviation)
{
    RenderPart *part = new RenderPart(PartName, Shape, meshDeviation);
    addPart(part);
}

void Renderer::attachRenderProcess(RenderProcess *Process)
{
  this->process = Process; // The baseclass will have to determine the process type
}

bool Renderer::getOutputStream(QTextStream &ts)
{
    //Check if temporary file can be open
    if(!inputFile.open())
        return false;

    ts.setDevice(&inputFile);
}

void Renderer::finish()
{
  if(!process || !process->isActive())
      return; // The process cannot be stopped because it's not active
  this->process->stop();
}

void Renderer::render()
{
    if(this->process && this->process->isActive())
      return;

    if(!inputFile.open())
      return;

    inputFile.resize(0); // Clear the previous contents and regenerate
    generateScene();

    process->setInputPath(inputFile.fileName());
    process->setOutputPath(QString::fromAscii(outputPath.c_str()));
    process->begin();
}
void Renderer::setCamera(const Base::Vector3d &camPos, const Base::Vector3d &camDir, const Base::Vector3d &lookAt, const Base::Vector3d &up) {
  if(!camera)
    return;
  
    camera->CamPos = camPos;
    camera->CamDir = camDir;
    camera->LookAt = lookAt;
    camera->Up     = up;
}

// void Renderer::loadSceneDefinition(const char *file) {}
// void Renderer::loadXML(Base::XMLReader &/*reader*/) {}

void Renderer::transferToArray(const TopoDS_Face& aFace,gp_Vec** vertices,gp_Vec** vertexnormals, long** cons,int &nbNodesInFace,int &nbTriInFace ) {
    TopLoc_Location aLoc;

    // doing the meshing and checking the result
    //BRepMesh_IncrementalMesh MESH(aFace,fDeflection);
    Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
    if (aPoly.IsNull()) {
        Base::Console().Log("Empty face trianglutaion\n");
        nbNodesInFace =0;
        nbTriInFace = 0;
        vertices = 0l;
        cons = 0l;
        return;
    }

    // geting the transformation of the shape/face
    gp_Trsf myTransf;
    Standard_Boolean identity = true;
    if (!aLoc.IsIdentity())  {
        identity = false;
        myTransf = aLoc.Transformation();
    }

    Standard_Integer i;
    // geting size and create the array
    nbNodesInFace = aPoly->NbNodes();
    nbTriInFace = aPoly->NbTriangles();
    *vertices = new gp_Vec[nbNodesInFace];
    *vertexnormals = new gp_Vec[nbNodesInFace];
    for (i=0; i < nbNodesInFace; i++) {
        (*vertexnormals)[i]= gp_Vec(0.0,0.0,0.0);
    }

    *cons = new long[3*(nbTriInFace)+1];

    // check orientation
    TopAbs_Orientation orient = aFace.Orientation();

    // cycling through the poly mesh
    const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
    const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
    for (i=1; i<=nbTriInFace; i++) {
        // Get the triangle
        Standard_Integer N1,N2,N3;
        Triangles(i).Get(N1,N2,N3);

        // change orientation of the triangles
        if ( orient != TopAbs_FORWARD )
        {
            Standard_Integer tmp = N1;
            N1 = N2;
            N2 = tmp;
        }

        gp_Pnt V1 = Nodes(N1);
        gp_Pnt V2 = Nodes(N2);
        gp_Pnt V3 = Nodes(N3);

        // transform the vertices to the place of the face
        if (!identity) {
            V1.Transform(myTransf);
            V2.Transform(myTransf);
            V3.Transform(myTransf);
        }

        // Calculate triangle normal
        gp_Vec v1(V1.X(),V1.Y(),V1.Z()),v2(V2.X(),V2.Y(),V2.Z()),v3(V3.X(),V3.Y(),V3.Z());
        gp_Vec Normal = (v2-v1)^(v3-v1);

        //Standard_Real Area = 0.5 * Normal.Magnitude();

        // add the triangle normal to the vertex normal for all points of this triangle
        (*vertexnormals)[N1-1] += gp_Vec(Normal.X(),Normal.Y(),Normal.Z());
        (*vertexnormals)[N2-1] += gp_Vec(Normal.X(),Normal.Y(),Normal.Z());
        (*vertexnormals)[N3-1] += gp_Vec(Normal.X(),Normal.Y(),Normal.Z());

        (*vertices)[N1-1].SetX((float)(V1.X()));
        (*vertices)[N1-1].SetY((float)(V1.Y()));
        (*vertices)[N1-1].SetZ((float)(V1.Z()));
        (*vertices)[N2-1].SetX((float)(V2.X()));
        (*vertices)[N2-1].SetY((float)(V2.Y()));
        (*vertices)[N2-1].SetZ((float)(V2.Z()));
        (*vertices)[N3-1].SetX((float)(V3.X()));
        (*vertices)[N3-1].SetY((float)(V3.Y()));
        (*vertices)[N3-1].SetZ((float)(V3.Z()));

        int j = i - 1;
        N1--;
        N2--;
        N3--;
        (*cons)[3*j] = N1;
        (*cons)[3*j+1] = N2;
        (*cons)[3*j+2] = N3;
    }

    // normalize all vertex normals
    for (i=0; i < nbNodesInFace; i++) {

        gp_Dir clNormal;

        try {
            Handle_Geom_Surface Surface = BRep_Tool::Surface(aFace);

            gp_Pnt vertex((*vertices)[i].XYZ());
//     gp_Pnt vertex((*vertices)[i][0], (*vertices)[i][1], (*vertices)[i][2]);
            GeomAPI_ProjectPointOnSurf ProPntSrf(vertex, Surface);
            Standard_Real fU, fV;
            ProPntSrf.Parameters(1, fU, fV);

            GeomLProp_SLProps clPropOfFace(Surface, fU, fV, 2, gp::Resolution());

            clNormal = clPropOfFace.Normal();
            gp_Vec temp = clNormal;
            //Base::Console().Log("unterschied:%.2f",temp.dot((*vertexnormals)[i]));
            if ( temp * (*vertexnormals)[i] < 0 )
                temp = -temp;
            (*vertexnormals)[i] = temp;

        }
        catch (...) {
        }

        (*vertexnormals)[i].Normalize();
    }
}