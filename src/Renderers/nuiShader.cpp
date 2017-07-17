
//
//  nuiShader.cpp
//  nui3
//
//  Created by Sébastien Métrot on 1/10/13.
//  Copyright (c) 2013 libNUI. All rights reserved.
//

#include "nui.h"

static const char* defaultVertexShader =
"attribute vec4 Position;\n\
attribute vec2 TexCoord;\n\
attribute vec4 Color;\n\
uniform vec4 DiffuseColor;\n\
uniform mat4 ModelViewMatrix;\n\
uniform mat4 ProjectionMatrix;\n\
uniform vec4 Offset;\n\
varying vec2 TexCoordVar;\n\
varying vec4 ColorVar;\n\
void main()\n\
{\n\
gl_Position = (ProjectionMatrix * ModelViewMatrix * (Position  + Offset));\n\
TexCoordVar = TexCoord;\n\
ColorVar = DiffuseColor * Color;\n\
}"
;

static const char* defaultFragmentShader =
"uniform sampler2D texture;\n\
varying vec4 ColorVar;\n\
varying vec2 TexCoordVar;\n\
void main()\n\
{\n\
  vec4 _gl_FragColor = ColorVar * texture2D(texture, TexCoordVar);\n\
  gl_FragColor = ColorVar * texture2D(texture, TexCoordVar);\n\
  //float v = texture2D(texture, TexCoordVar)[3];\
  //gl_FragColor = ColorVar * vec4(v, v, v, v);\n\
  //gl_FragColor = ColorVar;\n\
}"
;


struct TypeDesc
{
  GLenum mEnum;
  GLenum mType;
  GLint mSize;
  size_t mComponentSize;
} ShaderParamTypeDesc[] =
{
  {GL_FLOAT	,	GL_FLOAT,	1, sizeof(float)},
  {GL_FLOAT_VEC2	,	GL_FLOAT,	2, sizeof(float)},
  {GL_FLOAT_VEC3	,	GL_FLOAT,	3, sizeof(float)},
  {GL_FLOAT_VEC4	,	GL_FLOAT,	4, sizeof(float)},
  {GL_INT, GL_INT, 1, sizeof(int)},
  {GL_INT_VEC2, GL_INT, 2, sizeof(int)},
  {GL_INT_VEC3, GL_INT, 3, sizeof(int)},
  {GL_INT_VEC4, GL_INT, 4, sizeof(int)},
  {GL_UNSIGNED_INT, GL_INT, 1, sizeof(uint)},
  {GL_BOOL, GL_INT, 1, sizeof(int)},
  {GL_BOOL_VEC2, GL_INT, 2, sizeof(int)},
  {GL_BOOL_VEC3, GL_INT, 3, sizeof(int)},
  {GL_BOOL_VEC4, GL_INT, 4, sizeof(int)},
  {GL_FLOAT_MAT2,	GL_FLOAT,	4, sizeof(float)},
  {GL_FLOAT_MAT3,	GL_FLOAT,	9, sizeof(float)},
  {GL_FLOAT_MAT4,	GL_FLOAT,	16, sizeof(float)},
  {GL_SAMPLER_2D, GL_INT, 1, sizeof(int)},
  {GL_SAMPLER_CUBE, GL_INT, 1, sizeof(int)},
  {GL_ZERO, GL_ZERO, 0, 0}
};

static std::map<GLenum, TypeDesc > gParamTypeMap;

/////////////////////////////////////////////////////
nuiShaderState::nuiShaderState(nuiShaderProgram* pProgram)
: mpProgram(pProgram)
{
  //NGL_OUT("nuiShaderState Ctor %p\n", this);
  //SetTrace(true);

  mData.resize(mpProgram->GetStateDataSize());
}

nuiShaderState::nuiShaderState(const nuiShaderState& rOriginal)
: mpProgram(rOriginal.mpProgram)
{
  //NGL_OUT("nuiShaderState Ctor %p (from %p)\n", this, &rOriginal);
}

nuiShaderState::~nuiShaderState()
{
  //NGL_OUT("nuiShaderState Dtor %p\n", this);
}

nuiShaderProgram* nuiShaderState::GetProgram() const
{
  return mpProgram;
}

void nuiShaderState::Set(const nglString& rName, const float* pV, int32 count)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, pV, count);
}

