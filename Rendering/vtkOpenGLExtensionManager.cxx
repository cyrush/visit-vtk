/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpenGLExtensionManager.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

  Copyright 2003 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
  license for use of this work by or on behalf of the
  U.S. Government. Redistribution and use in source and binary forms, with
  or without modification, are permitted provided that this Notice and any
  statement of authorship are reproduced on all copies.

=========================================================================*/

#include "vtkOpenGLExtensionManager.h"
#include "vtkOpenGLExtensionManagerConfigure.h"
#include "vtkgl.h"

#include "vtkRenderWindow.h"
#include "vtkObjectFactory.h"

#include <string.h>

#include <vtkstd/string>

#include <assert.h>

#ifdef VTK_DEFINE_GLX_GET_PROC_ADDRESS_PROTOTYPE
extern "C" vtkglX::__GLXextFuncPtr glXGetProcAddressARB(const GLubyte *);
#endif //VTK_DEFINE_GLX_GET_PROC_ADDRESS_PROTOTYPE

#ifdef VTK_USE_VTK_DYNAMIC_LOADER
#include "vtkDynamicLoader.h"
#include <vtkstd/string>
#include <vtkstd/list>
#endif

#ifdef VTK_USE_APPLE_LOADER
#include <AvailabilityMacros.h>
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3
#include <dlfcn.h>
#else
#include <mach-o/dyld.h>
#endif
#endif //VTK_USE_APPLE_LOADER

// GLU is currently not linked in VTK.  We do not support it here.
#define GLU_SUPPORTED   0

vtkStandardNewMacro(vtkOpenGLExtensionManager);

vtkOpenGLExtensionManager::vtkOpenGLExtensionManager()
{
  this->OwnRenderWindow = 0;
  this->RenderWindow = NULL;
  this->ExtensionsString = NULL;

  this->Modified();
}

vtkOpenGLExtensionManager::~vtkOpenGLExtensionManager()
{
  this->SetRenderWindow(NULL);
}

void vtkOpenGLExtensionManager::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "RenderWindow: (" << this->RenderWindow << ")" << endl;
  os << indent << "BuildTime: " << this->BuildTime << endl;
  os << indent << "ExtensionsString: "
     << (this->ExtensionsString ? this->ExtensionsString : "(NULL)") << endl;
}

