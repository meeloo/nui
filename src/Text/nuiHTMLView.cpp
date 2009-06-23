/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"
#include "nuiHTMLView.h"
#include "nuiHTTP.h"
#include "nuiFontManager.h"
#include "nuiHTTP.h"

#include "nuiHTMLContext.h"
#include "nuiHTMLItem.h"
#include "nuiHTMLBox.h"
#include "nuiHTMLText.h"
#include "nuiHTMLImage.h"
#include "nuiHTMLFont.h"


/////////////////////////////// nuiHTMLView
nuiHTMLView::nuiHTMLView(float IdealWidth)
{
  mpHTML = NULL;
  mpRootBox = NULL;
  mIdealWidth = IdealWidth;
  mVSpace = 2.0f;
  mHSpace = 0.0f;
}

nuiHTMLView::~nuiHTMLView()
{
  delete mpHTML;
}

nuiRect nuiHTMLView::CalcIdealSize()
{
  float IdealWidth = mIdealWidth;
  if (mRect.GetWidth() > 0)
    IdealWidth = mRect.GetWidth();
  Clear();
//  context.mSetLayout = true;
//  WalkTree(mpHTML, context);
  //  return nuiRect(context.mMaxWidth, context.mH);
  nuiHTMLContext context;
  context.mMaxWidth = IdealWidth;
  if (!mpRootBox)
    return nuiRect(IdealWidth, 400.0f);
  mpRootBox->Layout(context);
  return nuiRect(mpRootBox->GetIdealRect().GetWidth(), mpRootBox->GetIdealRect().GetHeight());
}

bool nuiHTMLView::SetRect(const nuiRect& rRect)
{
  nuiWidget::SetRect(rRect);
  if (!mpRootBox)
    return true;
  nuiHTMLContext context;
  context.mMaxWidth = mRect.GetWidth();
  mpRootBox->Layout(context);
  return true;
}

bool nuiHTMLView::Draw(nuiDrawContext* pContext)
{
  nuiSimpleContainer::Draw(pContext);
  if (mpRootBox)
    mpRootBox->CallDraw(pContext);
  return true;
}

void nuiHTMLView::SetIdealWidth(float IdealWidth)
{
  mIdealWidth = IdealWidth;
  InvalidateLayout();
}

float nuiHTMLView::GetIdealWidth() const
{
  return mIdealWidth;
}

float nuiHTMLView::GetVSpace() const
{
  return mVSpace;
}

float nuiHTMLView::GetHSpace() const
{
  return mHSpace;
}

void nuiHTMLView::SetVSpace(float VSpace)
{
  mVSpace = VSpace;
  InvalidateLayout();
}

void nuiHTMLView::SetHSpace(float HSpace)
{
  mHSpace = HSpace;
  InvalidateLayout();
}

bool nuiHTMLView::SetText(const nglString& rHTMLText)
{
  Clear();
  nuiHTML* pHTML = new nuiHTML();
  
  std::string str(rHTMLText.GetStdString());
  nglIMemory mem(&str[0], str.size());
  bool res = pHTML->Load(mem);
  
  if (res)
  {
    Clear();
    delete mpHTML;
    mpHTML = pHTML;
    mpRootBox = new nuiHTMLBox(mpHTML);
    ParseTree(mpHTML, mpRootBox);
    nuiHTMLContext context;
    mpRootBox->Layout(context);
    InvalidateLayout();
  }
  return res;
}

bool nuiHTMLView::SetURL(const nglString& rURL)
{
  nglString url(rURL);
  nuiHTTPRequest request(rURL);
  nuiHTTPResponse* pResponse = request.SendRequest();
  if (!pResponse)
    return false;
  
  //NGL_OUT(_T("HTTP Response\n%ls\n\n"), pResponse->GetHeadersRep().GetChars());
  const nuiHTTPHeaderMap& rHeaders(pResponse->GetHeaders());
  nuiHTTPHeaderMap::const_iterator it = rHeaders.find(_T("location"));
  if (it != rHeaders.end())
  {
    nglString newurl = it->second;
    if (newurl[0] == '/')
    {
      url.TrimRight('/');
      url += newurl;
    }
    else
    {
      url = newurl;
    }
    NGL_OUT(_T("\n\nNew location: %ls\n\n"), url.GetChars());
    
    delete pResponse;
    return SetURL(url);
  }
  
  nuiHTML* pHTML = new nuiHTML();
  pHTML->SetSourceURL(url);
  nglIMemory mem(&pResponse->GetBody()[0], pResponse->GetBody().size());
  
  bool res = pHTML->Load(mem);
  
  if (res)
  {
    Clear();
    delete mpHTML;
    mpHTML = pHTML;
    mpRootBox = new nuiHTMLBox(mpHTML);
    ParseTree(mpHTML, mpRootBox);
    nuiHTMLContext context;
    mpRootBox->Layout(context);
    InvalidateLayout();
  }
  return res;
}

