/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"

// class nuiSpriteFrame
nuiSpriteFrame::nuiSpriteFrame()
: mpTexture(NULL),
  mX(0),
  mY(0)
{
}

nuiSpriteFrame::~nuiSpriteFrame()
{
  if (mpTexture)
    mpTexture->Release();
}

bool nuiSpriteFrame::SetTexture(nuiTexture* pTexture, const nuiRect& rRect)
{
  //printf("nuiSpriteFrame::SetTexture1 %p %s\n", pTexture, pTexture->GetSource().GetChars());
  if (pTexture)
  {
    pTexture->CheckValid();
    pTexture->Acquire();
  }

  if (mpTexture)
    mpTexture->Release();
  mpTexture = pTexture;
  mRect = rRect;
  return true;
}

bool nuiSpriteFrame::SetTexture(const nglPath& rPath, const nuiRect& rRect)
{
  if (mpTexture)
    mpTexture->Release();
  mpTexture = nuiTexture::GetTexture(rPath);
  //printf("nuiSpriteFrame::SetTexture2 %p %s\n", mpTexture, mpTexture->GetSource().GetChars());
  mRect = rRect;
  return mpTexture != NULL;
}

bool nuiSpriteFrame::SetTexture(const nglPath& rPath)
{
  if (!SetTexture(rPath, nuiRect()))
    return false;
  
  mRect = nuiRect((float)mpTexture->GetWidth(), (float)mpTexture->GetHeight());
  mX = (float)mpTexture->GetWidth() / 2;
  mY = (float)mpTexture->GetHeight() / 2;
  return true;
}

void nuiSpriteFrame::SetRect(const nuiRect& rRect)
{
  mRect = rRect;
}

void nuiSpriteFrame::SetHandle(float x, float y)
{
  mX = x;
  mY = y;
}

nuiTexture* nuiSpriteFrame::GetTexture() const
{
  return mpTexture;
}

const nuiRect& nuiSpriteFrame::GetRect() const
{
  return mRect;
}

float nuiSpriteFrame::GetHandleX() const
{
  return mX;
}

float nuiSpriteFrame::GetHandleY() const
{
  return mY;
}

//////////////////////////////////////////////////////
// class nuiSpriteAnimation
nuiSpriteAnimation::nuiSpriteAnimation()
: mFPS(10)
{

}

nuiSpriteAnimation::nuiSpriteAnimation(const nglPath& rPath)
: mFPS(10)
{
  nglPath path(rPath);
  std::vector<nglPath> children;
  path.GetChildren(&children);
  std::sort(children.begin(), children.end(), nglCompareNaturalPath);
  std::vector<nglPath>::const_iterator it = children.begin();
  std::vector<nglPath>::const_iterator end = children.end();
  for (; it != end; it++)
  {
    nglPath p = *it;
    nglString pp = p.GetRemovedExtension();
    if (pp.GetRight(3) != _T("@2x"))
    {
      nuiSpriteFrame* pFrame = new nuiSpriteFrame();
      pFrame->SetTexture(p);
      AddFrame(pFrame);
    }
  }
  
  SetName(rPath.GetNodeName());
}

nuiSpriteAnimation::~nuiSpriteAnimation()
{  
  for (size_t i = 0; i < mpFrames.size(); i++)
    delete mpFrames[i];
}

int32 nuiSpriteAnimation::GetFrameCount() const
{
  return (int32)mpFrames.size();
}

void nuiSpriteAnimation::AddFrame(nuiSpriteFrame* pFrame)
{
  mpFrames.push_back(pFrame);
}

const nuiSpriteFrame* nuiSpriteAnimation::GetFrame(int32 index) const
{
  return mpFrames[index];
}

void nuiSpriteAnimation::SetDuration(float seconds)
{
  mFPS = (float)GetFrameCount() / seconds;
}

void nuiSpriteAnimation::SetFPS(float FPS)
{
  mFPS = FPS;
}

float nuiSpriteAnimation::GetDuration() const
{
  return mFPS * GetFrameCount();
}

float nuiSpriteAnimation::GetFPS() const
{
  return mFPS;
}
               
void nuiSpriteAnimation::SetName(const nglString& rName)
{
  mName = rName;
}

const nglString& nuiSpriteAnimation::GetName() const
{
  return mName;
}
               
///////////////////////////////////////////////////////
// class nuiSpriteDef

