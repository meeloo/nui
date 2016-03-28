/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"

//class nuiRSSView : public nuiWidget
nuiRSSView::nuiRSSView(const nglString& rURL, int32 SecondsBetweenUpdates, nglIStream* pOriginalStream, bool ForceNoHTML)
: mViewSink(this)
{
  if (SetObjectClass("nuiRSSView"))
  {
    InitAttributes();
  }
  
  mTextColor = nuiColor(0,0,0);
  mFont = nglString::Null;
  mMaxItems = -1;
  
  mForceNoHTML = ForceNoHTML;
  mpRSS = new nuiRSS(rURL, SecondsBetweenUpdates, pOriginalStream);
  mpBox = new nuiVBox();
  mpBox->SetExpand(mExpand);
  AddChild(mpBox);
  mViewSink.Connect(mpRSS->UpdateDone, &nuiRSSView::Update);
  Update(nuiEvent());
}

nuiRSSView::~nuiRSSView()
{
  delete mpRSS;
}

nuiRect nuiRSSView::CalcIdealSize()
{
  return nuiWidget::CalcIdealSize();
}

bool nuiRSSView::SetRect(const nuiRect& rRect)
{
  nuiWidget::SetSelfRect(rRect);
  return true;
}

void nuiRSSView::InitAttributes()
{
  AddAttribute(new nuiAttribute<const nglString&>
               (nglString("Font"), nuiUnitName,
                nuiMakeDelegate(this, &nuiRSSView::_GetFont), 
                nuiMakeDelegate(this, &nuiRSSView::_SetFont)));
  
  AddAttribute(new nuiAttribute<const nuiColor&>
               (nglString("TextColor"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiRSSView::GetTextColor), 
                nuiMakeDelegate(this, &nuiRSSView::SetTextColor)));
  
  AddAttribute(new nuiAttribute<const nglString&>
               (nglString("URL"), nuiUnitName,
                nuiMakeDelegate(this, &nuiRSSView::GetURL), 
                nuiMakeDelegate(this, &nuiRSSView::SetURL)));
  
  AddAttribute(new nuiAttribute<const nglString&>
               (nglString("ItemWidget"), nuiUnitName,
                nuiMakeDelegate(this, &nuiRSSView::GetItemWidget), 
                nuiMakeDelegate(this, &nuiRSSView::SetItemWidget)));
  
  AddAttribute(new nuiAttribute<int32>
               (nglString("MaxItems"), nuiUnitNone,
                nuiMakeDelegate(this, &nuiRSSView::GetMaxItems), 
                nuiMakeDelegate(this, &nuiRSSView::SetMaxItems)));
  
  AddAttribute(new nuiAttribute<nuiExpandMode>
               (nglString("Expand"), nuiUnitPixels,
                nuiMakeDelegate(this, &nuiRSSView::GetExpand), 
                nuiMakeDelegate(this, &nuiRSSView::SetExpand)));
  
  AddAttribute(new nuiAttribute<int32>
               (nglString("RefreshRate"), nuiUnitSeconds,
                nuiMakeDelegate(this, &nuiRSSView::GetRefreshRate), 
                nuiMakeDelegate(this, &nuiRSSView::SetRefreshRate)));
  
}