const nglString& nuiHTMLView::GetURL() const
{
  if (mpHTML)
    return mpHTML->GetSourceURL();
  return nglString::Null;
}

//void nuiHTMLView::WalkTree(nuiHTMLNode* pNode, const nuiHTMLContext& rContext)
//{
//  nuiHTMLContext context(rContext);
//  context.mpNode = pNode;
//  if (!InterpretTree(context))
//    return;
//  
//  uint32 count = pNode->GetNbChildren();
//  for (uint32 i = 0; i < count; i++)
//  {
//    nuiHTMLNode* pChild = pNode->GetChild(i);
//    WalkTree(pChild, context);
//  }
//  LayoutLine(context);
//}
//

void nuiHTMLView::ParseTree(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  uint32 count = pNode->GetNbChildren();
  for (uint32 i = 0; i < count; i++)
  {
    nuiHTMLNode* pChild = pNode->GetChild(i);
    switch (pChild->GetTagType())
    {
      case nuiHTML::eTag_HTML:
        {
          ParseHTML(pChild, pBox);
          return;
        }
        break;
      default:
        {
//          printf("tree??? '%ls'\n", pChild->GetName().GetChars());
//          ParseTree(pChild, pBox); // Try all children!!!
        }
        break;
    }
  }
}

void nuiHTMLView::ParseHTML(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html\n");
  uint32 count = pNode->GetNbChildren();
  for (uint32 i = 0; i < count; i++)
  {
    nuiHTMLNode* pChild = pNode->GetChild(i);
    switch (pChild->GetTagType())
    {
      case nuiHTML::eTag_HEAD:
        ParseHead(pChild, pBox);
        break;
        
      case nuiHTML::eTag_BODY:
        ParseBody(pChild, pBox);
        break;
        
      default:
        {        
          //printf("html??? '%ls'\n", pChild->GetName().GetChars());
        }
        break;
    }
  }
  //printf("/html\n");
}

void nuiHTMLView::ParseHead(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  uint32 count = pNode->GetNbChildren();
  for (uint32 i = 0; i < count; i++)
  {
    nuiHTMLNode* pChild = pNode->GetChild(i);
    switch (pChild->GetTagType())
    {
      case nuiHTML::eTag_TITLE:
        ParseTitle(pChild, pBox);
        break;
      default:
        {        
          //printf("head??? '%ls'\n", pChild->GetName().GetChars());
        }
        break;
    }
  }
}

void nuiHTMLView::ParseTitle(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html title\n");
}

void nuiHTMLView::ParseBody(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html body\n");
  uint32 count = pNode->GetNbChildren();
  for (uint32 i = 0; i < count; i++)
  {
    nuiHTMLNode* pChild = pNode->GetChild(i);
    switch (pChild->GetTagType())
    {
      case nuiHTML::eTag_DIV:
        ParseDiv(pChild, pBox);
        break;
      case nuiHTML::eTag_TABLE:
        ParseTable(pChild, pBox);
        break;
      case nuiHTML::eTag_IMG:
        ParseImage(pChild, pBox);
        break;
      case nuiHTML::eTag_UL:
      case nuiHTML::eTag_OL:
      case nuiHTML::eTag_DL:
        ParseList(pChild, pBox);
        break;
      case nuiHTML::eTag_P:
        ParseP(pChild, pBox);
        break;
      case nuiHTML::eTag_I:
      case nuiHTML::eTag_B:
      case nuiHTML::eTag_U:
      case nuiHTML::eTag_STRIKE:
      case nuiHTML::eTag_STRONG:
      case nuiHTML::eTag_EM:
        ParseFormatTag(pChild, pBox);
        break;
      case nuiHTML::eTag_BR:
        ParseBr(pChild, pBox);
        break;
      case nuiHTML::eTag_A:
        ParseA(pChild, pBox);
        break;
      case nuiHTML::eTag_SPAN:
        ParseSpan(pChild, pBox);
        break;
      case nuiHTML::eTag_FONT:
        ParseFont(pChild, pBox);
        break;
      case nuiHTML::eTag_SCRIPT:
      case nuiHTML::eTag_COMMENT:
        // Skip those tags
        break;
      default:
        if (pChild->GetName().IsEmpty())
          ParseText(pChild, pBox);
        else
        {
          //printf("body??? '%ls'\n", pChild->GetName().GetChars());
          ParseBody(pChild, pBox);
        }
        break;
    }
  }
  //printf("body /body\n");
}

