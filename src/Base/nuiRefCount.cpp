//
//  nuiRefCount.cpp
//  nui3
//
//  Created by Sebastien Metrot on 29/12/14.
//  Copyright (c) 2014 libNUI. All rights reserved.
//

#include "nui.h"

pthread_key_t nuiRefCount::key;

void nuiRefCount::DumpInfos() const
{
#ifdef _NUI_DEBUG_OBJECTS_
  nuiObject::DumpObjectInfos((nuiObject*)(void*)this);
#endif
}