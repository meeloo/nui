/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nglImageJPEGCodec_h__
#define __nglImageJPEGCodec_h__

//#include "nui.h"
#include "nglImageCodec.h"

class nglImageJPEGCodecInfo : public nglImageCodecInfo
{
public:
  nglImageJPEGCodecInfo()
  {
    mCanSave = true;
    mCanLoad = true;
    mName = "JPEG";
    mExtensions.push_back(nglString(".jpeg"));
    mExtensions.push_back(nglString(".jpg"));
    mExtensions.push_back(nglString(".jpe"));
    mExtensions.push_back(nglString(".jif"));
    mInfo = "JPEG image codec, using libjpeg from www.ijg.org";
  }

  virtual ~nglImageJPEGCodecInfo()
  {
  }

  virtual nglImageCodec* CreateInstance();
};

#endif // __nglImageJPEGCodec_h__
