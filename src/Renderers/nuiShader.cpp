
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
} ShaderParamTypeDesc[] =
{
  {GL_FLOAT	,	GL_FLOAT,	1},
  {GL_FLOAT_VEC2	,	GL_FLOAT,	2},
  {GL_FLOAT_VEC3	,	GL_FLOAT,	3},
  {GL_FLOAT_VEC4	,	GL_FLOAT,	4},
  {GL_INT, GL_INT, 1},
  {GL_INT_VEC2, GL_INT, 2},
  {GL_INT_VEC3, GL_INT, 3},
  {GL_INT_VEC4, GL_INT, 4},
  {GL_UNSIGNED_INT, GL_INT, 1},
  {GL_BOOL, GL_INT, 1},
  {GL_BOOL_VEC2, GL_INT, 2},
  {GL_BOOL_VEC3, GL_INT, 3},
  {GL_BOOL_VEC4, GL_INT, 4},
  {GL_FLOAT_MAT2,	GL_FLOAT,	4},
  {GL_FLOAT_MAT3,	GL_FLOAT,	9},
  {GL_FLOAT_MAT4,	GL_FLOAT,	16},
  {GL_SAMPLER_2D, GL_INT, 1},
  {GL_SAMPLER_CUBE, GL_INT, 1},
  {GL_ZERO, GL_ZERO, 0}
};

/////////////////////////////////////////////////////
nuiShaderState::nuiShaderState(nuiShaderProgram* pProgram, std::map<GLuint, int32>& rIndexMap)
: mpProgram(NULL),
  mProjectionMatrix(-1),
  mModelViewMatrix(-1),
  mSurfaceMatrix(-1),
  mOffset(-1),
  mTextureScale(-1),
  mTextureTranslate(-1),
  mDifuseColor(-1)
{
  //NGL_OUT("nuiShaderState Ctor %p\n", this);
  //SetTrace(true);
  
  if (pProgram)
    InitWithProgram(pProgram, rIndexMap);
}

nuiShaderState::nuiShaderState(const nuiShaderState& rOriginal)
: mpProgram(rOriginal.mpProgram), mUniforms(rOriginal.mUniforms), mProjectionMatrix(rOriginal.mProjectionMatrix), mModelViewMatrix(rOriginal.mModelViewMatrix), mSurfaceMatrix(rOriginal.mSurfaceMatrix),  mOffset(rOriginal.mOffset), mTextureScale(rOriginal.mTextureScale), mTextureTranslate(rOriginal.mTextureTranslate), mDifuseColor(rOriginal.mDifuseColor)
{
  //NGL_OUT("nuiShaderState Ctor %p (from %p)\n", this, &rOriginal);
}

nuiShaderState::~nuiShaderState()
{
  //NGL_OUT("nuiShaderState Dtor %p\n", this);
}

void nuiShaderState::Clear()
{
  mUniforms.clear();
}

void nuiShaderState::Set(const nglString& rName, const float* pV, int32 count, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, pV, count, Apply);
}

void nuiShaderState::Set(const nglString& rName, const std::vector<float>& rV, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, &rV[0], rV.size(), Apply);
}

void nuiShaderState::Set(const nglString& rName, const int32* pV, int32 count, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, pV, count, Apply);
}

void nuiShaderState::Set(const nglString& rName, const std::vector<int32>& rV, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, rV, Apply);
}

////
void nuiShaderState::Set(const nglString& rName, float v1, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, Apply);
}

void nuiShaderState::Set(const nglString& rName, float v1, float v2, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, v2, Apply);
}

void nuiShaderState::Set(const nglString& rName, float v1, float v2, float v3, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, v2, v3, Apply);
}

void nuiShaderState::Set(const nglString& rName, float v1, float v2, float v3, float v4, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, v2, v3, v4, Apply);
}

void nuiShaderState::Set(const nglString& rName, const nglVector2f& rVec, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, rVec, Apply);
}

void nuiShaderState::Set(const nglString& rName, const nglVector3f& rVec, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, rVec, Apply);
}

void nuiShaderState::Set(const nglString& rName, const nglVectorf& rVec, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, rVec, Apply);
}

void nuiShaderState::Set(const nglString& rName, const nuiColor& rColor, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, rColor, Apply);
}

void nuiShaderState::Set(const nglString& rName, int32 v1, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, Apply);
}

void nuiShaderState::Set(const nglString& rName, int32 v1, int32 v2, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, v2, Apply);
}

void nuiShaderState::Set(const nglString& rName, int32 v1, int32 v2, int32 v3, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, v2, v3, Apply);
}

void nuiShaderState::Set(const nglString& rName, int32 v1, int32 v2, int32 v3, int32 v4, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, v1, v2, v3, v4, Apply);
}