void nuiShaderState::Set(const nglString& rName, const std::vector<float>& rV)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, &rV[0], rV.size());
}

void nuiShaderState::Set(const nglString& rName, const int32* pV, int32 count)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, pV, count);
}

void nuiShaderState::Set(const nglString& rName, const std::vector<int32>& rV)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, rV);
}

////
void nuiShaderState::Set(const nglString& rName, float v1)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1);
}

void nuiShaderState::Set(const nglString& rName, float v1, float v2)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, v2);
}

void nuiShaderState::Set(const nglString& rName, float v1, float v2, float v3)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, v2, v3);
}

void nuiShaderState::Set(const nglString& rName, float v1, float v2, float v3, float v4)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, v2, v3, v4);
}

void nuiShaderState::Set(const nglString& rName, const nglVector2f& rVec)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, rVec);
}

void nuiShaderState::Set(const nglString& rName, const nglVector3f& rVec)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, rVec);
}

void nuiShaderState::Set(const nglString& rName, const nglVectorf& rVec)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, rVec);
}

void nuiShaderState::Set(const nglString& rName, const nuiColor& rColor)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, rColor);
}

void nuiShaderState::Set(const nglString& rName, int32 v1)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1);
}

void nuiShaderState::Set(const nglString& rName, int32 v1, int32 v2)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, v2);
}

void nuiShaderState::Set(const nglString& rName, int32 v1, int32 v2, int32 v3)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, v2, v3);
}

void nuiShaderState::Set(const nglString& rName, int32 v1, int32 v2, int32 v3, int32 v4)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, v2, v3, v4);
}

void nuiShaderState::Set(const nglString& rName, const nglMatrixf& rMat)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, rMat);
}

static const char* Type2String(GLenum type)
{
  switch (type)
  {
    case GL_FLOAT:        return "float"; break;
    case GL_FLOAT_VEC2:   return "vec2"; break;
    case GL_FLOAT_VEC3:   return "vec3"; break;
    case GL_FLOAT_VEC4:   return "vec4"; break;

    case GL_INT:          return "int"; break;
    case GL_INT_VEC2:     return "ivec2"; break;
    case GL_INT_VEC3:     return "ivec3"; break;
    case GL_INT_VEC4:     return "ivec4"; break;
    case GL_UNSIGNED_INT: return "uint"; break;
    case GL_SAMPLER_2D:   return "sampler2D"; break;
    case GL_SAMPLER_CUBE: return "samplerCube"; break;

    case GL_FLOAT_MAT2:   return "mat2"; break;
    case GL_FLOAT_MAT3:   return "mat3"; break;
    case GL_FLOAT_MAT4:   return "mat4"; break;

    default:
      NGL_ASSERT(0);
  }

  return "???";
}

size_t nuiShaderState::GetStateDataSize() const
{
  return mData.size();
}

const void* nuiShaderState::GetStateData() const
{
  return &mData[0];
}

///
void nuiShaderState::Set(GLint loc, const float* pV, int32 count)
{
  memcpy(&mData[0] + loc, pV, count * sizeof(float));
}

void nuiShaderState::Set(GLint loc, const std::vector<float>& rV)
{
  Set(loc, &rV[0]);
}

void nuiShaderState::Set(GLint loc, const int32* pV, int32 count)
{
  memcpy(&mData[0] + loc, pV, count * sizeof(int32));
}

void nuiShaderState::Set(GLint loc, const std::vector<int32>& rV)
{
  Set(loc, &rV[0], rV.size());
}


void nuiShaderState::Set(GLint loc, float v1)
{
  Set(loc, &v1, 1);
}

void nuiShaderState::Set(GLint loc, float v1, float v2)
{
  float v[] = { v1, v2 };
  Set(loc, v, 2);
}

void nuiShaderState::Set(GLint loc, float v1, float v2, float v3)
{
  float v[] = { v1, v2, v3 };
  Set(loc, v, 3);
}

void nuiShaderState::Set(GLint loc, float v1, float v2, float v3, float v4)
{
  float v[] = { v1, v2, v3, v4 };
  Set(loc, v, 4);
}

void nuiShaderState::Set(GLint loc, const nglVector2f& rVec)
{
  Set(loc, rVec.Elt, 2);
}

void nuiShaderState::Set(GLint loc, const nglVector3f& rVec)
{
  Set(loc, rVec.Elt, 3);
}

