/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpenGLHardwareSupport.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkOpenGLHardwareSupport - OpenGL rendering window
// .SECTION Description
// vtkOpenGLHardwareSupport is an implementation of methods used
// to query OpenGL and the hardware of what kind of graphics support
// is available.

#ifndef __vtkOpenGLHardwareSupport_h
#define __vtkOpenGLHardwareSupport_h

#include "vtkHardwareSupport.h"

class VTK_RENDERING_EXPORT vtkOpenGLHardwareSupport : public vtkHardwareSupport
{
public:
  vtkTypeMacro(vtkOpenGLHardwareSupport,vtkHardwareSupport);
  static vtkOpenGLHardwareSupport *New();
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkOpenGLHardwareSupport();
  virtual ~vtkOpenGLHardwareSupport() {}

  virtual int MaxTextureUnits() const;
  virtual int MaxCombinedTextureImageUnits() const;
  
private:
  vtkOpenGLHardwareSupport(const vtkOpenGLHardwareSupport&);  // Not implemented.
  void operator=(const vtkOpenGLHardwareSupport&);  // Not implemented.
};
#endif
