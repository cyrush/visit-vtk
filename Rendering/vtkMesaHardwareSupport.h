/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMesaHardwareSupport.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMesaHardwareSupport - Mesa rendering window
// .SECTION Description
// vtkMesaHardwareSupport is an implementation of methods used
// to query OpenGL and the hardware of what kind of graphics support
// is available.
#ifndef __vtkMesaHardwareSupport_h
#define __vtkMesaHardwareSupport_h

#include "vtkHardwareSupport.h"

class VTK_RENDERING_EXPORT vtkMesaHardwareSupport : public vtkHardwareSupport
{
public:
  vtkTypeMacro(vtkMesaHardwareSupport,vtkHardwareSupport);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkMesaHardwareSupport *New();

protected:
  vtkMesaHardwareSupport();
  virtual ~vtkMesaHardwareSupport() {}

  virtual int MaxTextureUnits() const;
  virtual int MaxCombinedTextureImageUnits() const;
  
private:
  vtkMesaHardwareSupport(const vtkMesaHardwareSupport&);  // Not implemented.
  void operator=(const vtkMesaHardwareSupport&);  // Not implemented.
};
#endif
