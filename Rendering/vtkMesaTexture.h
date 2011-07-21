/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMesaTexture.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMesaTexture - Mesa texture map
// .SECTION Description
// vtkMesaTexture is a concrete implementation of the abstract class 
// vtkTexture. vtkMesaTexture interfaces to the Mesa rendering library.

#ifndef __vtkMesaTexture_h
#define __vtkMesaTexture_h

#include "vtkTexture.h"
//BTX
#include "vtkWeakPointer.h" // needed for vtkWeakPointer.
//ETX

class vtkWindow;
class vtkMesaRenderer;
class vtkRenderWindow;
class vtkPixelBufferObject;

class VTK_RENDERING_EXPORT vtkMesaTexture : public vtkTexture
{
public:
  static vtkMesaTexture *New();
  vtkTypeMacro(vtkMesaTexture,vtkTexture);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Implement base class method.
  void Load(vtkRenderer *ren);
  
  // Descsription:
  // Clean up after the rendering is complete.
  virtual void PostRender(vtkRenderer *ren);

  // Description:
  // Release any graphics resources that are being consumed by this texture.
  // The parameter window could be used to determine which graphic
  // resources to release. Using the same texture object in multiple
  // render windows is NOT currently supported. 
  void ReleaseGraphicsResources(vtkWindow *);

  
  // Description:
  // Get the openGL texture name to which this texture is bound.
  // This is available only if GL version >= 1.1
  vtkGetMacro(Index, long);
//BTX
protected:
  vtkMesaTexture();
  ~vtkMesaTexture();

  unsigned char *ResampleToPowerOfTwo(int &xsize, int &ysize, 
                                      unsigned char *dptr, int bpp);

  vtkTimeStamp   LoadTime;
  unsigned int Index; // actually GLuint
  vtkWeakPointer<vtkRenderWindow> RenderWindow;   // RenderWindow used for previous render
  bool CheckedHardwareSupport;
  bool SupportsNonPowerOfTwoTextures;
  bool SupportsPBO;
  vtkPixelBufferObject *PBO;

private:
  vtkMesaTexture(const vtkMesaTexture&);  // Not implemented.
  void operator=(const vtkMesaTexture&);  // Not implemented.

  // Description:
  // Handle loading in extension support
  virtual void Initialize(vtkRenderer * ren);

//ETX
};

#endif
