/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2005     *
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
# include <TopoDS_Face.hxx>
# include <sstream>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>


#include "LuxRender.h"
#include "LuxRenderProcess.h"

using Base::Console;

using namespace Raytracing;
using namespace std;


// #SAMPLE FOR LUX RENDERER!
// #Global Information
// LookAt 0 10 100 0 -1 0 0 1 0
// Camera "perspective" "float fov" [30]
// 
// Film "fleximage"
// "integer xresolution" [200] "integer yresolution" [200]
// 
// PixelFilter "mitchell" "float xwidth" [2] "float ywidth" [2]
// 
// Sampler "lowdiscrepancy" "string pixelsampler" ["lowdiscrepancy"]
// 
// #Scene Specific Information
// WorldBegin
// 
// AttributeBegin
//         CoordSysTransform "camera"
//         LightSource "distant"
//                 "point from" [0 0 0] "point to" [0 0 1]
//                 "color L" [3 3 3]
// AttributeEnd
// 
// AttributeBegin
//         Rotate 135 1 0 0
// 
//         Texture "checks" "color" "checkerboard"
//                 "float uscale" [4] "float vscale" [4]
//                 "color tex1" [1 0 0] "color tex2" [0 0 1]
// 
//         Material "matte"
//                 "texture Kd" "checks"
//         Shape "disk" "float radius" [20] "float height" [-1]
// AttributeEnd
// 
// WorldEnd

LuxRender::LuxRender(void){}
LuxRender::~LuxRender(void){}

void LuxRender::generateScene()
{
    QTextStream out;
    if(!getOutputStream(out))
      return;

    out << "#Global Information:" << endl
        << genCamera(camera).c_str()
        << genRenderProps().c_str()
        << "#Scene Specific Information:" << endl
        << "WorldBegin" << endl;

    for (std::vector<RenderLight *>::iterator it = lights.begin(); it != lights.end(); ++it) {
        out << genLight(*it).c_str();
    }
    for (std::vector<RenderPart *>::iterator it = parts.begin(); it != parts.end(); ++it) {
        RenderPart *part = *it;
        out << genObject(part->getName(), part->getShape(), part->getMeshDeviation()).c_str();
    }

    out << "\nWorldEnd" << endl;
}


// Renderer "sampler"
// 
// Sampler "metropolis"
//         "float largemutationprob" [0.400000005960464]
//         "bool usevariance" ["false"]
// 
// Accelerator "qbvh"
// 
// SurfaceIntegrator "bidirectional"
//         "integer eyedepth" [48]
//         "integer lightdepth" [48]
// 
// VolumeIntegrator "multi"
//         "float stepsize" [1.000000000000000]
// 
// PixelFilter "mitchell"
//         "bool supersample" ["true"]
        
std::string LuxRender::genRenderProps()
{
    std::stringstream out;
    out << "# Scene render Properties:" << endl
        << "Renderer \"sampler\"" << endl
        << "\nSampler \"metropolis\"" << endl
        << "\t \"float largemutationprob\" [0.400000005960464]" << endl
        << "\"bool usevariance\" [\"false\"]" << endl
        << "\nAccelerator \"qbvh\"" << endl
        << "SurfaceIntegrator \"bidirectional\""
        << "\t\"integer eyedepth\" [48]" << endl
        << "\t\"integer lightdepth\" [48]" << endl
        << "VolumeIntegrator \"multi\"" << endl
        << "\t\"float stepsize\" [1.000000000000000]"
        << "\nPixelFilter \"mitchell\"" << endl
        << "\t\"bool supersample\" [\"true\"]" << endl
        << "\nFilm \"fleximage\" \"integer xresolution\" [" << this->xRes << "] \"integer yresolution\" [" << yRes << "]" << endl
        << "PixelFilter \"mitchell\" \"float xwidth\" [2] \"float ywidth\" [2]" << endl;

    return out.str();
}

