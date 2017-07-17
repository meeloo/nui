//
//  nuiUniformDesc.cpp
//  nui3
//
//  Created by Sébastien Métrot on 3/4/13.
//  Copyright (c) 2013 libNUI. All rights reserved.
//

#include "nui.h"

//class nuiUniformDesc

nuiUniformDesc::nuiUniformDesc(const nglString& rName, GLenum Type, int count, GLuint Location, size_t offset, nuiShaderProgram* pProgram)
: mName(rName), mType(Type), mCount(count), mLocation(Location), mOffset(offset)
{
}

nuiUniformDesc::~nuiUniformDesc()
{
}


