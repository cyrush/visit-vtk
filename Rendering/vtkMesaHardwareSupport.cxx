/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMesaHardwareSupport.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// Make sure this is first, so any includes of gl.h can be stoped if needed
#define VTK_IMPLEMENT_MESA_CXX

#include "GL/gl_mangle.h"
#include "GL/gl.h"

#include <math.h>
#include "vtkToolkits.h"
#include "vtkMesaHardwareSupport.h"

// make sure this file is included before the #define takes place
// so we don't get two vtkMesaHardwareSUpport classes defined.
#include "vtkOpenGLHardwareSupport.h"
#include "vtkMesaHardwareSupport.h"

#define vtkOpenGLHardwareSupport vtkMesaHardwareSupport
#include "vtkOpenGLHardwareSupport.cxx"
#undef vtkOpenGLHardwareSupport
