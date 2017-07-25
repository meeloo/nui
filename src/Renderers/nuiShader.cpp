
//
//  nuiShader.cpp
//  nui3
//
//  Created by Sébastien Métrot on 1/10/13.
//  Copyright (c) 2013 libNUI. All rights reserved.
//

#include "nui.h"
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

static const char* types[] = {
  "Buffer",
  "ThreadgroupMemory",
  "Texture",
  "Sampler"
};

static const char* dataTypes[] = {
  "None",
  
  "Struct",
  "Array",
  
  "Float",
  "Float2",
  "Float3",
  "Float4",
  
  "Float2x2",
  "Float2x3",
  "Float2x4",
  
  "Float3x2",
  "Float3x3",
  "Float3x4",
  
  "Float4x2",
  "Float4x3",
  "Float4x4",
  
  "Half",
  "Half2",
  "Half3",
  "Half4",
  
  "Half2x2",
  "Half2x3",
  "Half2x4",
  
  "Half3x2",
  "Half3x3",
  "Half3x4",
  
  "Half4x2",
  "Half4x3",
  "Half4x4",
  
  "Int",
  "Int2",
  "Int3",
  "Int4",
  
  "UInt",
  "UInt2",
  "UInt3",
  "UInt4",
  
  "Short",
  "Short2",
  "Short3",
  "Short4",
  
  "UShort",
  "UShort2",
  "UShort3",
  "UShort4",
  
  "Char",
  "Char2",
  "Char3",
  "Char4",
  
  "UChar",
  "UChar2",
  "UChar3",
  "UChar4",
  
  "Bool",
  "Bool2",
  "Bool3",
  "Bool4",
  
  
  "Texture",
  "Sampler",
  "Pointer"
};


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
  mData.resize(rOriginal.mData.size());
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
    case GL_FLOAT:        return "float";
    case GL_FLOAT_VEC2:   return "vec2";
    case GL_FLOAT_VEC3:   return "vec3";
    case GL_FLOAT_VEC4:   return "vec4";

    case GL_INT:          return "int";
    case GL_INT_VEC2:     return "ivec2";
    case GL_INT_VEC3:     return "ivec3";
    case GL_INT_VEC4:     return "ivec4";
    case GL_UNSIGNED_INT: return "uint";
    case GL_SAMPLER_2D:   return "sampler2D";
    case GL_SAMPLER_CUBE: return "samplerCube";

    case GL_FLOAT_MAT2:   return "mat2";
    case GL_FLOAT_MAT3:   return "mat3";
    case GL_FLOAT_MAT4:   return "mat4";

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
  size_t index = mpProgram->GetUniformDescIndex(loc);
  const auto& desc = mpProgram->GetUniformDesc(index);
  size_t offset = desc.mOffset;
  memcpy(&mData[0] + offset, pV, count * sizeof(float));
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

  const nglString& GetSource() const;
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
  if (mShader != 0)
  {
    glDeleteShader(mShader);
  }
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

const nglString& nuiShader::GetSource() const
{
  return mSource;
}


////////////////////// nuiShader Program:
#ifdef _OPENGL_ES_
nglString nuiShaderProgram::mDefaultPrefix("precision mediump float;\n");
#else
nglString nuiShaderProgram::mDefaultPrefix;
#endif

nuiShaderProgram::nuiShaderProgram(nglContext* pContext, const nglString& rName)
: mName(rName), mProgram(0), mPrefix(mDefaultPrefix), mpContext(pContext)
{
  NGL_ASSERT(gpPrograms.find(rName) == gpPrograms.end());
  gpPrograms[rName] = this;
  Init();
}

nuiShaderProgram* nuiShaderProgram::GetProgram(nglContext* pContext, const nglString& rName)
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

void* nuiShaderProgram::GetMetalLibrary() const
{
  return mpMetalLibrary;
}

void* nuiShaderProgram::GetMetalFunction(const nglString& rFunctionName) const
{
  id<MTLLibrary> library = (id<MTLLibrary>)mpMetalLibrary;
  id<MTLFunction> function = [library newFunctionWithName:(NSString*)rFunctionName.ToCFString()];
  return function;
}

