/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"

/////////////
nuiTextRun::nuiTextRun(const nuiTextLayout& rLayout, nuiUnicodeScript script, size_t Position, size_t Length, const nuiTextStyle& rStyle)
: mLayout(rLayout),
  mPosition(Position),
  mLength(Length),
  mScript(script),
  mX(0),
  mY(0),
  mAdvanceX(0),
  mAdvanceY(0),
  mUnderline(false),
  mStrikeThrough(false),
  mDummy(false),
  mWrapStart(false),
  mStyle(rStyle)
{
}

nuiTextRun::nuiTextRun(const nuiTextLayout& rLayout, size_t Position, size_t Length, float AdvanceX, float AdvanceY, const nuiTextStyle& rStyle)
: mLayout(rLayout),
  mPosition(Position),
  mLength(Length),
  mScript(eScriptCommon),
  mX(0),
  mY(0),
  mAdvanceX(AdvanceX),
  mAdvanceY(AdvanceY),
  mUnderline(false),
  mStrikeThrough(false),
  mDummy(true), 
  mWrapStart(false),
  mStyle(rStyle)
{
}

nuiTextRun::~nuiTextRun()
{
}

std::vector<nuiTextGlyph>& nuiTextRun::GetGlyphs()
{
  return mGlyphs;
}

void nuiTextRun::SetFont(nuiFontBase* pFont)
{
  mStyle.SetFont(pFont);
  
  nuiFontInfo info;
  pFont->GetInfo(info);
  mAdvanceY = info.AdvanceMaxH;
}

nuiUnicodeScript nuiTextRun::GetScript() const
{
  return mScript;
}

nuiFontBase* nuiTextRun::GetFont() const
{
  return mStyle.GetFont();
}


size_t nuiTextRun::GetPosition() const
{
  return mPosition;
}

size_t nuiTextRun::GetLength() const
{
  return mLength;
}

float nuiTextRun::GetAdvanceX() const
{
  return mAdvanceX;
}

float nuiTextRun::GetAdvanceY() const
{
  return mAdvanceY;
}

float nuiTextRun::GetAscender() const
{
  if (IsDummy())
    return 0;
  return mStyle.GetFont()->GetAscender();
}

float nuiTextRun::GetDescender() const
{
  if (IsDummy())
    return 0;
  return mStyle.GetFont()->GetDescender();
}



const nglUChar* nuiTextRun::GetUnicodeChars() const
{
  return mLayout.GetUnicodeChars() + mPosition;
}

void nuiTextRun::SetUnderline(bool set)
{
  mUnderline = set;
}

bool nuiTextRun::GetUnderline() const
{
  return mUnderline;
}

void nuiTextRun::SetStrikeThrough(bool set)
{
  mStrikeThrough = set;
}

bool nuiTextRun::GetStrikeThrough() const
{
  return mStrikeThrough;
}

nuiRect nuiTextRun::GetRect() const
{
  if (IsDummy())
    return nuiRect(mAdvanceX, mAdvanceY);
  nuiFontInfo finfo;
  mStyle.GetFont()->GetInfo(finfo);
 
//   const nuiTextGlyph& rGlyph(*it);
//   nuiGlyphInfo info;
//   rGlyph.mpFont->GetGlyphInfo(info, rGlyph.Index, nuiFontBase::eGlyphBitmap);
//   nuiSize w = info.AdvanceX;
//   //    nuiSize h = finfo.AdvanceMaxH;
//   nuiSize x = rGlyph.X + info.BearingX;
//   nuiSize y = rGlyph.Y - finfo.Ascender;
//   nuiSize h = finfo.Height;
// 
//   nuiRect rr(r);
//   r.Union(rr, nuiRect(x, y, w, h));
//   return r;
  return nuiRect();
}

size_t nuiTextRun::GetGlyphCount() const
{
  return mGlyphs.size();
}

const nuiTextGlyph* nuiTextRun::GetGlyph(size_t Offset) const
{
  return &(mGlyphs.at(Offset));
}

const nuiTextGlyph* nuiTextRun::GetGlyphAt (float X, float Y) const
{
  X -= mX;
  Y -= mY;
  
  for (size_t i = 0; i < mGlyphs.size(); i++)
  {
    const nuiTextGlyph* pGlyph = &mGlyphs[i];
    if (pGlyph->mDestRect.IsInside(X, Y))
      return pGlyph;
  }
  
  return NULL;
}

bool nuiTextRun::IsDummy() const
{
  return mDummy;
}

const nuiTextStyle& nuiTextRun::GetStyle() const
{
  return mStyle;
}

bool nuiTextRun::IsWrapStart() const
{
  return mWrapStart;
}

void nuiTextRun::SetWrapStart(bool set)
{
  mWrapStart = set;
}