void nuiShaderState::Set(const nglString& rName, const nglMatrixf& rMat, bool Apply)
{
  GLint loc = mpProgram->GetUniformIndex(rName);
  if (loc >= 0)
    Set(loc, rMat, Apply);
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

void nuiShaderState::InitWithProgram(nuiShaderProgram* pProgram, std::map<GLuint, int32>& rIndexMap)
{
  Clear();
  mpProgram = pProgram;

  GLuint pgm = mpProgram->GetProgram();
  {
    int total = -1;
    glGetProgramiv(pgm, GL_ACTIVE_UNIFORMS, &total);
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

      rIndexMap[location] = i;
      //NGL_OUT("ShaderProgram %p Uniform: %d %s %s[%d]\n", pProgram, location, Type2String(type), name , num);
      mUniforms.push_back(nuiUniformDesc(name, type, num, location, pProgram));
    }
  }

  mProjectionMatrix = mpProgram->GetUniformIndex(NUI_PROJECTION_MATRIX_NAME);
  mModelViewMatrix = mpProgram->GetUniformIndex(NUI_MODELVIEW_MATRIX_NAME);
  mSurfaceMatrix = mpProgram->GetUniformIndex(NUI_SURFACE_MATRIX_NAME);
  mOffset = mpProgram->GetUniformIndex(NUI_OFFSET_NAME);
  mTextureScale = mpProgram->GetUniformIndex(NUI_TEXTURE_SCALE_NAME);
  mTextureTranslate = mpProgram->GetUniformIndex(NUI_TEXTURE_TRANSLATE_NAME);
  mDifuseColor = mpProgram->GetUniformIndex(NUI_DIFUSE_COLOR_NAME);
}

///
void nuiShaderState::Set(GLint loc, const float* pV, int32 count, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(pV, count, Apply);
}

void nuiShaderState::Set(GLint loc, const std::vector<float>& rV, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(rV, Apply);
}

void nuiShaderState::Set(GLint loc, const int32* pV, int32 count, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(pV, count, Apply);
}

void nuiShaderState::Set(GLint loc, const std::vector<int32>& rV, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(rV, Apply);
}


void nuiShaderState::Set(GLint loc, float v1, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(v1, Apply);
}

void nuiShaderState::Set(GLint loc, float v1, float v2, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(v1, v2, Apply);
}

void nuiShaderState::Set(GLint loc, float v1, float v2, float v3, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(v1, v2, v3, Apply);
}

void nuiShaderState::Set(GLint loc, float v1, float v2, float v3, float v4, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(v1, v2, v3, v4, Apply);
}

void nuiShaderState::Set(GLint loc, const nglVector2f& rVec, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(rVec, Apply);
}

void nuiShaderState::Set(GLint loc, const nglVector3f& rVec, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(rVec, Apply);
}

void nuiShaderState::Set(GLint loc, const nglVectorf& rVec, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(rVec, Apply);
}

void nuiShaderState::Set(GLint loc, const nuiColor& rColor, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(rColor, Apply);
}

void nuiShaderState::Set(GLint loc, int32 v1, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(v1, Apply);
}

void nuiShaderState::Set(GLint loc, int32 v1, int32 v2, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(v1, v2, Apply);
}

void nuiShaderState::Set(GLint loc, int32 v1, int32 v2, int32 v3, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(v1, v2, v3, Apply);
}

void nuiShaderState::Set(GLint loc, int32 v1, int32 v2, int32 v3, int32 v4, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(v1, v2, v3, v4, Apply);
}

void nuiShaderState::Set(GLint loc, const nglMatrixf& rMat, bool Apply)
{
  NGL_ASSERT(loc < mUniforms.size());
  mUniforms[loc].Set(rMat, Apply);
}

void nuiShaderState::SetProjectionMatrix(const nglMatrixf& rMat, bool Apply)
{
  if (mProjectionMatrix >= 0)
  {
    Set(mProjectionMatrix, rMat);
    if (Apply)
      mUniforms[mProjectionMatrix].Apply();
  }
}

void nuiShaderState::SetModelViewMatrix(const nglMatrixf& rMat, bool Apply)
{
  if (mModelViewMatrix >= 0)
  {
    Set(mModelViewMatrix, rMat);
    if (Apply)
      mUniforms[mModelViewMatrix].Apply();
  }
}

void nuiShaderState::SetSurfaceMatrix(const nglMatrixf& rMat, bool Apply)
{
  if (mSurfaceMatrix >= 0)
  {
    Set(mSurfaceMatrix, rMat);
    if (Apply)
      mUniforms[mSurfaceMatrix].Apply();
  }
}