void* nuiShaderProgram::GetMetalPipelineState() const // id<MTLRenderPipelineState>
{
  return mMetalPipelineState;
}

static GLenum GLTypeFromMetalType(MTLDataType type)
{
  switch (type)
  {
    case MTLDataTypeNone: return 0;

    case MTLDataTypeStruct: return 0;
    case MTLDataTypeArray: return 0;
      
    case MTLDataTypeFloat: return GL_FLOAT;
    case MTLDataTypeFloat2: return GL_FLOAT_VEC2;
    case MTLDataTypeFloat3: return GL_FLOAT_VEC3;
    case MTLDataTypeFloat4: return GL_FLOAT_VEC4;
      
    case MTLDataTypeFloat2x2: return GL_FLOAT_MAT2;
    case MTLDataTypeFloat2x3: return GL_FLOAT_MAT2x3;
    case MTLDataTypeFloat2x4: return GL_FLOAT_MAT2x4;
      
    case MTLDataTypeFloat3x2: return GL_FLOAT_MAT3x2;
    case MTLDataTypeFloat3x3: return GL_FLOAT_MAT3;
    case MTLDataTypeFloat3x4: return GL_FLOAT_MAT3x4;
      
    case MTLDataTypeFloat4x2: return GL_FLOAT_MAT4x2;
    case MTLDataTypeFloat4x3: return GL_FLOAT_MAT4x3;
    case MTLDataTypeFloat4x4: return GL_FLOAT_MAT4;
      
    case MTLDataTypeHalf: return GL_HALF_FLOAT;
    case MTLDataTypeHalf2: return 0;
    case MTLDataTypeHalf3: return 0;
    case MTLDataTypeHalf4: return 0;
      
    case MTLDataTypeHalf2x2: return 0;
    case MTLDataTypeHalf2x3: return 0;
    case MTLDataTypeHalf2x4: return 0;
      
    case MTLDataTypeHalf3x2: return 0;
    case MTLDataTypeHalf3x3: return 0;
    case MTLDataTypeHalf3x4: return 0;
      
    case MTLDataTypeHalf4x2: return 0;
    case MTLDataTypeHalf4x3: return 0;
    case MTLDataTypeHalf4x4: return 0;
      
    case MTLDataTypeInt: return GL_INT;
    case MTLDataTypeInt2: return GL_INT_VEC2;
    case MTLDataTypeInt3: return GL_INT_VEC3;
    case MTLDataTypeInt4: return GL_INT_VEC4;
      
    case MTLDataTypeUInt: return GL_UNSIGNED_INT;
    case MTLDataTypeUInt2: return GL_UNSIGNED_INT_VEC2;
    case MTLDataTypeUInt3: return GL_UNSIGNED_INT_VEC3;
    case MTLDataTypeUInt4: return GL_UNSIGNED_INT_VEC4;
      
    case MTLDataTypeShort: return GL_SHORT;
    case MTLDataTypeShort2: return 0;
    case MTLDataTypeShort3: return 0;
    case MTLDataTypeShort4: return 0;
      
    case MTLDataTypeUShort: return GL_UNSIGNED_SHORT;
    case MTLDataTypeUShort2: return 0;
    case MTLDataTypeUShort3: return 0;
    case MTLDataTypeUShort4: return 0;
      
    case MTLDataTypeChar: return GL_BYTE;
    case MTLDataTypeChar2: return 0;
    case MTLDataTypeChar3: return 0;
    case MTLDataTypeChar4: return 0;
      
    case MTLDataTypeUChar: return 0;
    case MTLDataTypeUChar2: return 0;
    case MTLDataTypeUChar3: return 0;
    case MTLDataTypeUChar4: return 0;
      
    case MTLDataTypeBool: return GL_BOOL;
    case MTLDataTypeBool2: return GL_BOOL_VEC2;
    case MTLDataTypeBool3: return GL_BOOL_VEC3;
    case MTLDataTypeBool4: return GL_BOOL_VEC4;
      
    case MTLDataTypeTexture: return GL_TEXTURE;
    case MTLDataTypeSampler: return GL_SAMPLER;
    case MTLDataTypePointer: return 0;
  }
  return 0;
}