std::map<nglString, nuiSpriteDef*> nuiSpriteDef::mSpriteMap;

nuiSpriteDef::nuiSpriteDef(const nglString& rSpriteDefName)
{
  Init();
  SetObjectName(rSpriteDefName);
  std::map<nglString, nuiSpriteDef*>::const_iterator it = mSpriteMap.find(rSpriteDefName);
  if (it != mSpriteMap.end())
    it->second->Release();
  mSpriteMap[rSpriteDefName] = this;
}

nuiSpriteDef::nuiSpriteDef(const nglPath& rSpriteDefPath)
{
  Init();
  nglString name(rSpriteDefPath.GetNodeName());
  SetObjectName(name);
  std::map<nglString, nuiSpriteDef*>::const_iterator it = mSpriteMap.find(name);
  if (it != mSpriteMap.end())
    it->second->Release();

  mSpriteMap[name] = this;

  {
    std::vector<nglPath> children;
    rSpriteDefPath.GetChildren(&children);
    std::vector<nglPath>::const_iterator it = children.begin();
    std::vector<nglPath>::const_iterator end = children.end();
    for (; it != end; it++)
    {
      nuiSpriteAnimation* pAnim = new nuiSpriteAnimation(*it);
      if (pAnim->GetFrameCount())
        AddAnimation(pAnim);
      else
        delete pAnim;
    }
  }
}

nuiSpriteDef::~nuiSpriteDef()
{
  for (size_t i = 0; i < mpAnimations.size(); i++)
    delete mpAnimations[i];
  nglString name(GetObjectName());
  //printf("~nuiSpriteDef: %p %s\n", this, name.GetChars());
  
  std::map<nglString, nuiSpriteDef*>::iterator it = mSpriteMap.find(name);
  NGL_ASSERT(it != mSpriteMap.end());
  mSpriteMap.erase(it);
}

void nuiSpriteDef::Init()
{
  CheckValid();
  if (SetObjectClass(_T("nuiSpriteDef")))
  {
    App->AddExit(&nuiSpriteDef::Uninit);
  }
}

void nuiSpriteDef::Uninit()
{
  std::map<nglString, nuiSpriteDef*>::iterator it = mSpriteMap.begin();
  std::map<nglString, nuiSpriteDef*>::iterator end = mSpriteMap.end();

  std::vector<nuiSpriteDef*> temp;
  
  while (it != end)
  {
    nuiSpriteDef* pDef = it->second;
    pDef->CheckValid();
    //pDef->Release();
    temp.push_back(pDef);
    ++it;
  }
  
  for (int32 i = 0; i < temp.size(); i++)
  {
    nuiSpriteDef* pDef = temp[i];
    pDef->Release();
  }

  NGL_ASSERT(mSpriteMap.empty());
  NGL_ASSERT(!nuiSprite::mSpriteCounter);

}

void nuiSpriteDef::AddAnimation(nuiSpriteAnimation* pAnim)
{
  CheckValid();
  mpAnimations.push_back(pAnim);
}

int32 nuiSpriteDef::GetAnimationCount() const
{
  CheckValid();
  return (int32)mpAnimations.size();
}

const nuiSpriteAnimation* nuiSpriteDef::GetAnimation(int32 index) const
{
  CheckValid();
  return mpAnimations[index];
}

int32 nuiSpriteDef::GetAnimation(const nglString& rName) const
{
  CheckValid();
  for (int32 i = 0; i < GetAnimationCount(); i++)
  {
    const nuiSpriteAnimation* pAnim = GetAnimation(i);
    if (pAnim->GetName() == rName)
      return i;
  }
  return 0;
}


nuiSpriteDef* nuiSpriteDef::GetSprite(const nglString& rName)
{
  std::map<nglString, nuiSpriteDef*>::const_iterator it = mSpriteMap.find(rName);
  if (it == mSpriteMap.end())
    return NULL;

  nuiSpriteDef* pDef = it->second;
  pDef->CheckValid();
  pDef->Acquire();
  return pDef;
}


/////////////////////////////////////////////////////////
// class nuiSprite

// static
uint32 nuiSprite::mSpriteCounter = 0;


nuiSprite::nuiSprite(const nglPath& rSpriteDefPath, bool forceReplace)
{
  mpSpriteDef = nuiSpriteDef::GetSprite(rSpriteDefPath.GetNodeName());
  if (!mpSpriteDef || forceReplace)
  {
    mpSpriteDef = new nuiSpriteDef(rSpriteDefPath);
  }

  NGL_ASSERT(mpSpriteDef);
  Init();  
}

