/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHardwareSupport.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkHardwareSupport - OpenGL rendering window
// .SECTION Description
// vtkHardwareSupport is an implementation of methods used
// to query OpenGL and the hardware of what kind of graphics support
// is available.

#ifndef __vtkHardwareSupport_h
#define __vtkHardwareSupport_h

#include "vtkObject.h"

class vtkExtensionManager;

class VTK_RENDERING_EXPORT vtkHardwareSupport : public vtkObject
{
public:
  vtkTypeMacro(vtkHardwareSupport,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Return the number of fixed-function texture units.
  virtual int GetNumberOfFixedTextureUnits();

  // Description:
  // Return the total number of texture image units accessible by a shader
  // program.
  virtual int GetNumberOfTextureUnits();
  
  // Description:
  // Test if MultiTexturing is supported.
  virtual bool GetSupportsMultiTexturing();

  // Description:
  // Set/Get a reference to a vtkRenderWindow which is Required
  // for most methods of this class to work.
  vtkGetObjectMacro(ExtensionManager, vtkExtensionManager);
  void SetExtensionManager(vtkExtensionManager* extensionManager);
  
protected:
  vtkHardwareSupport();
  virtual ~vtkHardwareSupport();
  
  bool ExtensionManagerSet();

  virtual int MaxTextureUnits() const = 0;
  virtual int MaxCombinedTextureImageUnits() const = 0;

  vtkExtensionManager* ExtensionManager;

private:
  vtkHardwareSupport(const vtkHardwareSupport&);  // Not implemented.
  void operator=(const vtkHardwareSupport&);  // Not implemented.
};
#endif