#if defined _METAL_
void nuiShaderProgram::ParseStructMember(int depth, const nglString& parentName, void* _member)
{
  MTLStructMember* member = (MTLStructMember*)_member;
//  nglString indent("\t\t");
//  for (int i = 0; i < depth; i++)
//    indent.Add("\t");
//  NGL_OUT("%s%s %s.%s (@%d)\n", indent.GetChars(), dataTypes[member.dataType], parentName.GetChars(), [member.name UTF8String], member.offset);
  
  nglString name(parentName);
  name.Add(".").Add((CFStringRef)member.name);
  auto location = mUniforms.size();
  mUniformIndexes[location] = location;
  int num = 1;
  GLenum type = GLTypeFromMetalType(member.dataType);
  
  if (member.dataType == MTLDataTypeStruct)
  {
    NGL_ASSERT(member.arrayType != nil);
    type = GLTypeFromMetalType(member.arrayType.elementType);
    num = member.arrayType.arrayLength;
  }
  
  mUniforms.push_back(nuiUniformDesc(name, type, num, location, member.offset, this));
  
  if (member.dataType == MTLDataTypeStruct)
  {
    for (MTLStructMember* smember in member.structType.members)
    {
      nglString parent(parentName);
      parent.Add(".").Add((CFStringRef)member.name);
      ParseStructMember(depth + 1, parent, smember);
    }
  }
}

void nuiShaderProgram::ParseArgument(const char* domain, void* _argument)
{
  MTLArgument* argument = (MTLArgument*)_argument;
  nglString n((CFStringRef)argument.name);
  if (argument.type == MTLArgumentTypeBuffer)
  {
    if (argument.bufferDataType == MTLDataTypeStruct)
    {
      //NGL_OUT("\t%s Argument [[%s %d]] - %s (struct size = %d)\n", domain, types[argument.type], argument.index, n.GetChars(), argument.bufferDataSize);
      for (MTLStructMember* member in argument.bufferStructType.members)
      {
        ParseStructMember(0, nglString((CFStringRef)argument.name), member);
      }
      
      if (!mUniformStructSize); ///  Only one struct per shader for now please, we're pretty dumb still
        mUniformStructSize = argument.bufferDataSize;
    }
    else
    {
      bool isArray = false;
      if (@available(macOS 10_13, *)) {
        isArray = argument.arrayLength > 1;
      }
      if (isArray)
      {
        if (@available(macOS 10_13, *)) {
          //NGL_OUT("\t%s Argument [[%s %d]] - %s[?] %s (array size %d)\n", domain, types[argument.type], argument.index, dataTypes[argument.bufferDataType], n.GetChars(), argument.arrayLength);
        }
      }
      else
      {
        //NGL_OUT("\t%s Argument [[%s %d]] - %s[?] %s\n", domain, types[argument.type], argument.index, dataTypes[argument.bufferDataType], n.GetChars());
      }
    }
  }
  else
  {
    //NGL_OUT("\t%s Argument [[%s %d]] - %s\n", domain, types[argument.type], argument.index, n.GetChars());
  }

}
#endif

