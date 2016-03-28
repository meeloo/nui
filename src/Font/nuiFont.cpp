/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

#if 0
#define DEBUG_FORCE_DEFAULT_FONT return GetFont(11)
#define DEBUG_FORCE_DEFAULT_FONT_SIZE 11
#else
#define DEBUG_FORCE_DEFAULT_FONT
#endif

nuiFont* nuiFont::GetFont(const nglPath& rPath, nuiSize size, int face, const nglString& rID)
{
  DEBUG_FORCE_DEFAULT_FONT;

  NGL_ASSERT(size);
  nuiFont* pFont= NULL;
  nglString id = rID.IsEmpty() ? GetUniqueID(rPath.GetPathName(), size, face) : rID;
  NGL_ASSERT(!id.IsEmpty());
  
  //NGL_OUT("nuiFont::GetFont1 %s\n", id.GetChars());

  if (mpFonts.find(id) == mpFonts.end())
  {
    if (!(rPath.Exists() && rPath.IsLeaf()))
      return NULL;

    pFont = new nuiFont(rPath, size, face, id);
    
    if (!pFont || !pFont->IsValid())
    {
      delete pFont;
      return NULL;
    }

  }
  else
  {
    pFont = mpFonts[id];
    pFont->Acquire();
  }

  if (!pFont)
  {
    // there was an error loading the font...
    return GetFont(size);
    // Acquire is already called in this GetFont
  }

  return pFont;
}

nuiFont* nuiFont::GetFont(const nglString& rName, uint8* pBuffer, uint BufferSize, nuiSize Size, int Face, const nglString& rID)
{
  DEBUG_FORCE_DEFAULT_FONT;

  NGL_ASSERT(Size);
  nuiFont* pFont= NULL;
  nglString id = rID.IsEmpty() ? GetUniqueID(rName, Size, Face) : rID;
  NGL_ASSERT(!id.IsEmpty());

  //NGL_OUT("nuiFont::GetFont2 %s\n", id.GetChars());

  if (mpFonts.find(id) == mpFonts.end())
  {
    if (!pBuffer || !Size)
      return GetFont(12.0f);
    
    pFont = new nuiFont(rName, pBuffer, BufferSize, Size, Face, id);

    if (!pFont->IsValid())
    {
      delete pFont;
      return GetFont(Size);
    }
  }
  else
  {
    pFont = mpFonts[id];
    pFont->Acquire();
  }

  NGL_ASSERT(pFont);
  return pFont;
}

nuiFont* nuiFont::GetFont(nuiXMLNode* pNode, const nglString& rID)
{
  DEBUG_FORCE_DEFAULT_FONT;

  nuiFont* pFont= NULL;
  nglString id = rID.IsEmpty() ? GetUniqueID(nuiGetString(pNode, "Source"), nuiGetVal(pNode, "Size", 0.0f), nuiGetVal(pNode, "Face", 0)) : rID;
  NGL_ASSERT(!id.IsEmpty());

  //NGL_OUT("nuiFont::GetFont3 %s\n", id.GetChars());

  nglPath path(nuiGetString(pNode, "Source"));
  if (mpFonts.find(id) == mpFonts.end())
  {
    if (path.Exists() & path.IsLeaf())
    {
      pFont = new nuiFont(pNode, id);
      
      if (pFont && !pFont->IsValid())
      {
        delete pFont;
        pFont = NULL;
      }
    }
  }
  else
  {
    pFont = mpFonts[id];
    pFont->Acquire();
  }

  if (pFont)
  {
    return pFont;
  }

  // there was an error loading the font...
  float s = 10;
  if (pNode->HasAttribute("Size"))
    s = pNode->GetAttribute("Size").GetCFloat();

  return GetFont(s);
}

nuiFont* nuiFont::GetFont(const nglString& rId)
{
  DEBUG_FORCE_DEFAULT_FONT;

  //NGL_OUT("nuiFont::GetFont4 %s\n", rId.GetChars());

  std::map<nglString,nuiFont*, nglString::LessFunctor>::iterator it = mpFonts.find(rId);
  if (it == mpFonts.end())
    return NULL;

  nuiFont* pFont = it->second;
  if (pFont)
    pFont->Acquire();
  else
    return GetFont(12.0f);
  return pFont;
}

nuiFont* nuiFont::GetFont(nuiSize size, const nglString& rID)
{
#ifdef DEBUG_FORCE_DEFAULT_FONT_SIZE
  size = DEBUG_FORCE_DEFAULT_FONT_SIZE;
#endif

  NGL_ASSERT(size);
  nuiFont* pFont= NULL;
  int face = 0;
  nglString id = rID.IsEmpty()?GetUniqueID("NGL Default Font", size, face) : rID;
  NGL_ASSERT(!id.IsEmpty());

  //NGL_OUT("nuiFont::GetFont4 %s\n", id.GetChars());

  if (mpFonts.find(id) == mpFonts.end())
  { 
    pFont = new nuiFont(size, face, id);
    if (!pFont->IsValid())
    {
      delete pFont;
      pFont = NULL;
    }
  }
  else
  {
    pFont = mpFonts[id];
    pFont->Acquire();
  }

  NGL_ASSERT(pFont);
  return pFont;
}


