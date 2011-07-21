/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMesaLightingPainter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMesaLightingPainter - painter that manages lighting.
// .SECTION Description
// This painter manages lighting.
// Ligting is disabled when rendering points/lines and no normals are present
// or rendering Polygons/TStrips and representation is points and no normals 
// are present.

#ifndef __vtkMesaLightingPainter_h
#define __vtkMesaLightingPainter_h

#include "vtkLightingPainter.h"

class vtkWindow;

class VTK_RENDERING_EXPORT vtkMesaLightingPainter : public vtkLightingPainter
{
public:
  static vtkMesaLightingPainter* New();
  vtkTypeMacro(vtkMesaLightingPainter, vtkLightingPainter);
  void PrintSelf(ostream& os ,vtkIndent indent);

  // Description:
  // This painter overrides GetTimeToDraw() to never pass the request to the
  // delegate. This is done since this class may propagate a single render
  // request multiple times to the delegate. In that case the time accumulation
  // responsibility is borne by the painter causing the multiple rendering
  // requests i.e. this painter itself.
  virtual double GetTimeToDraw()
    {
    return this->TimeToDraw;
    }

protected:
  vtkMesaLightingPainter();
  ~vtkMesaLightingPainter();

  // Description:
  // Setups lighting state before calling render on delegate 
  // painter.
  virtual void RenderInternal(vtkRenderer *renderer,
                              vtkActor *actor, 
                              unsigned long typeflags,
                              bool forceCompileOnly);

private:
  vtkMesaLightingPainter(const vtkMesaLightingPainter&); // Not implemented.
  void operator=(const vtkMesaLightingPainter&); // Not implemented.
};

#endif

