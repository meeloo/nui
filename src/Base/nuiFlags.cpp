/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

// Get enum from string desc:
nuiPosition nuiGetPosition (const nglString& Position, nuiPosition Default)
{
  if (!Position.Compare("Left",false))
    return nuiLeft;
  else if (!Position.Compare("Right",false))
    return nuiRight;
  else if (!Position.Compare("Top",false))
    return nuiTop;
  else if (!Position.Compare("Bottom",false))
    return nuiBottom;
  else if (!Position.Compare("TopLeft",false))
    return nuiTopLeft;
  else if (!Position.Compare("TopRight",false))
    return nuiTopRight;
  else if (!Position.Compare("BottomLeft",false))
    return nuiBottomLeft;
  else if (!Position.Compare("BottomRight",false))
    return nuiBottomRight;
  else if (!Position.Compare("Center",false))
    return nuiCenter;
  else if (!Position.Compare("Tile",false))
    return nuiTile;
  else if (!Position.Compare("Fill",false))
    return nuiFill;
  else if (!Position.Compare("FillHorizontal",false))
    return nuiFillHorizontal;
  else if (!Position.Compare("FillVertical",false))
    return nuiFillVertical;
  else if (!Position.Compare("FillLeft",false))
    return nuiFillLeft;
  else if (!Position.Compare("FillRight",false))
    return nuiFillRight;
  else if (!Position.Compare("FillTop",false))
    return nuiFillTop;
  else if (!Position.Compare("FillBottom",false))
    return nuiFillBottom;
  else if (!Position.Compare("None",false))
    return nuiNoPosition;

  return Default;
}

nglString nuiGetPosition (nuiPosition Value)
{
  switch (Value)
  {
  case nuiLeft:
    return "Left";
  case nuiRight:
    return "Right";
  case nuiTop:
    return "Top";
  case nuiBottom:
    return "Bottom";
  case nuiFillLeft:
    return "FillLeft";
  case nuiFillRight:
    return "FillRight";
  case nuiFillTop:
    return "FillTop";
  case nuiFillBottom:
    return "FillBottom";
  case nuiTopLeft:
    return "TopLeft";
  case nuiTopRight:
    return "TopRight";
  case nuiBottomLeft:
    return "BottomLeft";
  case nuiBottomRight:
    return "BottomRight";
  case nuiCenter:
    return "Center";
  case nuiTile:
    return "Tile";
  case nuiFill:
    return "Fill";
  case nuiFillHorizontal:
    return "FillHorizontal";
  case nuiFillVertical:
    return "FillVertical";
  default:
    return "None"; // Default?
  }
}

nuiDirection nuiGetDirection (const nglString& Direction, nuiDirection Default)
{
  if (!Direction.Compare("Forward",false))
    return nuiForward;
  else if (!Direction.Compare("Backward",false))
    return nuiBackward;
  return Default;
}

nglString nuiGetDirection (nuiDirection Value)
{
  switch (Value)
  {
  case nuiForward:
    return "Forward";
  case nuiBackward:
    return "Backward";
  default:
    return "Forward";
  }
}

nuiAlignment nuiGetAlignment (const nglString& Alignment, nuiAlignment Default)
{
  if (!Alignment.Compare("Begin",false))
    return eBegin;
  else if (!Alignment.Compare("End",false))
    return eEnd;
  else if (!Alignment.Compare("Center",false))
    return eCenter;
  else if (!Alignment.Compare("Justify",false))
    return eJustify;
  return Default;
}

nglString nuiGetAlignment (nuiAlignment Value)
{
  switch (Value)
  {
    case eBegin:
      return "Begin";
    case eEnd:
      return "End";
    case eCenter:
      return "Center";
    case eJustify:
      return "Justify";
    default:
      return "Begin";
  }
}

nuiOrientation nuiGetOrientation (const nglString& Orientation, nuiOrientation Default)
{
  if (!Orientation.Compare("Horizontal",false))
    return nuiHorizontal;
  else if (!Orientation.Compare("Vertical",false))
    return nuiVertical;
  return Default;
}
  
nglString nuiGetOrientation (nuiOrientation Value)
{
  switch (Value)
  {
  case nuiHorizontal:
    return "Horizontal";
  case nuiVertical:
  default:
    return "Vertical";
  }
}
  
bool nuiGetBool (const nglString& rBool, bool Default)
{
  if (!rBool.Compare("true",false))
    return true;
  else if (!rBool.Compare("false",false))
    return false;

  return Default;
}

const nglString& nuiGetEasingPresetName(nuiEasingPreset preset)
{
  static nglString names[] =
  {
    "BounceOut",
    "BounceIn",
    "BackIn",
    "BackOut",
    "Identity",
    "Square",
    "Cubic",
    "Quartic",
    "SlowStart",
    "Quintic",
    "SinusStartFast",
    "SinusStartSlow",
    "Sinus",
    "SquareRev",
    "CubicRev",
    "QuarticRev",
    "SlowStartRev",
    "QuinticRev",
    "SinusStartSlowRev",
    "SinusStartFastRev",
    "SinusRev",
    "ElasticIn",
    "ElasticOut",
  };

  NGL_ASSERT(preset < nuiEasingPresetLast);
  return names[preset];
}

