/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpenGLScalarsToColorsPainter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkOpenGLScalarsToColorsPainter.h"

#include "vtkActor.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkMapper.h" //for VTK_MATERIALMODE_*
#include "vtkObjectFactory.h"
#include "vtkOpenGLRenderer.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLTexture.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"


#ifndef VTK_IMPLEMENT_MESA_CXX
#  include "vtkOpenGL.h"
#endif

#include "vtkgl.h" // vtkgl namespace

#ifndef VTK_IMPLEMENT_MESA_CXX
vtkStandardNewMacro(vtkOpenGLScalarsToColorsPainter);
#endif
//-----------------------------------------------------------------------------
vtkOpenGLScalarsToColorsPainter::vtkOpenGLScalarsToColorsPainter()
{
  this->InternalColorTexture = 0;
  this->AlphaBitPlanes = -1;
}

//-----------------------------------------------------------------------------
vtkOpenGLScalarsToColorsPainter::~vtkOpenGLScalarsToColorsPainter()
{
  if (this->InternalColorTexture)
    {
    this->InternalColorTexture->Delete();
    this->InternalColorTexture = 0;
    }
}

//-----------------------------------------------------------------------------
void vtkOpenGLScalarsToColorsPainter::ReleaseGraphicsResources(vtkWindow* win)
{
  if (this->InternalColorTexture)
    {
    this->InternalColorTexture->ReleaseGraphicsResources(win);
    }
  this->Superclass::ReleaseGraphicsResources(win);
}

//-----------------------------------------------------------------------------
int vtkOpenGLScalarsToColorsPainter::GetPremultiplyColorsWithAlpha(
  vtkActor* actor)
{
  // Use the AlphaBitPlanes member, which should already be initialized. If
  // not, initialize it.
  GLint alphaBits;
  if (this->AlphaBitPlanes < 0)
    {
    glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
    this->AlphaBitPlanes = (int)alphaBits;
    }
  alphaBits = (GLint)this->AlphaBitPlanes;

  // Dealing with having a correct alpha (none square) in the framebuffer
  // is only required if there is an alpha component in the framebuffer
  // (doh...) and if we cannot deal directly with BlendFuncSeparate.
  
  return vtkgl::BlendFuncSeparate==0 && alphaBits>0 &&
    this->Superclass::GetPremultiplyColorsWithAlpha(actor);
}

//-----------------------------------------------------------------------------
vtkIdType vtkOpenGLScalarsToColorsPainter::GetTextureSizeLimit()
{
  GLint textureSize = 0;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &textureSize);
  return static_cast<vtkIdType>(textureSize);
}

//-----------------------------------------------------------------------------
void vtkOpenGLScalarsToColorsPainter::RenderInternal(vtkRenderer *renderer, 
                                                     vtkActor *actor,
                                                     unsigned long typeflags,
                                                     bool forceCompileOnly)
{
  vtkProperty* prop = actor->GetProperty();

  // If we have not yet set the alpha bit planes, do it based on the 
  // render window so we're not querying GL in the middle of render.
  if (this->AlphaBitPlanes < 0)
    {
    vtkOpenGLRenderer *oRenderer=static_cast<vtkOpenGLRenderer *>(renderer);
    if (oRenderer != NULL)
      {
      vtkOpenGLRenderWindow* context = vtkOpenGLRenderWindow::SafeDownCast(
        oRenderer->GetRenderWindow());
      this->AlphaBitPlanes = context->GetAlphaBitPlanes();
      }
    }

  // If we are coloring by texture, then load the texture map.
  if (this->ColorTextureMap)
    {
    if (this->InternalColorTexture == 0)
      {
      this->InternalColorTexture = vtkOpenGLTexture::New();
      this->InternalColorTexture->RepeatOff();
      }
    this->InternalColorTexture->SetInput(this->ColorTextureMap);
    // Keep color from interacting with texture.
    float info[4];
    info[0] = info[1] = info[2] = info[3] = 1.0;
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, info );
    
    this->LastWindow = renderer->GetRenderWindow();
    }
  else if (this->LastWindow)
    {
    // release the texture.
    this->ReleaseGraphicsResources(this->LastWindow);
    this->LastWindow = 0;
    }


  // if we are doing vertex colors then set lmcolor to adjust 
  // the current materials ambient and diffuse values using   
  // vertex color commands otherwise tell it not to.          
  glDisable( GL_COLOR_MATERIAL );
  if (this->UsingScalarColoring)
    {
    GLenum lmcolorMode;
    if (this->ScalarMaterialMode == VTK_MATERIALMODE_DEFAULT)
      {
      if (prop->GetAmbient() > prop->GetDiffuse())
        {
        lmcolorMode = GL_AMBIENT;
        }
      else
        {
        lmcolorMode = GL_DIFFUSE;
        }
      }
    else if (this->ScalarMaterialMode == VTK_MATERIALMODE_AMBIENT_AND_DIFFUSE)
      {
      lmcolorMode = GL_AMBIENT_AND_DIFFUSE;
      }
    else if (this->ScalarMaterialMode == VTK_MATERIALMODE_AMBIENT)
      {
      lmcolorMode = GL_AMBIENT;
      }
    else // if (this->ScalarMaterialMode == VTK_MATERIALMODE_DIFFUSE)
      {
      lmcolorMode = GL_DIFFUSE;
      } 

    if (this->ColorTextureMap)
      {
      this->InternalColorTexture->Load(renderer);
      }
    else
      {
      glColorMaterial( GL_FRONT_AND_BACK, lmcolorMode);
      glEnable( GL_COLOR_MATERIAL );
      }
    }

  int pre_multiplied_by_alpha =  this->GetPremultiplyColorsWithAlpha(actor);
  
  if(pre_multiplied_by_alpha || this->InterpolateScalarsBeforeMapping)
  {
    // save the blend function.
    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT);
  }
  
  // We colors were premultiplied by alpha then we change the blending
  // function to one that will compute correct blended destination alpha
  // value, otherwise we stick with the default.
  if (pre_multiplied_by_alpha)
    {   
    // the following function is not correct with textures because there are
    // not premultiplied by alpha.
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }

  if (this->InterpolateScalarsBeforeMapping)
    {
    // Turn on color sum and separate specular color so specular works 
    // with texturing.
    glEnable(vtkgl::COLOR_SUM);
    glLightModeli(vtkgl::LIGHT_MODEL_COLOR_CONTROL, vtkgl::SEPARATE_SPECULAR_COLOR);
    }

  this->Superclass::RenderInternal(renderer, actor, typeflags,forceCompileOnly);

  if (pre_multiplied_by_alpha || this->InterpolateScalarsBeforeMapping)
    {
    // restore the blend function & lights
    glPopAttrib();
    }
}

//-----------------------------------------------------------------------------
void vtkOpenGLScalarsToColorsPainter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "AlphaBitPlanes: " << this->AlphaBitPlanes << endl;
}