void nuiShaderState::Set(GLint loc, const nglVectorf& rVec)
{
  Set(loc, rVec.Elt, 4);
}

void nuiShaderState::Set(GLint loc, const nuiColor& rColor)
{
  nglVectorf v(rColor.Red(), rColor.Green(), rColor.Blue(), rColor.Alpha());
  Set(loc, v);
}

void nuiShaderState::Set(GLint loc, int32 v1)
{
  Set(loc, &v1, 1);
}

void nuiShaderState::Set(GLint loc, int32 v1, int32 v2)
{
  int32 v[] = { v1, v2 };
  Set(loc, v, 2);
}

void nuiShaderState::Set(GLint loc, int32 v1, int32 v2, int32 v3)
{
  int32 v[] = { v1, v2, v3 };
  Set(loc, v, 3);
}

void nuiShaderState::Set(GLint loc, int32 v1, int32 v2, int32 v3, int32 v4)
{
  int32 v[] = { v1, v2, v3, v4 };
  Set(loc, v, 4);
}

void nuiShaderState::Set(GLint loc, const nglMatrixf& rMat)
{
  Set(loc, rMat.Array, 16);
}

void nuiShaderState::SetProjectionMatrix(const nglMatrixf& rMat)
{
  auto ProjectionMatrix = mpProgram->GetProjectionMatrixLocation();
  if (ProjectionMatrix >= 0)
  {
    Set(ProjectionMatrix, rMat);
  }
}

void nuiShaderState::SetModelViewMatrix(const nglMatrixf& rMat)
{
  auto ModelViewMatrix = mpProgram->GetModelViewMatrixLocation();
  if (ModelViewMatrix >= 0)
  {
    Set(ModelViewMatrix, rMat);
  }
}

void nuiShaderState::SetSurfaceMatrix(const nglMatrixf& rMat)
{
  auto SurfaceMatrix = mpProgram->GetSurfaceMatrixLocation();
  if (SurfaceMatrix >= 0)
  {
    Set(SurfaceMatrix, rMat);
  }
}

void nuiShaderState::SetOffset(float OffsetX, float OffsetY)
{
  auto Offset = mpProgram->GetOffsetLocation();
  if (Offset >= 0)
  {
    Set(Offset, OffsetX, OffsetY);
  }
}

void nuiShaderState::SetTextureTranslate(const nglVector2f& rTranslate)
{
  auto TextureTranslate = mpProgram->GetTextureTranslateLocation();
  if (TextureTranslate >= 0)
  {
    Set(TextureTranslate, rTranslate);
  }
}

void nuiShaderState::SetTextureScale(const nglVector2f& rScale)
{
  auto TextureScale = mpProgram->GetTextureScaleLocation();
  if (TextureScale >= 0)
  {
    Set(TextureScale, rScale);
  }
}

void nuiShaderState::SetDifuseColor(const nuiColor& rDifuseColor)
{
  auto DifuseColor = mpProgram->GetDifuseColorLocation();
  if (DifuseColor >= 0)
  {
    Set(DifuseColor, rDifuseColor);
  }
}


bool nuiShaderState::operator == (const nuiShaderState& rState) const
{
  if (mData.size() != rState.mData.size())
    return false;
  return memcmp(&mData[0], &rState.mData[0], mData.size());
}

nuiShaderState& nuiShaderState::operator= (const nuiShaderState& rState)
{
  mpProgram = rState.mpProgram;
  mData = rState.mData;
  return *this;
}

//void nuiShaderState::Set(const nuiShaderState& rState)
//{
//  for (int32 i = 0; i < mUniforms.size(); i++)
//    mUniforms[i].Set(rState.mUniforms[i]);
//}

void nuiShaderState::Dump() const
{
//  NGL_OUT("Dumping state for program %p\n", mpProgram);
//  for (int32 i = 0; i < mUniforms.size(); i++)
//    mUniforms[i].Dump();
}


/////////////////////////////////////////////////////
class nuiShader : public nuiRefCount
{
public:
  nuiShader(nuiShaderKind kind, const nglString& rSource, const nglString& rPrefix);
  nuiShader(nuiShaderKind kind, nglIStream& rSource, const nglString& rPrefix);

  bool Load();
  void Delete();