nuiSprite::nuiSprite(const nglString& rSpriteDefName)
: mpSpriteDef(nuiSpriteDef::GetSprite(rSpriteDefName))
{
  NGL_ASSERT(mpSpriteDef);
  Init();
}

nuiSprite::nuiSprite(nuiSpriteDef* pSpriteDef)
: mpSpriteDef(pSpriteDef)
{
  NGL_ASSERT(mpSpriteDef);
  mpSpriteDef->Acquire();
  
  Init();
}


nuiSprite::~nuiSprite()
{
  // static counter
  mSpriteCounter--;
  
  if (mpSpriteDef)
    mpSpriteDef->Release();
}

void nuiSprite::Init()
{
  CheckValid();
  if (SetObjectClass(_T("nuiSprite")))
  {
    InitAttributes();
  }
  
  // static counter
  mSpriteCounter++;
  
  mCurrentAnimation = 0;
  mCurrentFrame = 0;
  mSpeed = 1.0f;
}

void nuiSprite::InitAttributes()
{
  AddAttribute(new nuiAttribute<const nglString&>
               (nglString(_T("Animation")), nuiUnitCustom,
                nuiMakeDelegate(this, &nuiSprite::GetCurrentAnimationName),
                nuiMakeDelegate(this, &nuiSprite::_SetAnimation)));
  
  AddAttribute(new nuiAttribute<float>
               (nglString(_T("FrameTime")), nuiUnitCustom,
                nuiMakeDelegate(this, &nuiSprite::GetSpeed),
                nuiMakeDelegate(this, &nuiSprite::SetSpeed)));
  
  AddAttribute(new nuiAttribute<float>
               (nglString(_T("FrameTime")), nuiUnitCustom,
                nuiMakeDelegate(this, &nuiSprite::GetFrameTime),
                nuiMakeDelegate(this, &nuiSprite::SetFrameTime)));
}


const nuiSpriteDef* nuiSprite::GetDefinition() const
{
  CheckValid();
  return mpSpriteDef;
}


void nuiSprite::Draw(nuiDrawContext* pContext)
{
  CheckValid();

  const nuiSpriteAnimation* pAnim = mpSpriteDef->GetAnimation(mCurrentAnimation);
  const nuiSpriteFrame* pFrame = pAnim->GetFrame(ToBelow(mCurrentFrame));
  nuiRect src(pFrame->GetRect());
  nuiRect dst(src);
  dst.Move(-pFrame->GetHandleX(), -pFrame->GetHandleY());
  
  nuiTexture* pTex = pFrame->GetTexture();
  pContext->SetTexture(pFrame->GetTexture());
  
  pContext->DrawImage(dst, src);
}

void nuiSprite::SetAnimation(const nglString& rAnimationName)
{
  CheckValid();
  SetAnimation(mpSpriteDef->GetAnimation(rAnimationName));
}

void nuiSprite::SetAnimation(int32 index)
{
  CheckValid();
  NGL_ASSERT(index < mpSpriteDef->GetAnimationCount());
  mCurrentAnimation = index;
  mCurrentFrame = 0;
}

void nuiSprite::_SetAnimation(const nglString& rAnimationName)
{
  CheckValid();
  SetAnimation(rAnimationName);
}



const nglString& nuiSprite::GetCurrentAnimationName() const
{
  CheckValid();
  const nuiSpriteAnimation* pAnim = mpSpriteDef->GetAnimation(mCurrentAnimation);
  if (!pAnim)
    return nglString::Null;
  return pAnim->GetName();
}

void nuiSprite::SetFrameTime(float framepos)
{
  CheckValid();
  NGL_ASSERT(mCurrentFrame < mpSpriteDef->GetAnimation(mCurrentAnimation)->GetFrameCount());
  mCurrentFrame = framepos;
}

float nuiSprite::GetFrameTime() const
{
  CheckValid();
  return mCurrentFrame;
}


