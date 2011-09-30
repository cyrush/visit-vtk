// -*- c++ -*-

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMesaExtensionManager.h

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

// .NAME vtkMesaExtensionManager - Interface class for querying and using Mesa extensions.
//
// .SECTION Description
//
// vtkMesaExtensionManager acts as an interface to Mesa extensions.  It
// provides methods to query Mesa extensions on the current or a given
// render window and to load extension function pointers.  Currently does
// not support GLU extensions since the GLU library is not linked to VTK.
//
// Before using vtkMesaExtensionManager, an Mesa context must be created.
// This is generally done with a vtkRenderWindow.  Note that simply creating
// the vtkRenderWindow is not sufficient.  Usually you have to call Render
// before the actual Mesa context is created.  You can specify the
// RenderWindow with the SetRenderWindow method.
// \code
// vtkMesaExtensionManager *extensions = vtkMesaExtensionManager::New();
// extensions->SetRenderWindow(renwin);
// \endcode
// If no vtkRenderWindow is specified, the current Mesa context (if any)
// is used.
//
// Generally speaking, when using Mesa extensions, you will need an
// vtkMesaExtensionManager and the prototypes defined in vtkgl.h.
// \code
// #include "vtkMesaExtensionManager.h"
// #include "vtkgl.h"
// \endcode
// The vtkgl.h include file contains all the constants and function
// pointers required for using Mesa extensions in a portable and
// namespace safe way.  vtkgl.h is built from parsed glext.h, glxext.h, and
// wglext.h files.  Snapshots of these files are distributed with VTK,
// but you can also set CMake options to use other files.
//
// To use an Mesa extension, you first need to make an instance of
// vtkMesaExtensionManager and give it a vtkRenderWindow.  You can then
// query the vtkMesaExtensionManager to see if the extension is supported
// with the ExtensionSupported method.  Valid names for extensions are
// given in the Mesa extension registry at
// http://www.opengl.org/registry/ .
// You can also grep vtkgl.h (which will be in the binary build directory
// if VTK is not installed) for appropriate names.  There are also
// special extensions GL_VERSION_X_X (where X_X is replaced with a major
// and minor version, respectively) which contain all the constants and
// functions for Mesa versions for which the gl.h header file is of an
// older version than the driver.
//
// \code
// if (   !extensions->ExtensionSupported("GL_VERSION_1_2")
//     || !extensions->ExtensionSupported("GL_ARB_multitexture") ) {
//   {
//   vtkErrorMacro("Required extensions not supported!");
//   }
// \endcode
//
// Once you have verified that the extensions you want exist, before you
// use them you have to load them with the LoadExtension method.
//
// \code
// extensions->LoadExtension("GL_VERSION_1_2");
// extensions->LoadExtension("GL_ARB_multitexture");
// \endcode
//
// Alternatively, you can use the LoadSupportedExtension method, which checks
// whether the requested extension is supported and, if so, loads it. The
// LoadSupportedExtension method will not raise any errors or warnings if it
// fails, so it is important for callers to pay attention to the return value.
//
// \code
// if (   extensions->LoadSupportedExtension("GL_VERSION_1_2")
//     && extensions->LoadSupportedExtension("GL_ARB_multitexture") ) {
//   {
//   vtkgl::ActiveTexture(vtkgl::TEXTURE0_ARB);
//   }
// else
//   {
//   vtkErrorMacro("Required extensions could not be loaded!");
//   }
// \endcode
//
// Once you have queried and loaded all of the extensions you need, you can
// delete the vtkMesaExtensionManager.  To use a constant of an extension,
// simply replace the "GL_" prefix with "vtkgl::".  Likewise, replace the
// "gl" prefix of functions with "vtkgl::".  In rare cases, an extension will
// add a type. In this case, add vtkgl:: to the type (i.e. vtkgl::GLchar).
//
// \code
// extensions->Delete();
// ...
// vtkgl::ActiveTexture(vtkgl::TEXTURE0_ARB);
// \endcode
//
// For wgl extensions, replace the "WGL_" and "wgl" prefixes with
// "vtkwgl::".  For glX extensions, replace the "GLX_" and "glX" prefixes
// with "vtkglX::".
//

#ifndef __vtkMesaExtensionManager_h
#define __vtkMesaExtensionManager_h

#include <vtkExtensionManager.h>

class vtkRenderWindow;

//BTX
extern "C" {
#ifdef _WIN32
#include <vtkMesa.h>  // Needed for WINAPI
  typedef int (WINAPI *vtkMesaExtensionManagerFunctionPointer)(void);
#else
  typedef void (*vtkMesaExtensionManagerFunctionPointer)(void);
#endif
}
//ETX

class VTK_RENDERING_EXPORT vtkMesaExtensionManager :
  public vtkExtensionManager
{
public:
  vtkTypeMacro(vtkMesaExtensionManager, vtkExtensionManager);
  static vtkMesaExtensionManager *New();
  void PrintSelf(ostream &os, vtkIndent indent);

  // Description:
  // Returns true if the extension is supported, false otherwise.
  virtual int ExtensionSupported(const char *name);

protected:
  vtkMesaExtensionManager();
  virtual ~vtkMesaExtensionManager();

  virtual void ReadOpenGLExtensions();
  
private:
  vtkMesaExtensionManager(const vtkMesaExtensionManager&); // Not implemented
  void operator=(const vtkMesaExtensionManager&); // Not implemented
};
#endif //__vtkMesaExtensionManager