int vtkOpenGLExtensionManager::ExtensionSupported(const char *name)
{
  this->Update();

  const char *p = this->ExtensionsString;
  size_t NameLen = strlen(name);
  int result = 0;

  for(;;)
    {
    size_t n;
    while (*p == ' ') p++;
    if (*p == '\0')
      {
      result = 0;
      break;
      }
    n = strcspn(p, " ");
    if ((NameLen == n) && (strncmp(name, p, n) == 0))
      {
      result = 1;
      break;
      }
    p += n;
    }
  
  // Woraround for a nVidia bug in indirect/remote rendering mode (ssh -X)
  // The version returns is not the one actually supported.
  // For example, the version returns is greater or equal to 2.1
  // but where PBO (which are core in 2.1) are not actually supported.
  // In this case, force the version to be 1.1 (minimal). Anything above
  // will be requested only through extensions.
  // See ParaView bug 
  if(result && !this->RenderWindow->IsDirect())
    {
    if (result && strncmp(name, "GL_VERSION_",11) == 0)
      {
      // whatever is the OpenGL version, return false.
      // (nobody asks for GL_VERSION_1_1)
      result=0;
      }
    }
  
  
  // Workaround for a bug on Mac PowerPC G5 with nVidia GeForce FX 5200
  // Mac OS 10.3.9 and driver 1.5 NVIDIA-1.3.42. It reports it supports
  // OpenGL>=1.4 but querying for glPointParameteri and glPointParameteriv
  // return null pointers. So it does not actually supports fully OpenGL 1.4.
  // It will make this method return false with "GL_VERSION_1_4" and true
  // with "GL_VERSION_1_5".
  if (result && strcmp(name, "GL_VERSION_1_4") == 0)
    {
    result=this->GetProcAddress("glPointParameteri")!=0 &&
      this->GetProcAddress("glPointParameteriv")!=0;
    }
  
  const char *gl_renderer=
    reinterpret_cast<const char *>(glGetString(GL_RENDERER));

  // Workaround for a bug on renderer string="Quadro4 900 XGL/AGP/SSE2"
  // version string="1.5.8 NVIDIA 96.43.01" or "1.5.6 NVIDIA 87.56"
  // The driver reports it supports 1.5 but the 1.4 core promoted extension
  // GL_EXT_blend_func_separate is implemented in software (poor performance).
  // All the NV2x chipsets are probably affected. NV2x chipsets are used
  // in GeForce4 and Quadro4.
  // It will make this method return false with "GL_VERSION_1_4" and true
  // with "GL_VERSION_1_5".
  if (result && strcmp(name, "GL_VERSION_1_4") == 0)
    {
    result=strstr(gl_renderer,"Quadro4")==0 &&
      strstr(gl_renderer,"GeForce4")==0;
    }
  
  const char *gl_version=
    reinterpret_cast<const char *>(glGetString(GL_VERSION));
  const char *gl_vendor=
    reinterpret_cast<const char *>(glGetString(GL_VENDOR));

  // Workaround for a bug on renderer string="ATI Radeon X1600 OpenGL Engine"
  // version string="2.0 ATI-1.4.58" vendor string="ATI Technologies Inc."
  // It happens on a Apple iMac Intel Core Duo (early 2006) with Mac OS X
  // 10.4.11 (Tiger) and an ATI Radeon X1600 128MB.
  // The driver reports it supports 2.0 (where GL_ARB_texture_non_power_of_two
  // extension has been promoted to core) and that it supports extension
  // GL_ARB_texture_non_power_of_two. Reality is that non power of two
  // textures just don't work in this OS/driver/card.
  // It will make this method returns false with "GL_VERSION_2_0" and true
  // with "GL_VERSION_2_1".
  // It will make this method returns false with
  // "GL_ARB_texture_non_power_of_two".
  if (result && strcmp(name, "GL_VERSION_2_0") == 0)
    {
    result=!(strcmp(gl_renderer,"ATI Radeon X1600 OpenGL Engine")==0 &&
             strcmp(gl_version,"2.0 ATI-1.4.58")==0 &&
             strcmp(gl_vendor,"ATI Technologies Inc.")==0);
    }
  if (result && strcmp(name, "GL_ARB_texture_non_power_of_two") == 0)
    {
    result=!(strcmp(gl_renderer,"ATI Radeon X1600 OpenGL Engine")==0 &&
             strcmp(gl_version,"2.0 ATI-1.4.58")==0 &&
             strcmp(gl_vendor,"ATI Technologies Inc.")==0);
    }
  return result;
}

