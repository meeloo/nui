 /*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"


nuiTextLine::nuiTextLine(const nuiTextLayout& rLayout, float X, float Y)
: mLayout(rLayout), mX(X), mY(Y)
{
}

nuiTextLine::~nuiTextLine()
{
  for (uint32 i = 0; i < mpRuns.size(); i++)
    mpRuns[i]->Release();
}

const std::vector<nuiTextRun*>& nuiTextLine::GetGlyphRuns() const
{
  return mpRuns;
}

float nuiTextLine::GetX() const
{
  return mX;
}

float nuiTextLine::GetY() const
{
  return mY;
}

void nuiTextLine::SetPosition(float X, float Y)
{
  mX = X;
  mY = Y;
}

void nuiTextLine::AddRun(nuiTextRun* pRun)
{
  pRun->Acquire();
  mpRuns.push_back(pRun);
}

int32 nuiTextLine::GetRunCount() const
{
  return (int32)mpRuns.size();
}

nuiTextRun* nuiTextLine::GetRun(int32 index) const
{
  return mpRuns[index];
}

float nuiTextLine::GetAdvanceY() const
{
  float y = 0;
  for (uint32 i = 0; i < mpRuns.size(); i++)
  {
    y = MAX(y, mpRuns[i]->GetAdvanceY());
  }
  
  return y;
}

float nuiTextLine::GetAdvanceX() const
{
  float x = 0;
  for (uint32 i = 0; i < mpRuns.size(); i++)
  {
    x += mpRuns[i]->GetAdvanceX();
  }
  
  return x;
}

float nuiTextLine::GetAscender() const
{
  float v = 0;
  for (uint32 i = 0; i < mpRuns.size(); i++)
  {
    v = MAX(v, mpRuns[i]->GetAscender());
  }
  
  return v;
}

float nuiTextLine::GetDescender() const
{
  float v = 0;
  for (uint32 i = 0; i < mpRuns.size(); i++)
  {
    v = MIN(v, mpRuns[i]->GetAscender());
  }
  
  return v;
}

int32 nuiTextLine::GetGlyphCount() const
{
  int32 count = 0;
  for (uint32 i = 0; i < mpRuns.size(); i++)
  {
    count += mpRuns[i]->GetGlyphCount();
  }
  
  return count;
}

const nuiTextGlyph* nuiTextLine::GetGlyph(int32 Offset) const
{
  for (uint32 i = 0; i < mpRuns.size(); i++)
  {
    int32 s = mpRuns[i]->GetGlyphCount();
    if (Offset < s)
      return mpRuns[i]->GetGlyph(Offset);
    Offset -= s;
  }
  
  return NULL;
}

const nuiTextGlyph* nuiTextLine::GetGlyphAt(float X, float Y) const
{
  X -= mX;
  Y -= mY;
  
  for (uint32 i = 0; i < mpRuns.size(); i++)
  {
    const nuiTextGlyph* pGlyph = mpRuns[i]->GetGlyphAt(X, Y);
    if (pGlyph)
      return pGlyph;
  }
  
  return NULL;
}

float LayoutDirect(float PenX, float PenY, float globalwidth, float sublinewidth, float spacewidth, std::list<nuiTextRun*>& subline, nuiRect& globalrect)
{
  float h = 0;
  float x = 0;
  float y = 0;
  for (nuiTextRun* pRun : subline)
  {
    nuiFontBase* pFont = pRun->GetFont();

    nuiFontInfo finfo;

    if (pFont)
      pFont->GetInfo(finfo);

    std::vector<nuiTextGlyph>& rGlyphs(pRun->GetGlyphs());

    // Prepare glyphs:
    for (int32 g = 0; g < rGlyphs.size(); g++)
    {
      nuiTextGlyph& rGlyph(rGlyphs.at(g));

      if (pFont)
      {
        pFont->PrepareGlyph(PenX + x, PenY + y, rGlyph);

        const nuiSize W = rGlyph.AdvanceX;
        const nuiSize X = rGlyph.mX + rGlyph.BearingX;
        const nuiSize Y = rGlyph.mY - finfo.Ascender;
        const nuiSize H = finfo.Height;

        h = MAX(h, H);
        nuiRect rr(globalrect);
        globalrect.Union(rr, nuiRect(PenX + x + X, PenY + y + Y, W, H));
      }
    }

    x += pRun->GetAdvanceX();
  }

  return h;
}

float LayoutJustify(float PenX, float PenY, float globalwidth, float sublinewidth, float spacewidth, std::list<nuiTextRun*>& subline, nuiRect& globalrect)
{
  float h = 0;
  float x = 0;
  float y = 0;

  float ratio = (globalwidth - (sublinewidth - spacewidth)) / spacewidth;
  for (nuiTextRun* pRun : subline)
  {
    nuiFontBase* pFont = pRun->GetFont();

    nuiFontInfo finfo;
    if (pFont)
      pFont->GetInfo(finfo);

    std::vector<nuiTextGlyph>& rGlyphs(pRun->GetGlyphs());

    // Prepare glyphs:
    for (int32 g = 0; g < rGlyphs.size(); g++)
    {
      nuiTextGlyph& rGlyph(rGlyphs.at(g));

      if (pFont)
      {
        pFont->PrepareGlyph(PenX + x, PenY + y, rGlyph);

        const nuiSize W = rGlyph.AdvanceX;
        const nuiSize X = rGlyph.mX + rGlyph.BearingX;
        const nuiSize Y = rGlyph.mY - finfo.Ascender;
        const nuiSize H = finfo.Height;

        h = MAX(h, H);
        nuiRect rr(globalrect);
        globalrect.Union(rr, nuiRect(PenX + x + X, PenY + y + Y, W, H));
      }
    }

    if (pRun->IsDummy())
    {
      x += pRun->GetAdvanceX() * ratio;
    }
    else
    {
      x += pRun->GetAdvanceX();
    }
  }
  
  return h;
}


float LayoutLeft(float PenX, float PenY, float globalwidth, float sublinewidth, float spacewidth, std::list<nuiTextRun*>& subline, nuiRect& globalrect)
{
  return LayoutDirect(PenX, PenY, globalwidth, sublinewidth, spacewidth, subline, globalrect);
}


float LayoutRight(float PenX, float PenY, float globalwidth, float sublinewidth, float spacewidth, std::list<nuiTextRun*>& subline, nuiRect& globalrect)
{
  float x = globalwidth - sublinewidth;
  return LayoutDirect(PenX + x, PenY, globalwidth, sublinewidth, spacewidth, subline, globalrect);
}

float LayoutCenter(float PenX, float PenY, float globalwidth, float sublinewidth, float spacewidth, std::list<nuiTextRun*>& subline, nuiRect& globalrect)
{
  float x = (globalwidth - sublinewidth) / 2;
  return LayoutDirect(PenX + x, PenY, globalwidth, sublinewidth, spacewidth, subline, globalrect);
}



float nuiTextLine::Layout(float PenX, float PenY, float width, nuiRect& globalrect)
{
  SetPosition(PenX, PenY);

  PenX = 0;
  float x = 0;
  float y = 0;

  std::vector< std::list <nuiTextRun*> > sublines;
  sublines.resize(1);
  bool sublinestart = true;
  // Prepare sublines:
  for (uint32 r = 0; r < GetRunCount(); r++)
  {
    nuiTextRun* pRun = GetRun(r);
    if (pRun->IsWrapStart())
      sublines.resize(sublines.size() + 1);

    sublines.back().push_back(pRun);
  }

  // Trim the dummy runs:
  for (int l = 0; l < sublines.size(); l++)
  {
    // Remove dummy text runs from the start of the lines (except the first line):
    bool skip = false;
    if (!sublines[l].empty() && sublines[l].front()->GetStyle().GetMode() == nuiTextLayoutJustify && l == 0)
      skip = true;

    if (!skip)
    {
      while (!sublines.empty() && !sublines[l].empty() && sublines[l].front()->IsDummy())
        sublines[l].pop_front();
    }

    // Remove dummy text runs from the end of the lines:
    while (!sublines.empty() && !sublines[l].empty() && sublines[l].back()->IsDummy())
      sublines[l].pop_back();
  }


  // Now position each run inside each subline:
  for (int l = 0; l < sublines.size(); l++)
  {
    float w = 0;
    float space = 0;
    for (nuiTextRun* pRun : sublines[l])
    {
      float a = pRun->GetAdvanceX();
      w += a;
      if (pRun->IsDummy())
        space += a;
    }

    float h = 0;
    if (!sublines[l].empty())
    {
      switch (sublines[l].front()->GetStyle().GetMode())
      {
        case nuiTextLayoutLeft:
          h = LayoutLeft(PenX, PenY, width, w, space, sublines[l], globalrect);
          break;

        case nuiTextLayoutRight:
          h = LayoutRight(PenX, PenY, width, w, space, sublines[l], globalrect);
          break;

        case nuiTextLayoutJustify:
          h = LayoutJustify(PenX, PenY, width, w, space, sublines[l], globalrect);
          break;

        case nuiTextLayoutCenter:
          h = LayoutCenter(PenX, PenY, width, w, space, sublines[l], globalrect);
          break;
      }
    }

    PenY += h;
  }

  return PenY;
}


