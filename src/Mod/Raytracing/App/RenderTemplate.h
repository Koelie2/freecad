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


#ifndef _RenderTemplate_h_
#define _RenderTemplate_h_

#include <QString>
namespace Raytracing
{

class RenderTemplate
{
public:
    enum TemplateSource {
    BUILTIN,
    EXTERNAL};

  RenderTemplate(QString id, QString label, QString filename, QString description, QString provider, TemplateSource tempType)
             : id(id),
               label(label),
               filename(filename),
               description(description),
               provider(provider),
               source(tempType)
               {}
  ~RenderTemplate(){}

  QString getId() const { return id;}
  QString getFilename() const { return filename;}
  QString getLabel() const { return label;}
  QString getDescription() const { return description;}
  QString getProvider() const { return provider;}
  TemplateSource getSource() const { return source;}

private:
  QString id;
  QString filename;
  QString label;
  QString description;
  QString provider;
  TemplateSource source;
};

}

#endif //_RenderTemplate_h_