  GLuint GetShader() const;
  bool IsValid() const;
  const nglString& GetError() const;

protected:
  virtual ~nuiShader();

private:
  nuiShaderKind mKind;
  nglString mSource;
  nglString mError;
  GLuint mShader;
};


nuiShader::nuiShader(nuiShaderKind kind, const nglString& rSource, const nglString& rPrefix)
: mKind(kind), mShader(0), mSource(rPrefix + rSource)
{

}

nuiShader::nuiShader(nuiShaderKind kind, nglIStream& rSource, const nglString& rPrefix)
: mKind(kind), mShader(0)
{
  if (rPrefix.IsEmpty())
    rSource.ReadText(mSource);
  else
  {
    nglString t;
    rSource.ReadText(t);
    mSource = rPrefix;
    mSource += t;
  }
}

nuiShader::~nuiShader()
{
  if (IsValid())
    Delete();
}

bool nuiShader::Load()
{
  NGL_ASSERT(mShader == 0);

  GLint compiled;
  // Create the shader object
  mShader = glCreateShader((GLenum)mKind);
  if (mShader == 0)
  {
    nuiCheckForGLErrorsReal();
    return false;
  }
  // Load the shader source
  const char* src = mSource.GetChars();
  glShaderSource(mShader, 1, &src, NULL);
  // Compile the shader
  glCompileShader(mShader);
  // Check the compile status
  glGetShaderiv(mShader, GL_COMPILE_STATUS, &compiled);

  if (!compiled)
  {
    GLint infoLen = 0;
    glGetShaderiv(mShader, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen > 1)
    {
      char* infoLog = (char*)malloc(sizeof(char) * infoLen);
      glGetShaderInfoLog(mShader, infoLen, NULL, infoLog);
      mError = infoLog;
      free(infoLog);
    }
    glDeleteShader(mShader);
    mShader = 0;
    return false;
  }

  return true;
}

void nuiShader::Delete()
{
  NGL_ASSERT(mShader != 0);
  glDeleteShader(mShader);
  mShader = 0;
}

GLuint nuiShader::GetShader() const
{
  return mShader;
}

bool nuiShader::IsValid() const
{
  return mShader != 0;
}

const nglString& nuiShader::GetError() const
{
  return mError;
}


////////////////////// nuiShader Program:
#ifdef _OPENGL_ES_
nglString nuiShaderProgram::mDefaultPrefix("precision mediump float;\n");
#else
nglString nuiShaderProgram::mDefaultPrefix;
#endif

nuiShaderProgram::nuiShaderProgram(const nglString& rName)
: mName(rName), mProgram(0), mPrefix(mDefaultPrefix)
{
  NGL_ASSERT(gpPrograms.find(rName) == gpPrograms.end());
  gpPrograms[rName] = this;
  Init();
}

nuiShaderProgram* nuiShaderProgram::GetProgram(const nglString& rName)
{
  auto it = gpPrograms.find(rName);
  if (it == gpPrograms.end())
    return NULL;

  nuiShaderProgram* pShader = it->second;
  pShader->Acquire();
  return pShader;
}

void nuiShaderProgram::ClearAll()
{
  auto it = gpPrograms.begin();
  auto end = gpPrograms.end();

  while (it != end)
  {
    nuiShaderProgram* pShader = it->second;
    pShader->Release();
    ++it;
  }
}

void nuiShaderProgram::Init()
{
  if (!gParamTypeMap.empty())
    return;

  for (int i = 0; ShaderParamTypeDesc[i].mEnum != GL_ZERO; i++)
  {
    gParamTypeMap[ShaderParamTypeDesc[i].mEnum] = ShaderParamTypeDesc[i];
  }
}

nuiShaderProgram::~nuiShaderProgram()
{
  std::map<GLenum, nuiShader*>::iterator it = mShaders.begin();
  std::map<GLenum, nuiShader*>::iterator end = mShaders.end();
  while (it != end)
  {
    it->second->Release();
    ++it;
  }

  if (mProgram)
    glDeleteProgram(mProgram);

  {
    auto it = gpPrograms.find(mName);
    NGL_ASSERT(it != gpPrograms.end());
    gpPrograms.erase(it);
  }
}

