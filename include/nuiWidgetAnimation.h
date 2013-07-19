/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiWidgetAnimation_h__
#define __nuiWidgetAnimation_h__

//#include "nui.h"
#include "nuiWidget.h"
#include "nuiAnimation.h"

class nuiWidgetAnimation : public nuiAnimation
{
public:
  nuiWidgetAnimation(nuiWidgetPtr pTarget, double DurationInSeconds);
  virtual ~nuiWidgetAnimation();
protected:
  nuiWidgetPtr mpTarget;
};

class nuiMoveRectAnim : public nuiWidgetAnimation
{
public:
  nuiMoveRectAnim (nuiWidgetPtr pTarget, double DurationInSeconds, const nuiRect& rSource, const nuiRect& rDestination);
  virtual void OnFrame();
protected:
  nuiRect mSource;
  nuiRect mDestination;
};

class nuiMoveToRectAnim : public nuiMoveRectAnim
{
public:
  nuiMoveToRectAnim (nuiWidgetPtr pTarget, double DurationInSeconds, const nuiRect& rDestination);
  void Play(int32 Count = 1, nuiAnimLoop LoopMode = eAnimLoopForward);
};

class nuiMoveAnim : public nuiMoveRectAnim
{
public:
  nuiMoveAnim (nuiWidgetPtr pTarget, double DurationInSeconds, nuiSize X1, nuiSize Y1, nuiSize X2, nuiSize Y2);
  void Play(int32 Count = 1, nuiAnimLoop LoopMode = eAnimLoopForward);
  void OnFrame();
protected:
  nuiSize mX1,mY1;
  nuiSize mX2,mY2;
};

class nuiMoveToAnim : public nuiMoveAnim
{
public:
  nuiMoveToAnim (nuiWidgetPtr pTarget, double DurationInSeconds, nuiSize X, nuiSize Y);
  void Play(int32 Count = 1, nuiAnimLoop LoopMode = eAnimLoopForward);
};

class nuiScaleToWidgetAnim : public nuiMoveRectAnim
{
public:
  nuiScaleToWidgetAnim (nuiWidgetPtr pTarget, double DurationInSeconds, float Ratio, nuiPosition ScaleBarycenterPosition = nuiCenter);
  void Play(int32 Count = 1, nuiAnimLoop LoopMode = eAnimLoopForward);
protected:
  float mRatio;
  nuiPosition mPosition;
};

class nuiScaleFromWidgetAnim : public nuiMoveRectAnim
{
public:
  nuiScaleFromWidgetAnim (nuiWidgetPtr pTarget, double DurationInSeconds, float Ratio, nuiPosition ScaleBarycenterPosition = nuiCenter);
  virtual void Play(int32 Count = 1, nuiAnimLoop LoopMode = eAnimLoopForward);
protected:
  float mRatio;
  nuiPosition mPosition;
};

/////// WIDGET MATRIX ANIMATIONS:
class nuiScaleMatrixWidgetAnim : public nuiWidgetAnimation
{
public:
  nuiScaleMatrixWidgetAnim (nuiWidgetPtr pTarget, double DurationInSeconds, float SourceRatio, float DestinationRatio, nuiPosition ScaleBarycenterPosition = nuiCenter);
  virtual void Play(int32 Count = 1, nuiAnimLoop LoopMode = eAnimLoopForward);
  virtual void OnFrame();
protected:
  float mSourceRatio;
  float mDestinationRatio;
  nuiPosition mPosition;
  float mX;
  float mY;
};

class nuiScaleToMatrixWidgetAnim : public nuiScaleMatrixWidgetAnim
{
public:
  nuiScaleToMatrixWidgetAnim (nuiWidgetPtr pTarget, double DurationInSeconds, float DestinationRatio, nuiPosition ScaleBarycenterPosition = nuiCenter);
  virtual void Play(int32 Count = 1, nuiAnimLoop LoopMode = eAnimLoopForward);
};

class nuiScaleFromMatrixWidgetAnim : public nuiScaleMatrixWidgetAnim
{
public:
  nuiScaleFromMatrixWidgetAnim (nuiWidgetPtr pTarget, double DurationInSeconds, float DestinationRatio, nuiPosition ScaleBarycenterPosition = nuiCenter);
  virtual void Play(int32 Count = 1, nuiAnimLoop LoopMode = eAnimLoopForward);
};

/////// WIDGET ALPHA ANIMATIONS:
class nuiFadeWidgetAnim : public nuiWidgetAnimation
{
public:
  nuiFadeWidgetAnim (nuiWidgetPtr pTarget, double DurationInSeconds, float SourceAlpha, float DestinationAlpha);
  virtual void Play(int32 Count = 1, nuiAnimLoop LoopMode = eAnimLoopForward);
  virtual void OnFrame();
protected:
  float mSourceAlpha;
  float mDestinationAlpha;
  nuiPosition mPosition;
};

class nuiFadeInWidgetAnim : public nuiFadeWidgetAnim
{
public:
  nuiFadeInWidgetAnim (nuiWidgetPtr pTarget, double DurationInSeconds, bool StartFromCurrentAlpha);
  virtual void Play(int32 Count = 1, nuiAnimLoop LoopMode = eAnimLoopForward);

private:
  bool mStartFromCurrentAlpha;
};

class nuiFadeOutWidgetAnim : public nuiFadeWidgetAnim
{
public:
  nuiFadeOutWidgetAnim (nuiWidgetPtr pTarget, double DurationInSeconds, bool StartFromCurrentAlpha);
  virtual void Play(int32 Count = 1, nuiAnimLoop LoopMode = eAnimLoopForward);

private:
  bool mStartFromCurrentAlpha;
};

#endif // __nuiAnimation_h__
