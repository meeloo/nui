/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#pragma once

#include "nuiTextStyle.h"
#include "nuiUnicode.h"

class nuiTextLayout;

//! Glyph metrics
/*!
 All glyph metrics are given in pixels, they depend on the current font size and
 resolution. See :
 
 - nuiFontBase::GetGlyphInfo() for more informations
 - the insightfull introduction to digital typography on FreeType's site :
 http://www.freetype.org/freetype2/docs/glyphs/
 - the font/metrics example bundled with the regular NGL distribution
 */
class nuiGlyphInfo
{
public:
  int   Index;     ///< Glyph index in font face
  float Width;     ///< Bounding box width
  float Height;    ///< Bounding box height
  float BearingX;  ///< X bearing
  float BearingY;  ///< Y bearing
  float AdvanceX;  ///< Horizontal advance
  float AdvanceY;  ///< Vertical advance
  
  nuiGlyphInfo();
  virtual ~nuiGlyphInfo();
  void Dump (uint Level = 0) const;  ///< Dumps informations to the application log using \p Level verbosity
};

class nuiTextGlyph : public nuiGlyphInfo
{
public:
  size_t mCluster; ///< Position of the source char in the original unicode code point vector
  float mX, mY; ///< Position on screen
  nuiTexture* mpTexture; ///< Texture that contains this glyph
  nuiRect mDestRect; ///< Destination rectangle of the laid ou glyph
  nuiRect mSourceRect; ///< Rectangle of the glyph in the texture
  nuiColor mColor;
  bool mUseColor;
};



class nuiTextRun : public nuiRefCount
{
public:
  nuiTextRun(const nuiTextLayout& rLayout, nuiUnicodeScript Script, size_t Position, size_t Length, const nuiTextStyle& rStyle);
  nuiTextRun(const nuiTextLayout& rLayout, size_t Position, size_t Length, float AdvanceX, float AdvanceY, const nuiTextStyle& rStyle); ///< Create a blank (dummy) layout
  void SetFont(nuiFontBase* pFont);
  
  nuiUnicodeScript GetScript() const;
  size_t GetPosition() const;
  const nglUChar* GetUnicodeChars() const;
  size_t GetLength() const;
  
  float GetAdvanceX() const;
  float GetAdvanceY() const;
  
  float GetAscender() const;
  float GetDescender() const;
  
  std::vector<nuiTextGlyph>& GetGlyphs();
  const nuiTextGlyph* GetGlyph   (size_t Offset) const;
  const nuiTextGlyph* GetGlyphAt (float X, float Y) const;

  size_t GetGlyphCount() const;
  nuiFontBase* GetFont() const;
  
  void SetUnderline(bool set);
  bool GetUnderline() const;
  void SetStrikeThrough(bool set);
  bool GetStrikeThrough() const;
  
  nuiRect GetRect() const;
  
  bool IsDummy() const;
  bool IsWrapStart() const;
  void SetWrapStart(bool set);

  const nuiTextStyle& GetStyle() const;

protected:
  virtual ~nuiTextRun();

private:
  friend class nuiTextLayout;
  friend class nuiFontBase;
  nuiTextStyle mStyle;
  const nuiTextLayout& mLayout;
  size_t mPosition;
  size_t mLength;
  nuiUnicodeScript mScript;
  
  bool mUnderline : 1;
  bool mStrikeThrough : 1;
  bool mDummy : 1;
  bool mWrapStart : 1;
  
  std::vector<nuiTextGlyph> mGlyphs;
  float mX;
  float mY;
  float mAdvanceX;
  float mAdvanceY;
};