bool nuiShaderProgram::Link()
{
#ifdef _METAL_
  id<MTLDevice> device = (id<MTLDevice>)mpContext->GetMetalDevice();
  if (device)
  {
    // This is a metal display
    nglString source;
    auto it =  mShaders.find(eVertexShader);
    if (it != mShaders.end())
    {
      nuiShader* pShader = it->second;
      source.Add(pShader->GetSource());
    }
    
    it =  mShaders.find(eFragmentShader);
    if (it != mShaders.end())
    {
      nuiShader* pShader = it->second;
      source.Add(pShader->GetSource());
    }

    NSError* error = nil;
    id<MTLLibrary> library = [device newLibraryWithSource:(NSString*)source.ToCFString() options:nil error:&error];;
    mpMetalLibrary = library;
    if (!library || error)
    {
      NSString* reason = error.localizedDescription;
      nglString msg((CFStringRef)reason);
      NGL_OUT("Metal Shader compilation error:\n%s\n", msg.GetChars());
      return false;
    }
    
    
    id<MTLFunction> vertex_function = [library newFunctionWithName:@"vertex_main"];
    id<MTLFunction> fragment_function = [library newFunctionWithName:@"fragment_main"];
    
    CAMetalLayer* metalLayer = (CAMetalLayer*)mpContext->GetMetalLayer();
    MTLAutoreleasedRenderPipelineReflection reflection = nil;
    MTLRenderPipelineDescriptor *descriptor = [MTLRenderPipelineDescriptor new];
    descriptor.vertexFunction = vertex_function;
    descriptor.fragmentFunction = fragment_function;
    descriptor.colorAttachments[0].pixelFormat = metalLayer.pixelFormat;
    
    id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:descriptor options:MTLPipelineOptionArgumentInfo+MTLPipelineOptionBufferTypeInfo reflection:&reflection error:&error];

    NGL_OUT("Function %s\n", nglString((CFStringRef)vertex_function.name).GetChars());
    for (MTLArgument* argument in reflection.vertexArguments)
    {
      ParseArgument("Vertex", argument);
    }

    NGL_OUT("Function %s\n", nglString((CFStringRef)fragment_function.name).GetChars());
    for (MTLArgument* argument in reflection.fragmentArguments)
    {
      ParseArgument("Fragment", argument);
    }
    
    pipelineState = nil;
  }
  else
#endif
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

    InitUniforms();
    
    mProjectionMatrix = GetUniformLocation(NUI_PROJECTION_MATRIX_NAME);
    mModelViewMatrix = GetUniformLocation(NUI_MODELVIEW_MATRIX_NAME);
    mSurfaceMatrix = GetUniformLocation(NUI_SURFACE_MATRIX_NAME);
    mOffset = GetUniformLocation(NUI_OFFSET_NAME);
    mTextureScale = GetUniformLocation(NUI_TEXTURE_SCALE_NAME);
    mTextureTranslate = GetUniformLocation(NUI_TEXTURE_TRANSLATE_NAME);
    mDifuseColor = GetUniformLocation(NUI_DIFUSE_COLOR_NAME);
  }

  return true;
}

void nuiShaderProgram::InitUniforms()
{
  GLuint total = 0;
  glGetProgramiv(mProgram, GL_ACTIVE_UNIFORMS, (GLint*)&total);
  size_t offset = 0;
  for (GLuint i = 0; i < total; ++i)
  {
    GLint name_len = 100;
    GLint num = -1;
    GLenum type = GL_ZERO;
    char name[name_len+1];
    glGetActiveUniform(mProgram, i, sizeof(name)-1, &name_len, &num, &type, name);
    name[name_len] = 0;
    //glBindAttribLocation(mProgram, i, name);
    GLint location = glGetUniformLocation(mProgram, name);
    
//      rIndexMap[location] = i;
    NGL_OUT("ShaderProgram %p Uniform: %d %s %s[%d]\n", this, location, Type2String(type), name , num);
    auto desc = gParamTypeMap[type];
    mUniformIndexes[location] = mUniforms.size();
    mUniforms.push_back(nuiUniformDesc(name, type, num, location, offset, this));
    offset += desc.mSize * desc.mComponentSize;
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

const nuiUniformDesc& nuiShaderProgram::GetUniformDesc(size_t index) const
{
  return mUniforms[index];
}

size_t nuiShaderProgram::GetUniformDescIndex(GLint location) const
{
  auto it = mUniformIndexes.find(location);
  if (it != mUniformIndexes.end())
    return it->second;
  return -1;
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
  if (mUniformStructSize)
    return mUniformStructSize;
  
  size_t size = 0;
  
  for (size_t i = 0; i < GetUniformCount(); i++)
  {
    auto t = GetUniformDesc(i).mType;
    const auto& type = gParamTypeMap[t];
    auto s = type.mSize;
    auto cs = type.mComponentSize;
    size += s * cs;
  }
  
  mUniformStructSize = size;
  return size;
}

//////////////////////////////////////////////////////////////////////
std::map<nglString, nuiShaderProgram*> nuiShaderProgram::gpPrograms;