void nuiHTMLView::ParseText(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html text\n");
  const nglString& rText(pNode->GetText());
  std::vector<nglString> words;
  rText.Tokenize(words);
  
  for (uint32 i = 0; i < words.size(); i++)
  {
    pBox->AddItem(new nuiHTMLText(pNode, words[i]));
  }
  //ParseBody(pNode, pBox);
  //printf("html /text\n");
}

void nuiHTMLView::ParseDiv(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html div\n");
  nuiHTMLBox* pNewBox = new nuiHTMLBox(pNode);
  pBox->AddItem(pNewBox);
  
  ParseBody(pNode, pNewBox);
  //printf("html /div\n");
}

void nuiHTMLView::ParseTable(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html table\n");
  nuiHTMLBox* pNewBox = new nuiHTMLBox(pNode, false);
  pBox->AddItem(pNewBox);
  
  uint32 count = pNode->GetNbChildren();
  for (uint32 i = 0; i < count; i++)
  {
    nuiHTMLNode* pChild = pNode->GetChild(i);
    switch (pChild->GetTagType())
    {
      case nuiHTML::eTag_TR:
        ParseTableRow(pChild, pNewBox);
        break;
    }
  }
  //printf("html /table\n");
}

void nuiHTMLView::ParseImage(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  nuiHTMLImage* pImg = new nuiHTMLImage(pNode);
  pBox->AddItem(pImg);
}

void nuiHTMLView::ParseTableRow(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html table row\n");
  nuiHTMLBox* pNewBox = new nuiHTMLBox(pNode, true);
  pBox->AddItem(pNewBox);
  
  uint32 count = pNode->GetNbChildren();
  for (uint32 i = 0; i < count; i++)
  {
    nuiHTMLNode* pChild = pNode->GetChild(i);
    switch (pChild->GetTagType())
    {
      case nuiHTML::eTag_TD:
        ParseBody(pChild, pNewBox);
        break;
    }
  }
  //printf("html /table row\n");
}


void nuiHTMLView::ParseList(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html list\n");
  nuiHTMLBox* pNewBox = new nuiHTMLBox(pNode, false);
  pBox->AddItem(pNewBox);
  
  uint32 count = pNode->GetNbChildren();
  for (uint32 i = 0; i < count; i++)
  {
    nuiHTMLNode* pChild = pNode->GetChild(i);
    switch (pChild->GetTagType())
    {
      case nuiHTML::eTag_LI:
      {
        uint32 count2 = pChild->GetNbChildren();
        for (uint32 j = 0; j < count2; j++)
        {
          nuiHTMLNode* pChild2 = pChild->GetChild(j);
          switch (pChild2->GetTagType())
          {
            case nuiHTML::eTag_LI:
              ParseBody(pChild2, pNewBox);
              break;
          }
        }
      }
      break;
    }
  }
  //printf("html /list\n");
}

void nuiHTMLView::ParseP(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html p\n");
  nuiHTMLBox* pNewBox = new nuiHTMLBox(pNode);
  pBox->AddItem(pNewBox);
  
  ParseBody(pNode, pNewBox);
  //printf("html /p\n");
}

void nuiHTMLView::ParseFormatTag(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html format\n");
  pBox->AddItem(new nuiHTMLItem(pNode));
  
  ParseBody(pNode, pBox);
  pBox->AddItemEnd(new nuiHTMLItem(pNode, false));
  //printf("html /format\n");
}

void nuiHTMLView::ParseA(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html anchor\n");
  ParseBody(pNode, pBox);
  //printf("html /anchor\n");
}

void nuiHTMLView::ParseBr(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html break\n");
  pBox->AddItem(new nuiHTMLItem(pNode));
}

void nuiHTMLView::ParseSpan(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  //printf("html span\n");
  ParseBody(pNode, pBox);
  //printf("html /span\n");
}

void nuiHTMLView::ParseFont(nuiHTMLNode* pNode, nuiHTMLBox* pBox)
{
  pBox->AddItem(new nuiHTMLFont(pNode));
  
  ParseBody(pNode, pBox);
  pBox->AddItemEnd(new nuiHTMLFont(pNode));
}