nuiEasingPreset nuiGetEasingPresetForName(const nglString& name)
{
  static std::map<nglString, nuiEasingPreset> names =
  {
    { "BounceOut", nuiEasingPresetBounceOut },
    { "BounceIn", nuiEasingPresetBounceIn },
    { "BackIn", nuiEasingPresetBackIn },
    { "BackOut", nuiEasingPresetBackOut },
    { "Identity", nuiEasingPresetIdentity },
    { "Square", nuiEasingPresetSquare },
    { "Cubic", nuiEasingPresetCubic },
    { "Quartic", nuiEasingPresetQuartic },
    { "SlowStart", nuiEasingPresetSlowStart },
    { "Quintic", nuiEasingPresetQuintic },
    { "SinusStartFast", nuiEasingPresetSinusStartFast },
    { "SinusStartSlow", nuiEasingPresetSinusStartSlow },
    { "Sinus", nuiEasingPresetSinus },
    { "SquareRev", nuiEasingPresetSquareRev },
    { "CubicRev", nuiEasingPresetCubicRev },
    { "QuarticRev", nuiEasingPresetQuarticRev },
    { "SlowStartRev", nuiEasingPresetSlowStartRev },
    { "QuinticRev", nuiEasingPresetQuinticRev },
    { "SinusStartSlowRev", nuiEasingPresetSinusStartSlowRev },
    { "SinusStartFastRev", nuiEasingPresetSinusStartFastRev },
    { "SinusRev", nuiEasingPresetSinusRev },
    { "ElasticIn", nuiEasingPresetElasticIn },
    { "ElasticOut", nuiEasingPresetElasticOut },
  };

  auto it = names.find(name);
  if (it == names.end())
    return nuiEasingPresetLast;
  return it->second;
}



// Get enum from XML desc:
nuiPosition nuiGetPosition (const nuiXMLNode* pNode, const nglString& Attr, nuiPosition Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return nuiGetPosition(pNode->GetAttribute(Attr), Default);
}

nuiDirection nuiGetDirection (const nuiXMLNode* pNode, const nglString& Attr, nuiDirection Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return nuiGetDirection(pNode->GetAttribute(Attr), Default);
}

nuiOrientation nuiGetOrientation (const nuiXMLNode* pNode, const nglString& Attr, nuiOrientation Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return nuiGetOrientation(pNode->GetAttribute(Attr), Default);
}

nuiAlignment nuiGetAlignment (const nuiXMLNode* pNode, const nglString& Attr, nuiAlignment Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return nuiGetAlignment(pNode->GetAttribute(Attr), Default);
}

// Get enum from standard xml attrib:
nuiPosition nuiGetPosition (const nuiXMLNode* pNode, nuiPosition Default)
{
  if (!pNode->HasAttribute("Position"))
    return Default;
  return nuiGetPosition(pNode->GetAttribute("Position"),Default);
}

nuiDirection nuiGetDirection (const nuiXMLNode* pNode, nuiDirection Default)
{
  if (!pNode->HasAttribute("Direction"))
    return Default;
  return nuiGetDirection(pNode->GetAttribute("Direction"),Default);
}

nuiOrientation nuiGetOrientation (const nuiXMLNode* pNode, nuiOrientation Default)
{
  if (!pNode->HasAttribute("Orientation"))
    return Default;
  return nuiGetOrientation(pNode->GetAttribute("Orientation"),Default);
}

nuiAlignment nuiGetAlignment (const nuiXMLNode* pNode, nuiAlignment Default)
{
  if (!pNode->HasAttribute("Alignment"))
    return Default;
  return nuiGetAlignment(pNode->GetAttribute("Alignment"), Default);
}

// Values helpers:
bool nuiGetBool (const nuiXMLNode* pNode, const nglString& Attr, bool Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return nuiGetBool(pNode->GetAttribute(Attr),Default);
}

const nglString& nuiGetString (const nuiXMLNode* pNode, const nglString& Attr, const nglString& Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return pNode->GetAttribute(Attr);
}


int nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, int Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return pNode->GetAttribute(Attr).GetCInt();
}


int64 nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, int64 Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return pNode->GetAttribute(Attr).GetCInt();
}


uint nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, uint Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return pNode->GetAttribute(Attr).GetCInt();
}

uint64 nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, uint64 Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return pNode->GetAttribute(Attr).GetCInt64();
}

float nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, float Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return pNode->GetAttribute(Attr).GetCFloat();
}

double nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, double Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return pNode->GetAttribute(Attr).GetCDouble();
}

nglString nuiGetVal (const nuiXMLNode* pNode, const nglString& Attr, const nglString& Default)
{
  if (!pNode->HasAttribute(Attr))
    return Default;
  return pNode->GetAttribute(Attr);
}

