/*
 *  nuiShadeContainer.h
 *  libnui
 */

#ifndef __nuiShadeContainer_h__
#define __nuiShadeContainer_h__

#include "nuiWidget.h"

class nuiShadeContainer : public nuiWidget
{
public:
  nuiShadeContainer(nuiSize shadeSize)
  : nuiWidget(), mShadeSize(shadeSize)
  {
  }
  
  virtual ~nuiShadeContainer()
  {
  }
  
  nuiRect CalcIdealSize()
  {
    nuiRect idealRect = nuiWidget::CalcIdealSize();
    idealRect.SetSize(idealRect.GetWidth()+2*mShadeSize, idealRect.GetHeight()+mShadeSize);
    return idealRect; 
  }

  virtual bool SetRect(const nuiRect& rRect)
  {
    nuiWidget::SetSelfRect(rRect);
    
    nuiRect rect(rRect.Size());
    rect.mLeft    += mShadeSize;
    rect.mRight   -= mShadeSize;
    rect.mBottom  -= mShadeSize;
    
    IteratorPtr pIt;
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      if (mCanRespectConstraint)
        pItem->SetLayoutConstraint(mConstraint);
      pItem->GetIdealRect();
      pItem->SetLayout(rect);
    }
    delete pIt;
    
    DebugRefreshInfo();
    return true;  
  }

  virtual bool Draw(nuiDrawContext* pContext)
  {
    nuiRect rect = GetRect().Size();
    
    rect.mLeft    += mShadeSize;
    rect.mRight   -= mShadeSize;
    rect.mBottom  -= mShadeSize;
    
    nuiRect shadeRect = GetRect().Size();
    
    pContext->DrawShade(rect, shadeRect, nuiColor(1.0f, 1.0f, 1.0f, GetMixedAlpha()));
    
    DrawChildren(pContext);
    
    return true;
  }

protected:
  nuiSize mShadeSize;
};

#endif //ifndef __nuiShadeContainer_h__