void nuiShaderProgram::AddShaderFromPath(nuiShaderKind shaderType, const nglPath& rPath)
{
  nglIStream* pStream = rPath.OpenRead();
  if (!pStream)
  {
    NGL_LOG("painter", NGL_LOG_ERROR, "Unable to open shader file '%s'\n", rPath.GetChars());
    return;
  }

  AddShader(shaderType, *pStream);
  delete pStream;
}

void nuiShaderProgram::AddShader(nuiShaderKind shaderType, nglIStream& rStream)
{
  nuiShader* pShader = new nuiShader(shaderType, rStream, mPrefix);
  if (mShaders[shaderType] != NULL)
    mShaders[shaderType]->Release();
  mShaders[shaderType] = pShader;
}

void nuiShaderProgram::AddShader(nuiShaderKind shaderType, const nglString& rSrc)
{
  nuiShader* pShader = new nuiShader(shaderType, rSrc, mPrefix);
  mShaders[shaderType] = pShader;
}

void nuiShaderProgram::SetPrefix(const nglString& rPrefix)
{
  mPrefix = rPrefix;
}

void nuiShaderProgram::SetDefaultPrefix(const nglString& rPrefix)
{
  mDefaultPrefix = rPrefix;
}

void nuiShaderProgram::LoadDefaultShaders()
{
  AddShader(eVertexShader, defaultVertexShader);
  AddShader(eFragmentShader, defaultFragmentShader);
}

nuiShaderState * nuiShaderProgram::NewState() const
{
  return new nuiShaderState(const_cast<nuiShaderProgram*>(this));
}

GLint nuiShaderProgram::GetUniformLocation(const char *name)
{
	return glGetUniformLocation(mProgram, name);
}

GLint nuiShaderProgram::GetUniformLocation(const nglString& name)
{
	return glGetUniformLocation(mProgram, name.GetChars());
}

int32 nuiShaderProgram::GetUniformIndex(const char *name)
{
  GLint loc = GetUniformLocation(name);
  if (loc >= 0)
    return mUniformMap[loc];
  return -1;
}

int32 nuiShaderProgram::GetUniformIndex(const nglString& name)
{
  GLint loc = GetUniformLocation(name);
  if (loc >= 0)
    return mUniformMap[loc];
  return -1;
}

GLint nuiShaderProgram::GetVertexAttribLocation(const char *name)
{
  return glGetAttribLocation(mProgram, name);
}

GLint nuiShaderProgram::GetVertexAttribLocation(const nglString& name)
{
  return glGetAttribLocation(mProgram, name.GetChars());
}

bool nuiShaderProgram::Link()
{
  mProgram = glCreateProgram();

  nuiShaderKind kinds[] =
  {
    eVertexShader,
    eFragmentShader
  };
  for (int i = 0; i < 2; i++)
  {
    nuiShaderKind k = kinds[i];
    std::map<GLenum, nuiShader*>::iterator it = mShaders.find(k);
    if (it != mShaders.end())
    {
      nuiShader* pShader = it->second;
      if (!pShader->Load())
      {
        nuiCheckForGLErrors();
        NGL_LOG("painter", NGL_LOG_ERROR, "nuiShaderProgram::Link() Unable to load shader: %s", pShader->GetError().GetChars());
        return false;
      }

      glAttachShader(mProgram, pShader->GetShader());
      nuiCheckForGLErrors();
    }
  }


  glLinkProgram(mProgram);
  nuiCheckForGLErrors();

  // 3
  GLint linkSuccess;
  glGetProgramiv(mProgram, GL_LINK_STATUS, &linkSuccess);
  nuiCheckForGLErrors();
  if (linkSuccess == GL_FALSE)
  {
    GLchar messages[256];
    glGetProgramInfoLog(mProgram, sizeof(messages), 0, &messages[0]);
    nuiCheckForGLErrors();
    NGL_LOG("painter", NGL_LOG_ERROR, "nuiShaderProgram::Link() %s", messages);
    return false;
  }

  glValidateProgram(mProgram);
  nuiCheckForGLErrors();


  glUseProgram(mProgram);
  nuiCheckForGLErrors();

  InitUniforms();
  
  // Enumerate Vertex Attributes:
  {
    int total = -1;
    glGetProgramiv(mProgram, GL_ACTIVE_ATTRIBUTES, &total);
    for (int i = 0; i < total; ++i)
    {
      int name_len = -1;
      int num = -1;
      GLenum type = GL_ZERO;
      char name[100];
      glGetActiveAttrib(mProgram, GLuint(i), sizeof(name)-1, &name_len, &num, &type, name);
      name[name_len] = 0;
      GLuint location = glGetAttribLocation(mProgram, name);

      mAttribMap[name] = nuiVertexAttribDesc(name, type, num, location);
    }
  }

  mVA_Position = glGetAttribLocation(mProgram, "Position");
  mVA_TexCoord = glGetAttribLocation(mProgram, "TexCoord");
  mVA_Color = glGetAttribLocation(mProgram, "Color");
  mVA_Normal = glGetAttribLocation(mProgram, "Normal");

  return true;
}

