/*
  NGL - C++ cross-platform framework for OpenGL based applications
  Copyright (C) 2000-2003 NGL Team

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __nglImageCGCodec_h__
#define __nglImageCGCodec_h__

#include "nglImageCodec.h"

//! Functions prototypes
size_t	CGGetBytesCallback(void* pInfo, void* pBuffer, size_t Count);
void		CGReleaseInfoCallback(void* pInfo);
void		CGRewindCallback(void* pInfo);
off_t		CGSkipForwardBytesCallback(void* pInfo, off_t Count);

class nglImageCGCodecInfo : public nglImageCodecInfo
{
public:
  nglImageCGCodecInfo()
  {
    mCanSave = false; ///< FIXME
    mCanLoad = true;

    mName = "CG";

    mExtensions.push_back(nglString(".png"));
    mExtensions.push_back(nglString(".jpeg"));
    mExtensions.push_back(nglString(".jpg"));
    mExtensions.push_back(nglString(".jpe"));
    mExtensions.push_back(nglString(".jif"));
    
    mInfo = "CG image codec, using Core Graphics Framework from Apple";
  }

  virtual ~nglImageCGCodecInfo()
  {
  }

  virtual nglImageCodec* CreateInstance();
};

#endif // __nglImageCGCodec_h__
