/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"
#include "nuiHTMLContext.h"

///////////class nuiHTMLContext
nuiHTMLContext::nuiHTMLContext()
{
  mMaxWidth = 640;
  
  mVSpace = 0;
  
  mpFontRequest->SetName(_T("arial"), 0.8f);
  mpFontRequest->SetGenericName(_T("sans-serif"), 0.8f);
  mpFontRequest->SetStyle(_T("regular"), 0.5f);
  mpFontRequest->SetProportionnal(1.0f);
  
  mpFontRequest->SetScalable(1.0f);
  mpFontRequest->MustHaveSize(12.0f, 1.0f);
  mpFontRequest->SetItalic(false, 1.0f);
  mpFontRequest->SetBold(false, 1.0f);
  mpFontRequest = NULL;
  mpFont = NULL;
  mUnderline = false;
  mStrikeThrough = false;
  mTextFgColor = nuiColor(0,0,0,255);
  mTextBgColor = nuiColor(0,0,0,0);
  mLinkColor = nuiColor(64, 64, 200);

  mAlignHorizontal = eBegin;
  mAlignVertical = eBegin;
  
  UpdateFont();
}

nuiHTMLContext::nuiHTMLContext(const nuiHTMLContext& rContext)
: mMaxWidth(rContext.mMaxWidth),
  mVSpace(rContext.mVSpace),
  mHSpace(rContext.mHSpace),
  mpFontRequest(rContext.mpFontRequest), 
  mpFont(rContext.mpFont),
  mUnderline(rContext.mUnderline),
  mStrikeThrough(rContext.mStrikeThrough),
  mTextFgColor(rContext.mTextFgColor),
  mTextBgColor(rContext.mTextBgColor),
  mLinkColor(rContext.mLinkColor),
  mAlignHorizontal(rContext.mAlignHorizontal),
  mAlignVertical(rContext.mAlignVertical),
  mpStyleSheets(rContext.mpStyleSheets)
{
  if (mpFontRequest)
    mpFontRequest->Acquire();
  if (mpFont)
    mpFont->Acquire();
}

nuiHTMLContext& nuiHTMLContext::operator=(const nuiHTMLContext& rContext)
{
  mMaxWidth = rContext.mMaxWidth;
  
  mVSpace = rContext.mVSpace;
  mHSpace = rContext.mHSpace;
  
  if (rContext.mpFontRequest)
    rContext.mpFontRequest->Acquire();
  if (mpFontRequest)
    mpFontRequest->Release();
  mpFontRequest = rContext.mpFontRequest;
  mpFont = rContext.mpFont;
  if (mpFont)
    mpFont->Acquire();
  
  mUnderline = rContext.mUnderline;
  mStrikeThrough = rContext.mStrikeThrough;
  mTextFgColor = rContext.mTextFgColor;
  mTextBgColor = rContext.mTextBgColor;
  mLinkColor = rContext.mLinkColor;
  
  mAlignHorizontal = rContext.mAlignHorizontal;
  mAlignVertical = rContext.mAlignVertical;

  mpStyleSheets = rContext.mpStyleSheets;

  return *this;
}

nuiHTMLContext::~nuiHTMLContext()
{
  if (mpFontRequest)
    mpFontRequest->Release();
  if (mpFont)
    mpFont->Release();
}

void nuiHTMLContext::UpdateFont()
{
  nuiFont* pOld = mpFont;
  
  mpFont = nuiFontManager::GetManager().GetFont(*mpFontRequest);
  NGL_ASSERT(mpFont);
  nuiGlyphInfo ginfo;
  mpFont->GetGlyphInfo(ginfo, mpFont->GetGlyphIndex(_T(' ')), nuiFontBase::eGlyphBitmap);
  mHSpace = ginfo.AdvanceX;
  
  if (pOld)
    pOld->Release();
}


