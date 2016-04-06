/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiFlags_h__
#define __nuiFlags_h__

//#include "nui.h"
#include "nglString.h"
class nuiXMLNode;

// Generic definitions that can be very useful for widgets:
enum nuiOrientation
{
  nuiHorizontal = 0,
  nuiVertical
};

enum nuiDirection
{
  nuiForward = 0,
  nuiBackward
};

enum nuiAlignment
{
  eBegin,
  eEnd,
  eCenter,
  eJustify
};

enum nuiPosition
{
  nuiNoPosition = 0,
  nuiLeft = 1,
  nuiRight = 2,
  nuiTop = 3,
  nuiBottom = 4,
  nuiTopLeft = 5,
  nuiTopRight = 6,
  nuiBottomLeft = 7,
  nuiBottomRight = 8,
  nuiCenter = 9,
  nuiTile = 10,  ///< This position is not handled by most widgets. 
  nuiFill = 11,
  nuiFillHorizontal = 12,
  nuiFillVertical = 13,
  nuiFillLeft = 14,
  nuiFillRight = 15,
  nuiFillTop = 16,
  nuiFillBottom = 17
};

enum nuiLineJoin
{
  nuiLineJoinMiter = 0,
  nuiLineJoinRound,
  nuiLineJoinBevel
};

enum nuiLineCap
{
  nuiLineCapBut = 0,
  nuiLineCapRound,
  nuiLineCapSquare
};


enum nuiCopyPolicy
{
  eClone=0,
  eReference,
  eTransfert
};


enum nuiExpandMode
{
  nuiExpandFixed,
  nuiExpandGrow,
  nuiExpandShrink,
  nuiExpandShrinkAndGrow
};

enum nuiTextDirection
{
  nuiLeftToRight,
  nuiRightToLeft,
  nuiTopToBottom,
  nuiBottomToTop
};

enum nuiTextBaseline
{
  nuiTextBaselineNormal,
  nuiTextBaselineSuperScript,
  nuiTextBaselineSubScript
};

enum nuiTextLayoutMode
{
  nuiTextLayoutJustify,
  nuiTextLayoutCenter,
  nuiTextLayoutLeft,
  nuiTextLayoutRight
};


enum nuiDrawPolicy
{
nuiDrawPolicyDrawNone     = 0, ///< Draw nothing
nuiDrawPolicyDrawSelf     = 1 << 0,  ///< draw itself but not its children
nuiDrawPolicyDrawChildren = 1 << 1,  ///< draw its children but not itself
nuiDrawPolicyDrawTree     = nuiDrawPolicyDrawSelf | nuiDrawPolicyDrawChildren ///< draw itself and its children
};

enum nuiEasingPreset
{
  nuiEasingPresetBounceOut = 0,
  nuiEasingPresetBounceIn,
  nuiEasingPresetBackIn,
  nuiEasingPresetBackOut,
  nuiEasingPresetIdentity,
  nuiEasingPresetSquare,
  nuiEasingPresetCubic,
  nuiEasingPresetQuartic,
  nuiEasingPresetSlowStart,
  nuiEasingPresetQuintic,
  nuiEasingPresetSinusStartFast,
  nuiEasingPresetSinusStartSlow,
  nuiEasingPresetSinus,
  nuiEasingPresetSquareRev,
  nuiEasingPresetCubicRev,
  nuiEasingPresetQuarticRev,
  nuiEasingPresetSlowStartRev,
  nuiEasingPresetQuinticRev,
  nuiEasingPresetSinusStartSlowRev,
  nuiEasingPresetSinusStartFastRev,
  nuiEasingPresetSinusRev,
  nuiEasingPresetElasticIn,
  nuiEasingPresetElasticOut,

  nuiEasingPresetLast
};



// This is just plain ugly; i know...
typedef uint nuiWindowFlags;



/// Useful conversion functions:
nuiPosition     nuiGetPosition    (const nglString& Position, nuiPosition Default = nuiNoPosition);
nuiDirection    nuiGetDirection   (const nglString& Direction, nuiDirection Default = nuiForward);
nuiOrientation  nuiGetOrientation (const nglString& Orientation, nuiOrientation Default = nuiVertical);
nuiAlignment    nuiGetAlignment   (const nglString& Alignment, nuiAlignment Default = eBegin);

nglString nuiGetPosition (nuiPosition Value);
nglString nuiGetDirection (nuiDirection Value);
nglString nuiGetOrientation (nuiOrientation Value);
nglString nuiGetAlignment (nuiAlignment Value);

bool nuiGetBool (const nglString& rBool, bool Default = false);

const nglString& nuiGetEasingPresetName(nuiEasingPreset preset);
nuiEasingPreset nuiGetEasingPresetForName(const nglString& name);


// Get enum from XML desc:
nuiPosition     nuiGetPosition    (const nuiXMLNode* pNode, const nglString& Attr, nuiPosition Default = nuiCenter);
nuiDirection    nuiGetDirection   (const nuiXMLNode* pNode, const nglString& Attr, nuiDirection Default = nuiForward);
nuiOrientation  nuiGetOrientation (const nuiXMLNode* pNode, const nglString& Attr, nuiOrientation Default = nuiVertical);
nuiAlignment    nuiGetAlignment   (const nuiXMLNode* pNode, const nglString& Attr, nuiAlignment Default = eBegin);

// Get enum from xml desc attrib with standard node:
nuiPosition     nuiGetPosition    (const nuiXMLNode* pNode, nuiPosition Default = nuiCenter);
nuiDirection    nuiGetDirection   (const nuiXMLNode* pNode, nuiDirection Default = nuiForward);
nuiOrientation  nuiGetOrientation (const nuiXMLNode* pNode, nuiOrientation Default = nuiVertical);
nuiAlignment  nuiGetAlignment     (const nuiXMLNode* pNode, nuiAlignment Default = eBegin);

bool nuiGetBool (const nuiXMLNode* pNode, const nglString& Attr, bool Default = false);
const nglString& nuiGetString (const nuiXMLNode* pNode, const nglString& Attr, const nglString& Default = nglString::Empty);

int     nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, int     Default = 0);
int64   nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, int64   Default = 0);
uint    nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, uint    Default = 0);
uint64  nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, uint64  Default = 0);
float   nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, float   Default = 0);
double  nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, double  Default = 0);
nglString nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, const nglString& Default = nglString::Empty);

template <class T>
bool nuiGet(const nuiXMLNode* pNode, const nglString& Attr, T& rVal)
{
  rVal = (T)nuiGetVal(pNode, Attr, rVal);
  return true;
}


#endif // __nuiFlags_h__