nuiFont::nuiFont(const nglPath& rPath, nuiSize size, int face, const nglString& rID)
: nuiFontBase(rPath, face, size)
{
  NGL_ASSERT(size);
  NGL_ASSERT(!rID.IsEmpty());
  //  NGL_OUT("Creating nuiFont 0x%x.", this);
  SetObjectClass("nuiFont");
  SetProperty("FontID",rID);
  mPermanent = false;

  SetProperty("Source",rPath.GetPathName());
  nglString tmp;
  tmp.CFormat("%f",size);
  SetProperty("Size",tmp);
  tmp.CFormat("%d",face);
  SetProperty("Face",tmp);
  mpFonts[rID] = this;
  
  //SetSize(size);

  FontListChanged();
}

nuiFont::nuiFont(const nglString& rName, uint8* pBuffer, uint BufferSize, nuiSize Size, int Face, const nglString& rID)
: nuiFontBase(pBuffer, BufferSize, Face, false, Size)
{
//  NGL_OUT("Creating nuiFont 0x%x.", this);
  NGL_ASSERT(Size);
  NGL_ASSERT(!rID.IsEmpty());
  SetObjectClass("nuiFont");
  SetProperty("FontID",rID);
  mPermanent = false;

  SetProperty("Source",rName);
  nglString tmp;
  tmp.CFormat("%f",Size);
  SetProperty("Size",tmp);
  tmp.CFormat("%d",Face);
  SetProperty("Face",tmp);
  mpFonts[rID] = this;
  //SetSize(Size);

  FontListChanged();
}

nuiFont::nuiFont (nuiXMLNode* pNode, const nglString& rID)
: nuiFontBase(nglPath(nuiGetString(pNode, "Source")), 0)
{
  NGL_ASSERT(!rID.IsEmpty());
//  NGL_OUT("Creating nuiFont 0x%x.", this);
  nuiSize size = nuiGetVal(pNode, "Size", 0.0f);

  SetObjectClass("nuiFont");
  SetProperty("FontID",rID);
  mPermanent = false;
  SetPermanent(nuiGetBool(pNode, "Permanent", false));

  SetProperty("Source", nuiGetString(pNode, "Source"));
  SetProperty("Size", nuiGetString(pNode, "Size", "0"));
  SetProperty("Face", nuiGetString(pNode, "Face", "0"));
  mpFonts[rID] = this;
  SetSize(size);
  FontListChanged();
}

nuiFont::nuiFont (nuiSize size, int face, const nglString& rID)
: nuiFontBase(size)
{
  NGL_ASSERT(size);
  NGL_ASSERT(!rID.IsEmpty());
  //  NGL_OUT("Creating nuiFont 0x%x.", this);
  SetObjectClass("nuiFont");
  SetProperty("FontID",rID);
  mPermanent = false;

  SetProperty("Source", "NGL Default Font");
  nglString tmp;
  tmp.CFormat("%f",size);
  SetProperty("Size",tmp);
  tmp.CFormat("%d",face);
  SetProperty("Face",tmp);
  mpFonts[rID] = this;

  //SetSize(size);

  FontListChanged();
}



nuiFont::~nuiFont()
{
  mpFonts.erase(GetProperty("FontID"));
  FontListChanged();
}

bool nuiFont::Apply(nuiDrawContext* pContext)
{
  pContext->SetFont(this);
  return true;
}

nglString nuiFont::GetUniqueID(const nglString& rFace, nuiSize size, int face)
{
  nglString id = rFace;
  id.Add(" ").Add(face).Add(" ").Add(size);
  //id.CFormat("%f %d %s",size, face, rFace.GetChars());
  NGL_ASSERT(!id.IsEmpty());
  return id;
}

std::map<nglString,nuiFont*, nglString::LessFunctor> nuiFont::mpFonts;

void nuiFont::GetFontIDs(std::vector<nglString>& rFontIDs)
{
  rFontIDs.clear();
  std::map<nglString,nuiFont*,nglString::LessFunctor>::iterator it = mpFonts.begin();
  std::map<nglString,nuiFont*,nglString::LessFunctor>::iterator end = mpFonts.end();
  
  while (it != end)
  {
    rFontIDs.push_back(it->first);
    ++it;
  }
}

void nuiFont::GetFonts(std::vector<nuiFont*>& rFonts)
{
  rFonts.clear();
  std::map<nglString,nuiFont*,nglString::LessFunctor>::iterator it = mpFonts.begin();
  std::map<nglString,nuiFont*,nglString::LessFunctor>::iterator end = mpFonts.end();
  
  while (it != end)
  {
    rFonts.push_back(it->second);
    ++it;
  }
}

nuiSimpleEventSource<0> nuiFont::FontListChanged;

void nuiFont::ClearAll()
{
  std::map<nglString,nuiFont*,nglString::LessFunctor>::iterator it = mpFonts.begin();
  std::map<nglString,nuiFont*,nglString::LessFunctor>::iterator end = mpFonts.end();
  
  while (it != end)
  {
    nuiFont* pFont = it->second;
    int c = pFont->Release();
    it = mpFonts.begin();
  }
  
  mpFonts.clear();
}