void nuiSprite::Animate(float passedtime)
{
  CheckValid();
  const nuiSpriteAnimation* pAnim = mpSpriteDef->GetAnimation(mCurrentAnimation);
  float fps = pAnim->GetFPS();
  NGL_ASSERT(fps != 0);
  mCurrentFrame += passedtime * mSpeed * fps;
  float count = (float)pAnim->GetFrameCount();
  

  bool done = false;
  while (mCurrentFrame >= count)
  {
    // send event
    done = true;
    mCurrentFrame -= count;
  }
  if (done)
    AnimEnd();
  
  for (size_t i = 0; i < mpChildren.size(); i++)
    ((nuiSprite*)mpChildren[i])->Animate(passedtime);
}

float nuiSprite::GetSpeed() const
{
  CheckValid();
  return mSpeed;
}

void nuiSprite::SetSpeed(float speed)
{
  CheckValid();
  mSpeed = speed;
}


bool nuiSprite::IsInside(float x, float y) const
{
  const nuiSpriteAnimation* pAnim = mpSpriteDef->GetAnimation(mCurrentAnimation);
  const nuiSpriteFrame* pFrame = pAnim->GetFrame(ToBelow(mCurrentFrame));
  nuiRect dst(pFrame->GetRect());
  dst.Move(-pFrame->GetHandleX(), -pFrame->GetHandleY());
  
  return dst.IsInside(x, y);
}


/////////////////////////////////////////////
// class nuiSpriteView : public nuiWidget
nuiSpriteView::nuiSpriteView()
{
  mLastTime = 0;
  if (SetObjectClass(_T("nuiSpriteView")))
  {
    // Init attributes
  }
  
  StartAutoDraw();
}

nuiSpriteView::~nuiSpriteView()
{
  for (size_t i = 0; i < mpSprites.size(); i++)
    mpSprites[i]->Release();
}

void nuiSpriteView::AddSprite(nuiSprite* pSprite)
{
  mpSprites.push_back(pSprite);
  pSprite->Acquire();
}

void nuiSpriteView::DelSprite(nuiSprite* pSprite)
{
  for (size_t i = 0; i < mpSprites.size(); i++)
  {
    if (mpSprites[i] == pSprite)
    {
      mpSprites[i]->Release();
      mpSprites.erase(mpSprites.begin() + i);
      return;
    }
  }
}
  
nuiRect nuiSpriteView::CalcIdealRect()
{
  return nuiRect(320, 200);
}

bool nuiSpriteView::Draw(nuiDrawContext* pContext)
{
  //printf("%p - draw %d sprites\n", this, mpSprites.size());

  nglTime now;
  double t = 0;
  if (mLastTime != 0)
    t = now - mLastTime;
  mLastTime = now;

  for (size_t i = 0; i < mpSprites.size(); i++)
  {
    mpSprites[i]->Animate((float)t);
    mpSprites[i]->Draw(pContext);
    
  }

  return true;
}

void nuiSpriteView::GetSpritesAtPoint(float x, float y, std::vector<nuiSprite*>& rSprites)
{
  uint32 s = mpSprites.size();
  for (size_t i = 0; i < s; i++)
  {
    mpSprites[i]->GetNodesAtPoint(x, y, *(std::vector<nuiNode*>*)&rSprites);
  }
}

bool nuiSpriteView::MouseClicked(const nglMouseInfo& rEvent)
{
  std::vector<nuiSprite*> Sprites;
  GetSpritesAtPoint(rEvent.X, rEvent.Y, Sprites);
  for (uint32 i = 0; i < Sprites.size(); i++)
  {
    Sprites[i]->MouseClicked(Sprites[i], rEvent);
  }
  
  return false;
}

bool nuiSpriteView::MouseUnclicked(const nglMouseInfo& rEvent)
{
  std::vector<nuiSprite*> Sprites;
  GetSpritesAtPoint(rEvent.X, rEvent.Y, Sprites);
  for (uint32 i = 0; i < Sprites.size(); i++)
  {
    Sprites[i]->MouseUnclicked(Sprites[i], rEvent);
  }
  
  return false;
}

bool nuiSpriteView::MouseMoved(const nglMouseInfo& rEvent)
{
  std::vector<nuiSprite*> Sprites;
  GetSpritesAtPoint(rEvent.X, rEvent.Y, Sprites);
  for (uint32 i = 0; i < Sprites.size(); i++)
  {
    Sprites[i]->MouseMoved(Sprites[i], rEvent);
  }
  
  return false;
}

const std::vector<nuiSprite*>& nuiSpriteView::GetSprites() const
{
  return mpSprites;
}

int32 nuiSpriteView::GetSpriteCount() const
{
  return mpSprites.size();
}