void vtkOpenGLExtensionManager::ReadOpenGLExtensions()
{
  vtkDebugMacro("ReadOpenGLExtensions");

#ifdef VTK_NO_EXTENSION_LOADING

  this->ExtensionsString = new char[1];
  this->ExtensionsString[0] = '\0';
  return;

#else //!VTK_NO_EXTENSION_LOADING

  if (this->RenderWindow)
    {
    if (!this->RenderWindow->IsA("vtkOpenGLRenderWindow"))
      {
      // If the render window is not OpenGL, then it obviously has no
      // extensions.
      this->ExtensionsString = new char[1];
      this->ExtensionsString[0] = '\0';
      return;
      }
    this->RenderWindow->MakeCurrent();
    if (!this->RenderWindow->IsCurrent())
      {
      // Really should create a method in the render window to create
      // the graphics context instead of forcing a full render.
      this->RenderWindow->Render();
      }
    if (!this->RenderWindow->IsCurrent())
      {
      // this case happens with a headless Mac: a mac with a graphics card
      // with no monitor attached to it, connected to it with "Screen Sharing"
      // (VNC-like feature added in Mac OS 10.5)
      // see bug 8554.
      this->ExtensionsString = new char[1];
      this->ExtensionsString[0] = '\0';
      return;
      }
    }

  vtkstd::string extensions_string;

  const char *gl_extensions;
  const char *glu_extensions = "";
  const char *win_extensions;

  gl_extensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

  if (gl_extensions == NULL)
    {
    gl_extensions = "";
    }

  if (!this->RenderWindow && (gl_extensions[0] == '\0'))
    {
    vtkDebugMacro("No window active?  Attaching default render window.");
    vtkRenderWindow *renwin = vtkRenderWindow::New();
    renwin->SetSize(1, 1);
    this->SetRenderWindow(renwin);
    renwin->Register(this);
    this->OwnRenderWindow = 1;
    renwin->Delete();
    this->ReadOpenGLExtensions();
    return;
    }

  extensions_string = gl_extensions;

#if GLU_SUPPORTED
  glu_extensions =
    reinterpret_cast<const char *>(gluGetString(GLU_EXTENSIONS));
#endif
  if (glu_extensions != NULL)
    {
    extensions_string += " ";
    extensions_string += glu_extensions;
    }

#if defined(WIN32)
  // Don't use this->LoadExtension or we will go into an infinite loop.
  vtkgl::LoadExtension("WGL_ARB_extensions_string", this);
  if (vtkwgl::GetExtensionsStringARB)
    {
    win_extensions = vtkwgl::GetExtensionsStringARB(wglGetCurrentDC());
    }
  else
    {
    //vtkWarningMacro("Could not query WGL extensions.");
    win_extensions = "";
    }
#elif defined(__APPLE__)
//   vtkWarningMacro("Does APPLE have a windows extension string?");
  win_extensions = "";
#else
  win_extensions = glXGetClientString(glXGetCurrentDisplay(),
                                      GLX_EXTENSIONS);
#endif

  if (win_extensions != NULL)
    {
    extensions_string += " ";
    extensions_string += win_extensions;
    }

  // We build special extension identifiers for OpenGL versions.  Check to
  // see which are supported.
  vtkstd::string version_extensions;
  vtkstd::string::size_type beginpos, endpos;

  const char *version =
    reinterpret_cast<const char *>(glGetString(GL_VERSION));
  int driverMajor = 0;
  int driverMinor = 0;
  sscanf(version, "%d.%d", &driverMajor, &driverMinor);

  version_extensions = vtkgl::GLVersionExtensionsString();
  endpos = 0;
  while (endpos != vtkstd::string::npos)
    {
    beginpos = version_extensions.find_first_not_of(' ', endpos);
    if (beginpos == vtkstd::string::npos) break;
    endpos = version_extensions.find_first_of(' ', beginpos);

    vtkstd::string ve = version_extensions.substr(beginpos, endpos-beginpos);
    int tryMajor, tryMinor;
    sscanf(ve.c_str(), "GL_VERSION_%d_%d", &tryMajor, &tryMinor);
    if (   (driverMajor > tryMajor)
        || ((driverMajor == tryMajor) && (driverMinor >= tryMinor)) )
      {
      // OpenGL version supported.
      extensions_string += " ";
      extensions_string += ve;
      }
    }

#ifdef VTK_USE_X
  Display *display = NULL;
  int closeDisplay = 0;
  if (this->RenderWindow)
    {
    // Try getting the display of the window we are doing the queries on.
    display =
      static_cast<Display *>(this->RenderWindow->GetGenericDisplayId());
    }
  if (!display)
    {
    // Try opening my own display.
    display = XOpenDisplay(NULL);
    closeDisplay = 1;
    }

  if (!display)
    {
    // If we could not find a display, silently fail to query the glX
    // extensions.  It could be that there is no glX (for example if using Mesa
    // offscreen).
    vtkDebugMacro(<< "Could not get a Display to query GLX extensions.");
    }
  else
    {
    glXQueryExtension(display, &driverMajor, &driverMinor);

    version_extensions = vtkgl::GLXVersionExtensionsString();
    endpos = 0;
    while (endpos != vtkstd::string::npos)
      {
      beginpos = version_extensions.find_first_not_of(' ', endpos);
      if (beginpos == vtkstd::string::npos) break;
      endpos = version_extensions.find_first_of(' ', beginpos);
      
      vtkstd::string ve = version_extensions.substr(beginpos, endpos-beginpos);
      int tryMajor, tryMinor;
      sscanf(ve.c_str(), "GLX_VERSION_%d_%d", &tryMajor, &tryMinor);
      if (   (driverMajor > tryMajor)
          || ((driverMajor == tryMajor) && (driverMinor >= tryMinor)) )
        {
        extensions_string += " ";
        extensions_string += ve;
        }
      }

    if (closeDisplay)
      {
      XCloseDisplay(display);
      }
    }
#endif //VTK_USE_X

  // Store extensions string.
  this->ExtensionsString = new char[extensions_string.length()+1];
  strcpy(this->ExtensionsString, extensions_string.c_str());

#endif //!VTK_NO_EXTENSION_LOADING
}
