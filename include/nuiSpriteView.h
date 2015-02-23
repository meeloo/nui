/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron
 
 licence: see nui3/LICENCE.TXT
 */

#pragma once

#include "nuiSignalsSlots.h"

class nuiTexture;

class nuiSpriteFrame
{
public:
  nuiSpriteFrame();
  virtual ~nuiSpriteFrame();
  
  bool SetTexture(nuiTexture* pTexture, const nuiRect& rRect);
  bool SetTexture(const nglPath& rPath, const nuiRect& rRect);
  bool SetTexture(const nglPath& rPath);

  void SetRect(const nuiRect& rRect);
  void SetHandle(float x, float y);
  
  nuiTexture* GetTexture() const;
  const nuiRect& GetRect() const;
  
  float GetHandleX() const;
  float GetHandleY() const;

protected:
  nuiTexture* mpTexture;
  nuiRect mRect; ///< Rectangle inside mpTexture occupied by the sprite pixels
  float mX, mY; ///< Handle position
};

class nuiSpriteAnimation
{
public:
  nuiSpriteAnimation();
  nuiSpriteAnimation(const nglPath& rPath);
  virtual ~nuiSpriteAnimation();
  
  int32 GetFrameCount() const;
  void AddFrame(nuiSpriteFrame* pFrame);
  const nuiSpriteFrame* GetFrame(int32 index) const;
  
  void SetDuration(float seconds);
  void SetFPS(float FPS);
  float GetDuration() const;
  float GetFPS() const;
  
  void SetName(const nglString& rName);
  const nglString& GetName() const;
  
protected:
  nglString mName;
  std::vector<nuiSpriteFrame*> mpFrames;
  float mFPS;
};

class nuiSpriteDef : public nuiObject
{
public:
  nuiSpriteDef(const nglString& rObjectName);
  nuiSpriteDef(const nglPath& rObjectPath);
  
  void AddAnimation(nuiSpriteAnimation* pAnim);
  int32 GetAnimationCount() const;
  const nuiSpriteAnimation* GetAnimation(int32 index) const;
  int32 GetAnimation(const nglString& rName) const;
  
  static nuiSpriteDef* GetSprite(const nglString& rSpriteDefName);
  
protected:
  virtual ~nuiSpriteDef();
  static std::map<nglString, nuiSpriteDef*> mSpriteMap;
  void Init();
  static void Uninit();
  std::vector<nuiSpriteAnimation*> mpAnimations;
};

class nuiSprite : public nuiNode
{
  friend class nuiSpriteDef;
  
public:
  nuiSprite(const nglString& rSpriteDefName);
  nuiSprite(const nglPath& rSpriteDefPath, bool forceReplace);
  nuiSprite(nuiSpriteDef* pSpriteDef);

  const nuiSpriteDef* GetDefinition() const;

  void Draw(nuiDrawContext* pContext);

  void Animate(float passedtime);

  void SetAnimation(const nglString& rAnimationName);
  void SetAnimation(int32 index);
  const nglString& GetCurrentAnimationName() const;

  void SetFrameTime(float framepos);
  float GetFrameTime() const;
  
  float GetSpeed() const;
  void SetSpeed(float speed); ///< Default is 1

  nuiSignal2<nuiSprite*, const nglMouseInfo&, bool> MouseClicked;
  nuiSignal2<nuiSprite*, const nglMouseInfo&, bool> MouseUnclicked;
  nuiSignal2<nuiSprite*, const nglMouseInfo&, bool> MouseMoved;
  
  nuiSimpleEventSource<0> AnimEnd;

  bool IsInside(float x, float y) const;

protected:
  virtual ~nuiSprite();
  
  void Init();
  void InitAttributes();

  nuiSpriteDef* mpSpriteDef;
  int32 mCurrentAnimation;
  float mCurrentFrame;
  float mSpeed;

private:
  
  void _SetAnimation(const nglString& rAnimationName);
  static uint32 mSpriteCounter;

};


class nuiSpriteView : public nuiWidget
{
public:
  nuiSpriteView();

  void AddSprite(nuiSprite* pSprite);
  void DelSprite(nuiSprite* pSprite);

  nuiRect CalcIdealRect();

  bool Draw(nuiDrawContext* pContext);

  void GetSpritesAtPoint(float x, float y, std::vector<nuiSprite*>& rSprites);
  const std::vector<nuiSprite*>& GetSprites() const;
  int32 GetSpriteCount() const;
  
  bool MouseClicked(const nglMouseInfo& rEvent);
  bool MouseUnclicked(const nglMouseInfo& rEvent);
  bool MouseMoved(const nglMouseInfo& rEvent);
  
protected:
  virtual ~nuiSpriteView();

  std::vector<nuiSprite*> mpSprites;
  double mLastTime;
};


