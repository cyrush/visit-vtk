/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMesaExtensionManager.cxx

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
#include "vtkMesaExtensionManager.h"

#ifdef VTK_USE_X
#include "vtkDynamicLoader.h"
#include <vtkstd/string>
#include <vtkstd/list>
#endif

// make sure this file is included before the #define takes place
// so we don't get two vtkMesaTexture classes defined.
#include "vtkOpenGLExtensionManager.h"
#include "vtkMesaExtensionManager.h"
//BTX
  // Description:
  // Returns a function pointer to the OpenGL extension function with the
  // given name.  Returns NULL if the function could not be retrieved.
vtkExtensionManagerFunctionPointer
vtkMesaExtensionManager::GetProcAddress(const char *fname)
{
    vtkExtensionManagerFunctionPointer retval = 0;

    if(strncmp(fname, "gl", 2) == 0)
    {
        // Mangle the function name.
        char *mfname = new char[1 + strlen(fname) + 1];
        snprintf(mfname, 1+strlen(fname)+1, "m%s", fname);
#ifdef VTK_USE_X
        /* We may yet need to do this on other platforms too.
         *
         * This change prevents the base class' GetProcAddress from using
         * glX functions to look up the "mgl" function name.
         */
        vtkLibHandle lh = vtkDynamicLoader::OpenLibrary(NULL);
        retval = (vtkExtensionManagerFunctionPointer)vtkDynamicLoader::GetSymbolAddress(lh, mfname);
        vtkDynamicLoader::CloseLibrary(lh);
#else
        retval = vtkExtensionManager::GetProcAddress(mfname);
#endif
        delete [] mfname;
    }
    
    if(retval == 0)
        retval = vtkExtensionManager::GetProcAddress(fname);

    return retval;
}

#define vtkOpenGLExtensionManager vtkMesaExtensionManager
#include "vtkOpenGLExtensionManager.cxx"
#undef vtkOpenGLExtensionManager
