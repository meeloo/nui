//
//  nuiUniformDesc.h
//  nui3
//
//  Created by Sébastien Métrot on 3/4/13.
//  Copyright (c) 2013 libNUI. All rights reserved.
//

#pragma once

class nuiShaderProgram;

class nuiUniformDesc
{
public:
  nuiUniformDesc(const nglString& rName, GLenum Type, int count, GLuint Location, size_t offset, nuiShaderProgram* pProgram = NULL);

  ~nuiUniformDesc();

  nglString mName;
  GLenum mType;
  GLenum mCount;
  GLuint mLocation;
  size_t mOffset;

private:
  nuiUniformDesc();

};