std::string LuxRender::genLight(RenderLight *light) const
{
    std::stringstream out;
    out <<  "\nAttributeBegin " << endl
        <<  "\tCoordSysTransform \"camera\" " << endl
        <<  "\tLightSource \"distant\"" << endl
        <<  "\t\t\"point from\" [0 0 0] \"point to\" [0 0 1]" << endl
        <<  "\t\t\"color L\" [3 3 3]" << endl
        <<  "AttributeEnd" << endl;

    return out.str();
}
std::string LuxRender::genCamera(RenderCamera *camera) const
{
    if(!camera)
        return std::string();

    std::string camType;
    // Switch the camera type
    switch(camera->Type) {
      case RenderCamera::ORTHOGRAPHIC:
        camType = "orthographic"; break;
      default:
      case RenderCamera::PERSPECTIVE:
        camType = "perspective"; break;
    }

    std::stringstream out;
    out << "# Camera Declaration and View Direction" << endl
        << "Camera \"" << camType << "\" \"float fov\" [50]" << endl;
    if(camera->Type == RenderCamera::PERSPECTIVE && camera->autofocus)
        out << "\t\"bool autofocus\" [\"true\"]" << endl;
    out << "\t\"float focaldistance\" [" << camera->focaldistance << "]" << endl;

    out << "\nLookAt " << camera->CamPos.x << " " << camera->CamPos.y << " " << camera->CamPos.z << " "
                     << camera->LookAt.x << " " << camera->LookAt.y << " " << camera->LookAt.z << " "
                     << camera->Up.x     << " " << camera->Up.y     << " " << camera->Up.z     << "\n " << endl;

    return out.str();
}

std::string LuxRender::genObject(const char *PartName,const TopoDS_Shape& Shape, float meshDeviation)
{
    //fMeshDeviation is a class variable
    Base::Console().Log("Meshing with Deviation: %f\n",meshDeviation);

    TopExp_Explorer ex;
    BRepMesh_IncrementalMesh MESH(Shape,meshDeviation);

    // counting faces and start sequencer
    int l = 1;
    std::stringstream out;
    out << "\nObjectBegin \"" << PartName << "\"" << endl;

    for (ex.Init(Shape, TopAbs_FACE); ex.More(); ex.Next(),l++) { 
        const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
        out << genFace(aFace, l);
    }
    out << "ObjectEnd"  << endl
        << "ObjectInstance \"" << PartName << "\"" << endl;
    return out.str();
}

std::string LuxRender::genFace(const TopoDS_Face& aFace, int index )
{
    // this block mesh the face and transfers it in a C array of vertices and face indexes
    Standard_Integer nbNodesInFace,nbTriInFace;
    gp_Vec* verts=0;
    gp_Vec* vertNorms=0;
    long* cons=0;

    transferToArray(aFace,&verts,&vertNorms,&cons,nbNodesInFace,nbTriInFace);
    if (!verts)
      return std::string();

    std::stringstream out;

    out << "#Face Number: " << index << endl
        << "AttributeBegin" << endl
        << "\tShape \"trianglemesh\" \"string name\" \"Face " << index << "\"" << endl;

    // Write the Vertex Points in order
    out << "\n\t\"point P\"" << endl
        << "\t[" << endl;
    for (int i=0; i < nbNodesInFace; i++) {
        out << "\t\t" <<  verts[i].X() << " " << verts[i].Y() << " " << verts[i].Z() << " " << endl;
    }
    out << "\t]" << endl; // End Property

    // Write the Normals in order
    out << "\n\t\"normal N\"" << endl
        << "\t[" << endl;
    for (int j=0; j < nbNodesInFace; j++) {
        out << "\t\t" <<  vertNorms[j].X() << " " << vertNorms[j].Y() << " " << vertNorms[j].Z() << " " << endl;
    }
    out << "\t]" << endl; // End Property

    // Write the Face Indices in order
    out << "\n\t\"integer indices\"" << endl
        << "\t[" << endl;
   for (int k=0; k < nbTriInFace; k++) {
        out << "\t\t" <<  cons[3*k] << " " << cons[3*k + 1] << " " << cons[3*k + 2] << endl;
    }
    out << "\t]" << endl; // End Property

    // End of Face
    out << "AttributeEnd\n" << endl;

    delete [] vertNorms;
    delete [] verts;
    delete [] cons;

    return out.str();
}

void LuxRender::render()
{
    // Check if there are any processes running;
    if(process && process->isActive())
        return;

    // Create a new Render Process
    LuxRenderProcess * process = new LuxRenderProcess();

    process->initialiseSettings();
    this->attachRenderProcess(process);
    Renderer::render();
}