void nuiShaderState::SetOffset(float OffsetX, float OffsetY, bool Apply)
{
  if (mOffset >= 0)
  {
    Set(mOffset, OffsetX, OffsetY);
    if (Apply)
      mUniforms[mOffset].Apply();
  }
}

void nuiShaderState::SetTextureTranslate(const nglVector2f& rTranslate, bool Apply)
{
  if (mTextureTranslate >= 0)
  {
    Set(mTextureTranslate, rTranslate);
    if (Apply)
      mUniforms[mTextureTranslate].Apply();
  }
}

void nuiShaderState::SetTextureScale(const nglVector2f& rScale, bool Apply)
{
  if (mTextureScale >= 0)
  {
    Set(mTextureScale, rScale);
    if (Apply)
      mUniforms[mTextureScale].Apply();
  }
}

void nuiShaderState::SetDifuseColor(const nuiColor& rDifuseColor, bool Apply)
{
  if (mDifuseColor >= 0)
  {
    Set(mDifuseColor, rDifuseColor);
    if (Apply)
      mUniforms[mDifuseColor].Apply();
  }
}


void nuiShaderState::Apply() const
{
  for (size_t i = 0; i < mUniforms.size(); i++)
    mUniforms[i].Apply();
}


bool nuiShaderState::operator == (const nuiShaderState& rState) const
{
  if (mpProgram != rState.mpProgram)
    return false;

  if (mProjectionMatrix != rState.mProjectionMatrix)
    return false;

  if (mModelViewMatrix != rState.mModelViewMatrix)
    return false;

  if (mSurfaceMatrix != rState.mSurfaceMatrix)
    return false;

  return mUniforms == rState.mUniforms;
}

nuiShaderState& nuiShaderState::operator= (const nuiShaderState& rState)
{
  mpProgram = rState.mpProgram;
  mProjectionMatrix = rState.mProjectionMatrix;
  mModelViewMatrix = rState.mModelViewMatrix;
  mSurfaceMatrix = rState.mSurfaceMatrix;
  mOffset = rState.mOffset;
  mTextureScale = rState.mTextureScale;
  mTextureTranslate = rState.mTextureTranslate;
  mDifuseColor = rState.mDifuseColor;
  mUniforms = rState.mUniforms;
  return *this;
}

void nuiShaderState::Set(const nuiShaderState& rState)
{
  for (int32 i = 0; i < mUniforms.size(); i++)
    mUniforms[i].Set(rState.mUniforms[i]);
}

void nuiShaderState::Dump() const
{
  NGL_OUT("Dumping state for program %p\n", mpProgram);
  for (int32 i = 0; i < mUniforms.size(); i++)
    mUniforms[i].Dump();
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
: mName(rName), mProgram(0), mpCurrentState(NULL), mPrefix(mDefaultPrefix)
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

std::map<GLenum, std::pair<GLenum, GLint> > nuiShaderProgram::gParamTypeMap;

void nuiShaderProgram::Init()
{
  if (!gParamTypeMap.empty())
    return;

  for (int i = 0; ShaderParamTypeDesc[i].mEnum != GL_ZERO; i++)
  {
    gParamTypeMap[ShaderParamTypeDesc[i].mEnum] = std::make_pair(ShaderParamTypeDesc[i].mType, ShaderParamTypeDesc[i].mSize);
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

  if (mpCurrentState)
    mpCurrentState->Release();

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


nuiShaderState* nuiShaderProgram::GetCurrentState() const
{
  return mpCurrentState;
}

nuiShaderState* nuiShaderProgram::CopyCurrentState() const
{
  nuiShaderState* pState = new nuiShaderState(*mpCurrentState);
  return pState;
}

void nuiShaderProgram::SetState(const nuiShaderState& rState, bool apply)
{
  mpCurrentState->Set(rState);

  if (apply)
    ApplyState();
}

void nuiShaderProgram::ApplyState()
{
  mpCurrentState->Apply();
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


  // Enumerate Uniforms:
  mpCurrentState = new nuiShaderState(this, mUniformMap);

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


// Receive Uniform variables:
void nuiShaderProgram::GetUniformfv(GLint index, GLfloat* values)
{
  glGetUniformfv(mProgram, index, values);
}

void nuiShaderProgram::GetUniformiv(GLint index, GLint* values)
{
  glGetUniformiv(mProgram, index, values);
}

//////////////////////////////////////////////////////////////////////
void nuiShaderProgram::BindAttribLocation(GLint index, GLchar* name)
{
  glBindAttribLocation(mProgram, index, name);
}

std::map<nglString, nuiShaderProgram*> nuiShaderProgram::gpPrograms;