void nuiRSSView::Update(const nuiEvent& rEvent)
{
  mpBox->Trash();
  mpBox = new nuiVBox();
  mpBox->SetExpand(mExpand);
  AddChild(mpBox);

  int32 count = mpRSS->GetItemCount();
  count = mMaxItems >= 0 ? MIN(mMaxItems, count) : count;
  
  for (uint32 i = 0; i < count; i++)
  {
    const nuiRSSItem& rItem(mpRSS->GetItem(i));
    //printf("%s / %s\n", rItem.GetLink().GetChars(), rItem.GetTitle().GetChars());

    nglString desc(rItem.GetDescription());
    
    nuiHTML html(true);
    bool res = !desc.IsNull();
    if (res)
    {
      std::string str(desc.GetStdString());
      nglIMemory mem(&str[0], str.size());
      bool res = html.Load(mem);
    }
    
    nglString text;
    if (res)
    {
      // Contents is valid HTML
      html.GetSimpleText(text);
      //NGL_OUT("%d - Could parse HTML tags:\n%s\n", i, text.GetChars());
    }
    else
    {
      text = rItem.GetDescription();
      //NGL_OUT("%d - Couldn't parse HTML tags:\n%s\n", i, text.GetChars());
    }

    
    if (mItemWidget.IsEmpty())
    {
      nuiHyperLink* pLink = new nuiHyperLink(rItem.GetLink(), rItem.GetTitle());
      pLink->UseEllipsis(true);
      nuiFolderPane* pPane = new nuiFolderPane(pLink, true);
      
      
  //    nuiLabel* pLabel = new nuiLabel(text);
  //    pLabel->SetObjectName("nuiRSSView::Description");
  //    pLabel->SetWrapping(true);
      if (!mForceNoHTML && res)
      {
        nuiHTMLView* pLabel = new nuiHTMLView(480);
        pLabel->SetText(desc);
        pLabel->SetObjectClass("nuiRSSContents");
        if (!mFont.IsNull())
          pLabel->_SetFont(mFont);
        pLabel->SetTextColor(mTextColor);
        pPane->AddChild(pLabel);
      }
      else
      {
        nuiLabel* pLabel = new nuiLabel(text);
        pLabel->SetObjectClass("nuiRSSContents");
        pLabel->SetWrapping(true);
        pPane->AddChild(pLabel);
      }
      mpBox->AddCell(pPane);
    }
    else
    {
//      if (!mForceNoHTML && res)
//      {
//        text = desc;
//      }

      NGL_OUT("AGENDA: %s\n", rItem.GetTitle().GetChars());
      std::map<nglString, nglString> dictionnary;
      dictionnary["ItemText"] = text;
      dictionnary["ItemHTML"] = desc;
      dictionnary["Title"] = rItem.GetTitle();
      dictionnary["Link"] = rItem.GetLink();
      dictionnary["Description"] = rItem.GetDescription();
      dictionnary["Author"] = rItem.GetAuthor();
      dictionnary["Category"] = rItem.GetCategory();
      dictionnary["CategoryDomain"] = rItem.GetCategoryDomain();
      dictionnary["Comments"] = rItem.GetComments();
      dictionnary["EnclosureURL"] = rItem.GetEnclosureURL();
      dictionnary["EnclosureType"] = rItem.GetEnclosureType();
      dictionnary["GUID"] = rItem.GetGUID();
      dictionnary["PublishingDate"] = rItem.GetPublishingDate();
      dictionnary["SourceURL"] = rItem.GetSourceURL();
      dictionnary["ImageURL"] = rItem.GetImageURL();
      if (!rItem.GetImageURL().IsEmpty())
      {
        nglString imagehtml;
        imagehtml.CFormat("<img src=\"%s\" alt=\"%s\"/>", rItem.GetImageURL().GetChars(), rItem.GetImageTitle().GetChars());
        NGL_OUT("ImageHTML: %s\n", imagehtml.GetChars());
        dictionnary["ImageHTML"] = imagehtml;
      }
      else
      {
        nglString imagehtml;
        imagehtml.CFormat("<img src=\"%s\" alt=\"%s\"/>", rItem.GetLink().GetChars(), _T(""));
        imagehtml.Replace("https", "http");
        NGL_OUT("ImageHTML (from link): %s\n", imagehtml.GetChars());
        dictionnary["ImageHTML"] = imagehtml;
      }
      if (!rItem.GetEnclosureURL().IsEmpty())
      {
        nglString enclosurehtml;
        enclosurehtml.CFormat("<img src=\"%s\" alt=\"enclosure\"/>", rItem.GetEnclosureURL().GetChars());
        NGL_OUT("EnclosureHTML: %s\n", enclosurehtml.GetChars());
        dictionnary["EnclosureHTML"] = enclosurehtml;
      }
      nuiWidget* pWidget = nuiBuilder::Get().CreateWidget(mItemWidget, dictionnary);
      if (pWidget)
        mpBox->AddCell(pWidget);
    }
    
  }
}

void nuiRSSView::ForceUpdate()
{
  mpRSS->ForceUpdate();
}

void nuiRSSView::SetURL(const nglString& rURL)
{
  mpRSS->SetURL(rURL);
}

const nglString& nuiRSSView::GetURL() const
{
  return mpRSS->GetURL();
}

void nuiRSSView::SetItemWidget(const nglString& rWidget)
{
  mItemWidget = rWidget;
  Update(nuiEvent());
}

const nglString& nuiRSSView::GetItemWidget() const
{
  return mItemWidget;
}

void nuiRSSView::SetMaxItems(int32 maxshown)
{
  mMaxItems = maxshown;
  Update(nuiEvent());
}

int32 nuiRSSView::GetMaxItems() const
{
  return mMaxItems;
}


void nuiRSSView::_SetFont(const nglString& rFontSymbol)
{
  mFont = rFontSymbol;
  ForceUpdate();  
}


const nglString& nuiRSSView::_GetFont() const
{
  return mFont;
}


const nuiColor& nuiRSSView::GetTextColor() const
{
  return mTextColor;
}


void nuiRSSView::SetTextColor(const nuiColor& Color)
{
  mTextColor = Color;
  ForceUpdate();  
}


void nuiRSSView::SetExpand(nuiExpandMode expand)
{
  mExpand = expand;
  if (mpBox)
    mpBox->SetExpand(mExpand);
}

nuiExpandMode nuiRSSView::GetExpand() const
{
  return mExpand;
}

void nuiRSSView::SetRefreshRate(int32 SecondsBetweenUpdates)
{
  return mpRSS->SetRefreshRate(SecondsBetweenUpdates);
}

int32 nuiRSSView::GetRefreshRate() const
{
  return mpRSS->GetRefreshRate();
}
