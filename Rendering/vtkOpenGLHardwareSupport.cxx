/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpenGLHardwareSupport.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkOpenGLHardwareSupport.h"

#include "vtkObjectFactory.h"
#include "vtkOpenGL.h"
#include "vtkgl.h"

vtkStandardNewMacro(vtkOpenGLHardwareSupport);

// ----------------------------------------------------------------------------
vtkOpenGLHardwareSupport::vtkOpenGLHardwareSupport()
{
}

// ----------------------------------------------------------------------------
int vtkOpenGLHardwareSupport::MaxTextureUnits() const
{
  GLint numSupportedTextures = 1;
  glGetIntegerv(vtkgl::MAX_TEXTURE_UNITS, &numSupportedTextures);

  return numSupportedTextures;
}

// ----------------------------------------------------------------------------
int vtkOpenGLHardwareSupport::MaxCombinedTextureImageUnits() const
{
  GLint value;
  glGetIntegerv(vtkgl::MAX_COMBINED_TEXTURE_IMAGE_UNITS, &value);
  return static_cast<int>(value);
}

// ----------------------------------------------------------------------------
void vtkOpenGLHardwareSupport::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