void nuiShaderProgram::InitUniforms()
{
  GLuint pgm = GetProgram();
  {
    int total = -1;
    glGetProgramiv(pgm, GL_ACTIVE_UNIFORMS, &total);
    size_t offset = 0;
    for (int i = 0; i < total; ++i)
    {
      int name_len = -1;
      int num = -1;
      GLenum type = GL_ZERO;
      char name[100];
      glGetActiveUniform(pgm, GLuint(i), sizeof(name)-1, &name_len, &num, &type, name);
      name[name_len] = 0;
      //glBindAttribLocation(pgm, i, name);
      GLuint location = glGetUniformLocation(pgm, name);
      
//      rIndexMap[location] = i;
      //NGL_OUT("ShaderProgram %p Uniform: %d %s %s[%d]\n", pProgram, location, Type2String(type), name , num);
      auto desc = ShaderParamTypeDesc[type];
      offset += desc.mSize * desc.mComponentSize;
      mUniforms.push_back(nuiUniformDesc(name, type, num, location, offset, this));
    }
  }
}

bool nuiShaderProgram::Validate() const
{
  GLint validation = GL_FALSE;
  glGetProgramiv(mProgram, GL_VALIDATE_STATUS, &validation);
  if (validation == GL_FALSE)
  {
    GLchar messages[256];
    glGetProgramInfoLog(mProgram, sizeof(messages), 0, &messages[0]);
    nuiCheckForGLErrors();
    //NGL_LOG("painter", NGL_LOG_ERROR, "nuiShaderProgram::Validate() failed: %s", messages);
    return false;
  }

  return true;
}


GLint nuiShaderProgram::GetVAPositionLocation() const
{
  return mVA_Position;
}

GLint nuiShaderProgram::GetVATexCoordLocation() const
{
  return mVA_TexCoord;
}

GLint nuiShaderProgram::GetVAColorLocation() const
{
  return mVA_Color;
}

GLint nuiShaderProgram::GetVANormalLocation() const
{
  return mVA_Normal;
}

GLint nuiShaderProgram::GetProgram() const
{
  return mProgram;
}

const size_t nuiShaderProgram::GetUniformCount() const
{
  return mUniforms.size();
}

const nuiUniformDesc& nuiShaderProgram::GetUniformDesc(size_t i) const
{
  return mUniforms[i];
}

//////////////////////////////////////////////////////////////////////
GLint nuiShaderProgram::GetProjectionMatrixLocation() const
{
  return mProjectionMatrix;
}

GLint nuiShaderProgram::GetModelViewMatrixLocation() const
{
  return mModelViewMatrix;
}

GLint nuiShaderProgram::GetSurfaceMatrixLocation() const
{
  return mSurfaceMatrix;
}

GLint nuiShaderProgram::GetOffsetLocation() const
{
  return mOffset;
}

GLint nuiShaderProgram::GetTextureScaleLocation() const
{
  return mTextureScale;
}

GLint nuiShaderProgram::GetTextureTranslateLocation() const
{
  return mTextureTranslate;
}

GLint nuiShaderProgram::GetDifuseColorLocation() const
{
  return mDifuseColor;
}

size_t nuiShaderProgram::GetStateDataSize() const
{
  size_t size = 0;
  
  for (size_t i = 0; i < GetUniformCount(); i++)
  {
    auto t = GetUniformDesc(i).mType;
    const auto& type = gParamTypeMap[t];
    auto s = type.mSize;
    auto cs = type.mComponentSize;
    size += s * cs;
  }
  
  return size;
}

//////////////////////////////////////////////////////////////////////
std::map<nglString, nuiShaderProgram*> nuiShaderProgram::gpPrograms;

