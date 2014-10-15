/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/




#include "nui.h"

//#define NUI_CHECK_LAYOUTS




class nuiContainerIterator : public nuiContainer::Iterator
{
public:
  nuiContainerIterator(nuiContainer* pSource, bool DoRefCounting);
  nuiContainerIterator(const nuiContainerIterator& rIterator);
  virtual ~nuiContainerIterator();

  virtual nuiWidgetPtr GetWidget() const;

  nuiContainerIterator& operator = (const nuiContainerIterator& rIterator);

  void Increment()
  {
    mIndex++;
  }

  void Decrement()
  {
    mIndex--;
  }
protected:
  int32 mIndex;
private:
  friend class nuiContainer;
  bool SetIndex(int32 index);
  int32 GetIndex() const;
};

class nuiContainerConstIterator : public nuiContainer::ConstIterator
{
public:
  nuiContainerConstIterator(const nuiContainer* pSource, bool DoRefCounting);
  nuiContainerConstIterator(const nuiContainerConstIterator& rIterator);
  virtual ~nuiContainerConstIterator();

  virtual nuiWidgetPtr GetWidget() const;

  nuiContainerConstIterator& operator = (const nuiContainerConstIterator& rIterator);

  void Increment()
  {
    mIndex++;
  }

  void Decrement()
  {
    mIndex--;
  }
protected:
  int32 mIndex;
private:
  friend class nuiContainer;
  bool SetIndex(int32 index);
  int32 GetIndex() const;
};

typedef nuiContainerIterator* nuiContainerIteratorPtr;
typedef nuiContainerConstIterator* nuiContainerConstIteratorPtr;


nuiContainer::~nuiContainer()
{
  CheckValid();
  // Delete all children:
  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem)
    {
      //      if (!pItem->IsTrashed(false) && pItem->Release())
      //        pItem->SetParent(NULL);
      if (!pItem->IsTrashed(false))
        pItem->SetParent(NULL);
      pItem->Release();

    }
  }
  delete pIt;
}

bool nuiContainer::AddChild(nuiWidgetPtr pChild)
{
  CheckValid();
  if (GetDebug())
  {
    NGL_OUT("[%s] Add Child %p <--- %p (%s / %s)\n", GetObjectClass().GetChars(), this, pChild, pChild->GetObjectClass().GetChars(), pChild->GetObjectName().GetChars());
  }
  pChild->Acquire();
  nuiContainer* pParent = pChild->GetParent();
  NGL_ASSERT(pParent != this);

  int32 capacity = mpChildren.capacity();
  int32 size = mpChildren.size();
  if (size == capacity)
  {
    if (size < 128)
    {
      mpChildren.reserve(size * 2);
    }
    else
    {
      mpChildren.reserve(size + 128);
    }
  }

  mpChildren.push_back(pChild);
  if (pParent)
    pParent->DelChild(pChild); // Remove from previous parent...

  pChild->SetParent(this);
  ChildAdded(this, pChild);
  Invalidate();
  InvalidateLayout();

  DebugRefreshInfo();
  return true;
}

bool nuiContainer::DelChild(nuiWidgetPtr pChild)
{
  CheckValid();
  NGL_ASSERT(pChild->GetParent() == this)


  if (GetDebug())
  {
    NGL_OUT("[%s] Del Child %p <--- %p (%s / %s)\n", GetObjectClass().GetChars(), this, pChild, pChild->GetObjectClass().GetChars(), pChild->GetObjectName().GetChars());
  }

  nuiWidgetList::iterator it  = mpChildren.begin();
  nuiWidgetList::iterator end = mpChildren.end();
  for ( ; it != end; ++it)
  {
    if (*it == pChild)
    {
      mpChildren.erase(it);
      if (!pChild->IsTrashed())
      {
        nuiTopLevel* pRoot = GetTopLevel();
        pChild->Trashed();
        Invalidate();

        if (pRoot)
          pRoot->AdviseObjectDeath(pChild);
        pChild->SetParent(NULL);
      }
      ChildDeleted(this, pChild);
      InvalidateLayout();
      DebugRefreshInfo();
      pChild->Release();
      return true;
    }
  }
  DebugRefreshInfo();
  return false;
}

int nuiContainer::GetChildrenCount() const
{
  CheckValid();
  return mpChildren.size();
}

nuiWidgetPtr nuiContainer::GetChild(int index)
{
  CheckValid();
  NGL_ASSERT(index >= 0);
  NGL_ASSERT(index < mpChildren.size());
  return mpChildren[index];
}

bool nuiContainer::Clear()
{
  CheckValid();
  // start by trashing everybody
  nuiContainer::ChildrenCallOnTrash();

  // then, clear the container
  int childCount = GetChildrenCount();
  for (childCount--; childCount >= 0; childCount--)
  {
    nuiWidget* pWidget = GetChild(childCount);
    if (pWidget)
    {
      DelChild(pWidget);
    }
  }
  mpChildren.clear();
  InvalidateLayout();
  DebugRefreshInfo();
  return true;
}


nuiContainer::Iterator* nuiContainer::GetFirstChild(bool DoRefCounting)
{
  CheckValid();
  IteratorPtr pIt;
  pIt = new nuiContainerIterator(this, DoRefCounting);
  bool valid = !mpChildren.empty();
  pIt->SetValid(valid);
  if (valid)
    ((nuiContainerIterator*)pIt)->SetIndex(0);
  return pIt;
}

nuiContainer::ConstIterator* nuiContainer::GetFirstChild(bool DoRefCounting) const
{
  CheckValid();
  nuiContainerConstIteratorPtr pIt;
  pIt = new nuiContainerConstIterator(this, DoRefCounting);
  bool valid = !mpChildren.empty();
  pIt->SetValid(valid);
  if (valid)
    pIt->SetIndex(0);
  return pIt;
}

nuiContainer::Iterator* nuiContainer::GetLastChild(bool DoRefCounting)
{
  CheckValid();
  IteratorPtr pIt;
  pIt = new nuiContainerIterator(this, DoRefCounting);
  if (!mpChildren.empty())
  {
    ((nuiContainerIterator*)pIt)->SetIndex(mpChildren.size() - 1);
    pIt->SetValid(true);
  }
  else
  {
    pIt->SetValid(false);
  }
  return pIt;
}

nuiContainer::ConstIterator* nuiContainer::GetLastChild(bool DoRefCounting) const
{
  CheckValid();
  nuiContainerConstIteratorPtr pIt;
  pIt = new nuiContainerConstIterator(this, DoRefCounting);
  if (!mpChildren.empty())
  {
    pIt->SetIndex(mpChildren.size() - 1);
    pIt->SetValid(true);
  }
  else
  {
    pIt->SetValid(false);
  }
  return pIt;
}

bool nuiContainer::GetNextChild(nuiContainer::IteratorPtr pIterator)
{
  CheckValid();
  if (!pIterator)
    return false;
  if (!pIterator->IsValid())
    return false;
  ((nuiContainerIterator*)pIterator)->Increment();
  if (((nuiContainerIterator*)pIterator)->GetIndex() >= mpChildren.size())
  {
    pIterator->SetValid(false);
    return false;
  }
  pIterator->SetValid(true);
  return true;
}

bool nuiContainer::GetNextChild(nuiContainer::ConstIteratorPtr pIterator) const
{
  CheckValid();
  if (!pIterator)
    return false;
  if (!pIterator->IsValid())
    return false;
  ((nuiContainerConstIterator*)pIterator)->Increment();
  if (((nuiContainerConstIterator*)pIterator)->GetIndex() >= mpChildren.size())
  {
    pIterator->SetValid(false);
    return false;
  }
  pIterator->SetValid(true);
  return true;
}

bool nuiContainer::GetPreviousChild(nuiContainer::IteratorPtr pIterator)
{
  CheckValid();
  if (!pIterator)
    return false;
  if (!pIterator->IsValid())
    return false;
  if (((nuiContainerIterator*)pIterator)->GetIndex() <= 0)
  {
    pIterator->SetValid(false);
    return false;
  }

  ((nuiContainerIterator*)pIterator)->Decrement();

  pIterator->SetValid(true);
  return true;
}

bool nuiContainer::GetPreviousChild(nuiContainer::ConstIteratorPtr pIterator) const
{
  CheckValid();
  if (!pIterator)
    return false;
  if (!pIterator->IsValid())
    return false;
  if (((nuiContainerConstIterator*)pIterator)->GetIndex() <= 0)
  {
    pIterator->SetValid(false);
    return false;
  }

  ((nuiContainerConstIterator*)pIterator)->Decrement();

  pIterator->SetValid(true);
  return true;
}

void nuiContainer::RaiseChild(nuiWidgetPtr pChild)
{
  CheckValid();

  nuiWidgetList::iterator it = mpChildren.begin();
  nuiWidgetList::iterator end = mpChildren.end();
  for ( ; it != end; ++it)
  {
    nuiWidgetPtr pItem = *it;
    if (pChild == pItem)
    {
      nuiWidgetList::iterator next = it;
      ++next;
      mpChildren.erase(it);
      mpChildren.insert(next, pItem);
      Invalidate();
      DebugRefreshInfo();
      return;
    }
  }
  DebugRefreshInfo();
}

void nuiContainer::LowerChild(nuiWidgetPtr pChild)
{
  CheckValid();
  nuiWidgetList::iterator it = mpChildren.begin();
  nuiWidgetList::iterator end = mpChildren.end();
  nuiWidgetList::iterator previous = it;
  for ( ; it != end; ++it)
  {
    nuiWidgetPtr pItem = *it;
    if (pChild == pItem)
    {
      if (previous != mpChildren.begin())
      {
        nuiWidgetPtr pPrevious = *previous;
        mpChildren.erase(previous);
        mpChildren.insert(it, pPrevious);
        Invalidate();
      }
      DebugRefreshInfo();
      return;
    }
    previous = it;
  }
  DebugRefreshInfo();
}

void nuiContainer::RaiseChildToFront(nuiWidgetPtr pChild)
{
  CheckValid();
  nuiWidgetList::iterator it = mpChildren.begin();
  nuiWidgetList::iterator end = mpChildren.end();
  for ( ; it != end; ++it)
  {
    nuiWidgetPtr pItem = *it;
    if (pChild == pItem)
    {
      mpChildren.erase(it);
      mpChildren.push_back(pItem);
      Invalidate();
      DebugRefreshInfo();
      return;
    }
  }
  DebugRefreshInfo();
}

void nuiContainer::LowerChildToBack(nuiWidgetPtr pChild)
{
  CheckValid();
  nuiWidgetList::iterator it = mpChildren.begin();
  nuiWidgetList::iterator end = mpChildren.end();
  for ( ; it != end; ++it)
  {
    nuiWidgetPtr pItem = *it;
    if (pChild == pItem)
    {
      mpChildren.erase(it);
      mpChildren.insert(mpChildren.begin(), pItem);
      Invalidate();
      DebugRefreshInfo();
      return;
    }
  }
  DebugRefreshInfo();
}

///////////////////////
////// nuiContainer::Iterator

nuiContainerIterator::nuiContainerIterator(nuiContainer* pSource, bool DoRefCounting)
: nuiContainer::Iterator(pSource, DoRefCounting), mIndex(-1)
{
  mValid = false;
}

nuiContainerConstIterator::nuiContainerConstIterator(const nuiContainer* pSource, bool DoRefCounting)
: nuiContainer::ConstIterator(pSource, DoRefCounting), mIndex(-1)
{
  mValid = false;
}

nuiContainerIterator::nuiContainerIterator(const nuiContainerIterator& rIterator)
: nuiContainer::Iterator(rIterator)
{
  mIndex = rIterator.mIndex;
}

nuiContainerConstIterator::nuiContainerConstIterator(const nuiContainerConstIterator& rIterator)
: nuiContainer::ConstIterator(rIterator)
{
  mIndex = rIterator.mIndex;
}

nuiContainerIterator& nuiContainerIterator::operator = (const nuiContainerIterator& rIterator)
{
  *((nuiContainer::Iterator*)this) = rIterator;
  mIndex = rIterator.mIndex;
  return *this;
}

nuiContainerConstIterator& nuiContainerConstIterator::operator = (const nuiContainerConstIterator& rIterator)
{
  *((nuiContainer::ConstIterator*)this) = rIterator;
  mIndex = rIterator.mIndex;
  return *this;
}

bool nuiContainerIterator::SetIndex(int32 index)
{
  mIndex = index;
  return true;
}

bool nuiContainerConstIterator::SetIndex(int32 index)
{
  mIndex = index;
  return true;
}

int32 nuiContainerIterator::GetIndex() const
{
  return mIndex;
}

int32 nuiContainerConstIterator::GetIndex() const
{
  return mIndex;
}

nuiContainerIterator::~nuiContainerIterator()
{
}

nuiContainerConstIterator::~nuiContainerConstIterator()
{
}

nuiWidgetPtr nuiContainerIterator::GetWidget() const
{
  return IsValid() ? (nuiContainer*)mpSource->GetChild(mIndex) : NULL;
}

nuiWidgetPtr nuiContainerConstIterator::GetWidget() const
{
  return IsValid() ? const_cast<nuiContainer*>((nuiContainer*)mpSource)->GetChild(mIndex) : NULL;
}








nuiContainer::nuiContainer()
: nuiWidget()
{
  SetObjectClass(_T("nuiContainer"));
  NUI_ADD_EVENT(ChildAdded);
  NUI_ADD_EVENT(ChildDeleted);
}

bool nuiContainer::Trash()
{
  nuiAutoRef;
  return nuiWidget::Trash();
}

void nuiContainer::CallOnTrash()
{
  CheckValid();
  ChildrenCallOnTrash();
  nuiWidget::CallOnTrash();
//  OnTrash();
}


void nuiContainer::ChildrenCallOnTrash()
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem)
      pItem->CallOnTrash();
  }
  delete pIt;  
}

nuiContainerPtr nuiContainer::GetRoot() const
{
  CheckValid();
  void* Self = (void*)this;
  if (mpParent)
    return mpParent->GetRoot();
  else
    return (nuiContainerPtr)Self;
}

nuiWidgetPtr nuiContainer::GetChild(nuiSize X, nuiSize Y)
{
  CheckValid();
  X -= mRect.mLeft;
  Y -= mRect.mTop;

  IteratorPtr pIt;
  for (pIt = GetLastChild(); pIt && pIt->IsValid(); GetPreviousChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem && pItem->IsInsideFromSelf(X,Y))
    {
      delete pIt;
      nuiContainerPtr pContainer = dynamic_cast<nuiContainerPtr>(pItem);
      if (pContainer)
        return pContainer->GetChild(X,Y);
      else 
        return pItem;
    }
  }
  delete pIt;

  return this;
}

void nuiContainer::GetChildren(nuiSize X, nuiSize Y, nuiWidgetList& rChildren, bool DeepSearch)
{
  CheckValid();
  X -= mRect.mLeft;
  Y -= mRect.mTop;
  
  IteratorPtr pIt;
  for (pIt = GetLastChild(); pIt && pIt->IsValid(); GetPreviousChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem && pItem->IsInsideFromParent(X,Y))
    {
      if (DeepSearch)
      {
        nuiContainerPtr pContainer = dynamic_cast<nuiContainerPtr>(pItem);
        if (pContainer)
          pContainer->GetChildren(X, Y, rChildren, DeepSearch);
      }
      rChildren.push_back(pItem);
    }
  }
  delete pIt;
}



nuiWidgetPtr nuiContainer::GetChildIf(nuiSize X, nuiSize Y, TestWidgetFunctor* pFunctor)
{
  CheckValid();
  X -= mRect.mLeft;
  Y -= mRect.mTop;

  IteratorPtr pIt;
  for (pIt = GetLastChild(); pIt && pIt->IsValid(); GetPreviousChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem && pItem->IsInsideFromParent(X,Y))
    {
      nuiContainerPtr pContainer = dynamic_cast<nuiContainerPtr>(pItem);
      if (pContainer)
      {
        nuiWidget* pWidget = pContainer->GetChildIf(X,Y, pFunctor);
        if (pWidget)
        {
          delete pIt;
          return pWidget;
        }
      }
      else 
      {
        if ((*pFunctor)(pItem))
        {
          delete pIt;
          return pItem;
        }
      }
    }
  }
  delete pIt;

  if ((*pFunctor)(this))
    return this;

  return NULL;
}


nuiWidgetPtr nuiContainer::GetChild(const nglString& rName, bool ResolveNameAsPath)
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem->GetObjectName() == rName)
    {
      delete pIt;
      return pItem;
    }
  }
  delete pIt;

  if (!ResolveNameAsPath) // Are we allowed to search the complete tree?
    return NULL;

  nuiWidgetPtr pNode = this;
  nglString name = rName;

  if (name[0] == '/')
  {
    // Get the root of the tree:
    pNode = GetRoot();

    name.DeleteLeft(1); // Remove the '/'
  }

  // Get all the nodes and remove the slashes:
  std::vector<nglString> tokens;
  name.Tokenize(tokens, _T('/'));

  size_t i;
  size_t count = tokens.size();
  for (i = 0; i < count; i++)
  {
    nglString& rTok = tokens[i];
    //Node* pOld = pNode;
    if (rTok == _T(".."))
      pNode = pNode->GetParent();
    else
      pNode = pNode->GetChild(rTok, false);

    if (!pNode)
    {
      //NUI_OUT("Tried to find %s on %s", rTok.GetChars(), pOld->GetParamCString(ParamIds::Name));
      return NULL;
    }
  }

  return pNode;
}

nuiWidgetPtr nuiContainer::SearchForChild(const nglString& rName, bool recurse )
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem->GetObjectName() == rName)
    {
      delete pIt;
      return pItem;
    }
  }
  delete pIt;

  if (!recurse) // Are we allowed to search the complete tree?
    return NULL;

  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    nuiContainerPtr pContainer = NULL;
    pContainer = dynamic_cast<nuiContainerPtr>(pItem);
    if (pContainer)
    {
      nuiWidgetPtr pWidget = pContainer->SearchForChild(rName,recurse);
      if (pWidget)
      {
        delete pIt;       
        return pWidget;
      }
    }
  }
  delete pIt;

  return NULL;
}

void nuiContainer::CallConnectTopLevel(nuiTopLevel* pTopLevel)
{
  CheckValid();
  nuiWidget::CallConnectTopLevel(pTopLevel);
  IteratorPtr pIt;
  for (pIt = GetFirstChild(true); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    pIt->GetWidget()->CallConnectTopLevel(pTopLevel);
  }
  delete pIt;
}

void nuiContainer::CallDisconnectTopLevel(nuiTopLevel* pTopLevel)
{
  CheckValid();
  nuiWidget::CallDisconnectTopLevel(pTopLevel);
  IteratorPtr pIt;
  for (pIt = GetFirstChild(true); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    pIt->GetWidget()->CallDisconnectTopLevel(pTopLevel);
  }
  delete pIt;
}

void nuiContainer::InvalidateChildren(bool Recurse)
{
  CheckValid();
  IteratorPtr pIt;
  if (Recurse)
  {
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      nuiContainerPtr pCont = dynamic_cast<nuiContainerPtr>(pItem);
      if (pCont)
        pCont->InvalidateChildren(Recurse);
      pItem->Invalidate();
    }
  }
  else
  {
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      pItem->Invalidate();
    }
  }
  delete pIt;
}

void nuiContainer::SilentInvalidateChildren(bool Recurse)
{
  CheckValid();
  IteratorPtr pIt;
  if (Recurse)
  {
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      nuiContainerPtr pCont = dynamic_cast<nuiContainerPtr>(pItem);
      if (pCont)
        pCont->SilentInvalidateChildren(Recurse);
      pItem->SilentInvalidate();
    }
  }
  else
  {
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      pItem->SilentInvalidate();
    }
  }
  delete pIt;
}

bool nuiContainer::Draw(nuiDrawContext* pContext)
{
  CheckValid();
  return DrawChildren(pContext);
}

bool nuiContainer::DrawChildren(nuiDrawContext* pContext)
{
  CheckValid();
  IteratorPtr pIt;

  if (mReverseRender)
  {
    for (pIt = GetLastChild(); pIt && pIt->IsValid(); GetPreviousChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      if (pItem)
        DrawChild(pContext, pItem);
    }
    delete pIt;
  }
  else
  {
    for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
    {
      nuiWidgetPtr pItem = pIt->GetWidget();
      if (pItem)
        DrawChild(pContext, pItem);
    }
    delete pIt;
  }
  return true;
}

void nuiContainer::DrawChild(nuiDrawContext* pContext, nuiWidget* pChild)
{  
  CheckValid();
  float x,y;

  x = (float)pChild->GetRect().mLeft;
  y = (float)pChild->GetRect().mTop;

  bool matrixchanged = false;
  if (x != 0 || y != 0)
  {
    pContext->PushMatrix();
    pContext->Translate( x, y );
    matrixchanged = true;
  }

  nuiPainter* pPainter = pContext->GetPainter();
  if (mpSavedPainter)
    pContext->SetPainter(mpSavedPainter);

  pChild->DrawWidget(pContext);

  if (mpSavedPainter)
    pContext->SetPainter(pPainter);

  if (IsDrawingInCache(true))
  {
    nuiMetaPainter* pMetaPainter = dynamic_cast<nuiMetaPainter*>(pPainter);
    if (pMetaPainter)
      pMetaPainter->DrawChild(pContext, pChild);
  }

  if (matrixchanged)
  {
    pContext->PopMatrix();
  }
}

////// Private event management:
bool nuiContainer::DispatchMouseClick(const nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
  if (!mMouseEventEnabled || mTrashed)
  return false;

  bool hasgrab = HasGrab(rInfo.TouchId);
  if (IsDisabled() && !hasgrab)
  return false;

  nglMouseInfo info(rInfo);
  GlobalToLocal(info.X, info.Y);

  // Get a chance to preempt the mouse event before the children get it:
  if (PreMouseClicked(info))
  {
    Grab();
    return true;
  }

  if (IsInsideFromRoot(rInfo.X, rInfo.Y) || hasgrab)
  {
    if (!hasgrab)
    {
      IteratorPtr pIt;
      for (pIt = GetLastChild(false); pIt && pIt->IsValid(); GetPreviousChild(pIt))
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
        if (pItem)
        {
          if (IsEnabled() && !HasGrab(rInfo.TouchId))
          {
            if (pItem->DispatchMouseClick(rInfo))
            {
              delete pIt;
              return true;
            }
          }
        }
      }
      delete pIt;
    }

    nglMouseInfo info(rInfo);
    GlobalToLocal(info.X, info.Y);
    if (PreClicked(info))
    {
      Grab();
      return true;
    }
    bool ret = MouseClicked(info);
    ret |= Clicked(info);
    ret = ret | (!mClickThru);
    if (ret)
    Grab();

    return ret;
  }
  return false;
}

bool nuiContainer::DispatchMouseCanceled(nuiWidgetPtr pThief, const nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
  if (mTrashed)
    return false;

  bool hasgrab = HasGrab(rInfo.TouchId);

  nglMouseInfo info(rInfo);
  GlobalToLocal(info.X, info.Y);

  // Get a chance to preempt the mouse event before the children get it:
  PreClickCanceled(info);

  IteratorPtr pIt;
  for (pIt = GetLastChild(false); pIt && pIt->IsValid(); GetPreviousChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem)
    {
      pItem->DispatchMouseCanceled(pThief, rInfo);
    }
  }
  delete pIt;

  if (pThief != this)
  {
    GlobalToLocal(info.X, info.Y);
    PreClickCanceled(info);
    bool ret = MouseCanceled(info);
    ret |= ClickCanceled(info);
    ret = ret | (!mClickThru);
    return ret;
  }

  return false;
}

bool nuiContainer::DispatchMouseUnclick(const nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
  if (!mMouseEventEnabled || mTrashed)
    return false;

  bool hasgrab = HasGrab(rInfo.TouchId);
  if (IsDisabled() && !hasgrab)
    return false;

  nglMouseInfo info(rInfo);
  GlobalToLocal(info.X, info.Y);
  // Get a chance to preempt the mouse event before the children get it:
  if (PreMouseUnclicked(info))
  {
    Ungrab();
    return true;
  }
  
  if (IsInsideFromRoot(rInfo.X, rInfo.Y) || hasgrab)
  {
    if (!hasgrab)
    {
      IteratorPtr pIt;
      for (pIt = GetLastChild(false); pIt && pIt->IsValid(); GetPreviousChild(pIt))
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
        if (pItem)
        {
          if (IsEnabled())
          {
            if ((pItem)->DispatchMouseUnclick(rInfo))
            {
              delete pIt;
              return true;
            }
          }
        }
      }
      delete pIt;
    }

    bool res = PreUnclicked(info);
    if (!res)
    {
      res = MouseUnclicked(info);
      res |= Unclicked(info);
    }

    res = res | (!mClickThru);
    if (res)
      Ungrab();
    return res;
  }
  return false;
}

nuiWidgetPtr nuiContainer::DispatchMouseMove(const nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
  if (!mMouseEventEnabled || mTrashed)
    return NULL;

  nuiWidgetPtr pHandled=NULL;
  bool inside=false,res=false;
  bool hasgrab = HasGrab(rInfo.TouchId);

  if (IsDisabled() && !hasgrab)
    return NULL;

  nglMouseInfo info(rInfo);
  GlobalToLocal(info.X, info.Y);

  // Get a chance to preempt the mouse event before the children get it:
  if (PreMouseMoved(info))
    return this;
  
  if (IsInsideFromRoot(rInfo.X, rInfo.Y) || hasgrab)
  {
    inside = true;

    // If the object has the grab we should not try to notify its children of mouse events!
    if (!hasgrab) 
    {

      IteratorPtr pIt;
      for (pIt = GetLastChild(false); pIt && pIt->IsValid(); GetPreviousChild(pIt))
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
        if (pItem)
        {
          if (pItem->IsVisible())
          {
            pHandled = pItem->DispatchMouseMove(rInfo);
          } 
        }
        if (pHandled)
        {
          // stop as soon as someone caught the event
          delete pIt;
          return pHandled;
        }
      }
      delete pIt;
    }

    res = MouseMoved(info);
    res |= MovedMouse(info);
  }
  else
  {
    if (GetHover())
    {
      res = MouseMoved(info);
      res |= MovedMouse(info);
    }
  }

  if (!pHandled && (res | (!mClickThru)) && inside)
  {
    nuiTopLevelPtr pRoot = GetTopLevel();
    if (pRoot)
      return this;
  }

  if (pHandled)
    return pHandled;

  return (res && inside) ? this : NULL;
}

nuiWidgetPtr nuiContainer::DispatchMouseWheelMove(const nglMouseInfo& rInfo)
{
  CheckValid();
  nuiAutoRef;
  if (!mMouseEventEnabled || mTrashed)
    return NULL;

  nuiWidgetPtr pHandled=NULL;
  bool inside=false,res=false;
  bool hasgrab = HasGrab(rInfo.TouchId);

  if (IsDisabled() && !hasgrab)
    return NULL;

  nglMouseInfo info(rInfo);
  GlobalToLocal(info.X, info.Y);

  // Get a chance to preempt the mouse event before the children get it:
  if (PreMouseWheelMoved(info))
    return this;

  if (IsInsideFromRoot(rInfo.X, rInfo.Y) || hasgrab)
  {
    inside = true;

    // If the object has the grab we should not try to notify its children of mouse events!
    if (!hasgrab)
    {

      IteratorPtr pIt;
      for (pIt = GetLastChild(false); pIt && pIt->IsValid(); GetPreviousChild(pIt))
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
        if (pItem)
        {
          if (pItem->IsVisible())
          {
            pHandled = pItem->DispatchMouseWheelMove(rInfo);
          }
        }
        if (pHandled)
        {
          // stop as soon as someone caught the event
          delete pIt;
          return pHandled;
        }
      }
      delete pIt;
    }

    res = MouseWheelMoved(info);
    res |= WheelMovedMouse(info);
  }
  else
  {
    if (GetHover())
    {
      res = MouseWheelMoved(info);
      res |= WheelMovedMouse(info);
    }
  }

  if (!pHandled && (res | (!mClickThru)) && inside)
  {
    nuiTopLevelPtr pRoot = GetTopLevel();
    if (pRoot)
      return this;
  }

  if (pHandled)
    return pHandled;

  return (res && inside) ? this : NULL;
}


void nuiContainer::SetAlpha(float Alpha)
{
  CheckValid();
  if (Alpha == mAlpha)
    return;
  nuiWidget::SetAlpha(Alpha);
  SilentInvalidateChildren(true);
  DebugRefreshInfo();
}

void nuiContainer::SetEnabled(bool set)
{
  CheckValid();
  if (set == mEnabled)
    return;
  nuiWidget::SetEnabled(set);
  SilentInvalidateChildren(true);
  Invalidate();
  DebugRefreshInfo();
}

void nuiContainer::SetSelected(bool set)
{
  CheckValid();
  if (mSelected == set)
    return;
  nuiWidget::SetSelected(set);
  SilentInvalidateChildren(true);
  Invalidate();
  DebugRefreshInfo();
}

void nuiContainer::SetVisible(bool Visible)
{
  CheckValid();
  if (IsVisible(false) == Visible)
    return;

  nuiWidget::SetVisible(Visible);
  if (mVisible)
    BroadcastVisible();
}

void nuiContainer::BroadcastVisible()
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem->GetNeedSelfRedraw())
    {
      pItem->InvalidateRect(pItem->GetRect().Size());
    }
    nuiContainer* pContainer = dynamic_cast<nuiContainer*>(pItem);
    if (pContainer)
    {
      pContainer->BroadcastVisible();
    }
  }
  delete pIt;
}

nuiRect nuiContainer::CalcIdealSize()
{
  CheckValid();
  nuiRect temp;

  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (mCanRespectConstraint)
      pItem->SetLayoutConstraint(mConstraint);
    nuiRect r(pItem->GetIdealRect()); // Dummy call. Only the side effect is important: the object recalculates its layout.
    temp.Union(temp, r.Size()); 
  }
  delete pIt;

  DebugRefreshInfo();
  return temp.Size();
}

bool nuiContainer::SetRect(const nuiRect& rRect)
{
  CheckValid();
  nuiWidget::SetRect(rRect);

  nuiRect rect(rRect.Size());
  IteratorPtr pIt;
  for (pIt = GetFirstChild(false); pIt && pIt->IsValid(); GetNextChild(pIt))
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

void nuiContainer::InternalSetLayout(const nuiRect& rect, bool PositionChanged, bool SizeChanged)
{
  CheckValid();

  if (mNeedSelfLayout || SizeChanged)
  {
    mInSetRect = true;
    SetRect(rect);
    mInSetRect = false;
    Invalidate();
  }
  else
  {
    // Is this case the widget have just been moved inside its parent. No need to re layout it, only change the rect...
    mRect = rect;
    
    if (mNeedLayout)
    {
      // The children need to be re layed out (at least one of them!).
      nuiContainer::IteratorPtr pIt = GetFirstChild(false);
      do
      {
        nuiWidgetPtr pItem = pIt->GetWidget();
        if (pItem)
        {
          // The rect of each child doesn't change BUT we still ask for its ideal rect.
          nuiRect rect(pItem->GetBorderedRect());
          nuiRect ideal(pItem->GetIdealRect());
          
          if (pItem->HasUserPos()) 	 
          { 	 
            rect = ideal; 	 
          } 	 
          else if (pItem->HasUserSize())
          {
            rect.SetSize(ideal.GetWidth(), ideal.GetHeight());
          }
          else
          {
            // Set the widget to the size of the parent
          }
          
          pItem->SetLayout(rect);
        }
      } while (GetNextChild(pIt));
      delete pIt;
      
    }
  }
  
  //#TEST:
#ifdef NUI_CHECK_LAYOUTS
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem->IsVisible())
    {
      NGL_ASSERT(!pItem->GetNeedLayout());
    }
  }
  delete pIt;
  //#TEST end
#endif
}


////// nuiContainer::Iterator

nuiContainer::Iterator::Iterator(nuiContainer* pSource, bool DoRefCounting)
: mpSource(pSource), mRefCounting(DoRefCounting)
{
  if (mRefCounting)
  {
    NGL_ASSERT(mpSource->GetRefCount() != 0);
    mpSource->Acquire();
    
  }
  mValid = false;
}

nuiContainer::ConstIterator::ConstIterator(const nuiContainer* pSource, bool DoRefCounting)
: mpSource(pSource), mRefCounting(DoRefCounting)
{
  if (mRefCounting)
  {
    NGL_ASSERT(mpSource->GetRefCount() != 0);
    mpSource->Acquire();
  }
  mValid = false;
}

nuiContainer::Iterator::Iterator(const nuiContainer::Iterator& rIterator)
: mpSource(rIterator.mpSource), mRefCounting(rIterator.mRefCounting), mValid(rIterator.mValid)
{
  if (mRefCounting)
  {
    NGL_ASSERT(mpSource->GetRefCount() != 0);
    mpSource->Acquire();
  }
}

nuiContainer::ConstIterator::ConstIterator(const nuiContainer::ConstIterator& rIterator)
: mpSource(rIterator.mpSource), mRefCounting(rIterator.mRefCounting), mValid(rIterator.mValid)
{
  if (mRefCounting)
  {
    NGL_ASSERT(mpSource->GetRefCount() != 0);
    mpSource->Acquire();
  }
}

nuiContainer::Iterator& nuiContainer::Iterator::operator = (const nuiContainer::Iterator& rIterator)
{
  mRefCounting = rIterator.mRefCounting;
  const nuiContainer* pOld = mpSource;
  mpSource = rIterator.mpSource;
  if (mRefCounting)
  {
    mpSource->Acquire();
    pOld->Release();
  }
  mValid = rIterator.mValid;
  return *this;
}

nuiContainer::ConstIterator& nuiContainer::ConstIterator::operator = (const nuiContainer::ConstIterator& rIterator)
{
  mRefCounting = rIterator.mRefCounting;
  const nuiContainer* pOld = mpSource;
  mpSource = rIterator.mpSource;
  if (mRefCounting)
  {
    mpSource->Acquire();
    pOld->Release();
  }
  mValid = rIterator.mValid;
  return *this;
}

nuiContainer::Iterator::~Iterator()
{
  if (mRefCounting)
    mpSource->Release();
}

nuiContainer::ConstIterator::~ConstIterator()
{
  if (mRefCounting)
    mpSource->Release();
}

bool nuiContainer::Iterator::IsValid() const
{
  return mValid;
}

bool nuiContainer::ConstIterator::IsValid() const
{
  return mValid;
}

void nuiContainer::Iterator::SetValid(bool Valid)
{
  mValid = Valid;
}

void nuiContainer::ConstIterator::SetValid(bool Valid)
{
  mValid = Valid;
}


/////////////////////////////////////////////////////////////
nuiWidgetPtr nuiContainer::Find(const nglString& rName)
{ 
  CheckValid();
  int slash = rName.Find('/'); 

  if (slash >= 0) 
  { 
    nglString path = rName.GetLeft(slash); 
    nglString rest = rName.Extract(slash + 1); 
    nuiWidgetPtr node = SearchForChild(path, false); 

    return node ? node->Find(rest) : NULL; 
  } 
  else 
    return SearchForChild(rName,false); 
} 

void nuiContainer::OnChildHotRectChanged(nuiWidget* pChild, const nuiRect& rChildHotRect)
{
  CheckValid();
  SetHotRect(rChildHotRect);
}

void nuiContainer::InternalResetCSSPass()
{
  CheckValid();
  nuiWidget::InternalResetCSSPass();
  
  IteratorPtr pIt = GetFirstChild(false);
  for (; pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    pItem->ResetCSSPass();
  }
  delete pIt;
}

nuiContainer::IteratorPtr nuiContainer::GetChildIterator(nuiWidgetPtr pChild, bool DoRefCounting)
{
  CheckValid();
  IteratorPtr pIt = GetFirstChild(DoRefCounting);
  while (pIt->IsValid() && pIt->GetWidget() != pChild)
    GetNextChild(pIt);
  return pIt;
}

nuiContainer::ConstIteratorPtr nuiContainer::GetChildIterator(nuiWidgetPtr pChild, bool DoRefCounting) const
{
  CheckValid();
  ConstIteratorPtr pIt = GetFirstChild(DoRefCounting);
  while (pIt->IsValid() && pIt->GetWidget() != pChild)
    GetNextChild(pIt);
  return pIt;
}

nuiWidgetPtr nuiContainer::GetNextFocussableChild(nuiWidgetPtr pChild) const
{
  CheckValid();
  ConstIteratorPtr pIt = pChild ? GetChildIterator(pChild) : GetFirstChild();
  if (!pIt->IsValid())
    return NULL;
  
  if (pChild)
    GetNextChild(pIt);
  
  while (pIt->IsValid() && !pIt->GetWidget()->GetWantKeyboardFocus() && pIt->GetWidget())
    GetNextChild(pIt);
  
  if (pIt->IsValid())
  {
    nuiWidgetPtr pW = pIt->GetWidget();
    delete pIt;
    return pW;
  }
  
  delete pIt;
  return NULL;
}

nuiWidgetPtr nuiContainer::GetPreviousFocussableChild(nuiWidgetPtr pChild) const
{
  CheckValid();
  ConstIteratorPtr pIt = pChild ? GetChildIterator(pChild) : GetLastChild();
  if (!pIt->IsValid())
    return NULL;
  
  if (pChild)
    GetPreviousChild(pIt);
  
  while (pIt->IsValid() && !pIt->GetWidget()->GetWantKeyboardFocus() && pIt->GetWidget())
    GetPreviousChild(pIt);
  
  if (pIt->IsValid())
  {
    nuiWidgetPtr pW = pIt->GetWidget();
    delete pIt;
    return pW;
  }
  
  delete pIt;
  return NULL;
}


nuiWidgetPtr nuiContainer::GetNextSibling(nuiWidgetPtr pChild) const
{
  CheckValid();
  ConstIteratorPtr pIt = pChild ? GetChildIterator(pChild) : GetFirstChild();
  if (!pIt->IsValid())
    return NULL;
  
  if (pChild)
    GetNextChild(pIt);
  
  nuiWidgetPtr pW = NULL;
  if (pIt->IsValid())
    pW = pIt->GetWidget();
  
  delete pIt;
  return pW;
}

nuiWidgetPtr nuiContainer::GetPreviousSibling(nuiWidgetPtr pChild) const
{
  CheckValid();
  ConstIteratorPtr pIt = pChild ? GetChildIterator(pChild) : GetFirstChild();
  if (!pIt->IsValid())
    return NULL;
  
  if (pChild)
    GetPreviousChild(pIt);
  
  nuiWidgetPtr pW = NULL;
  if (pIt->IsValid())
    pW = pIt->GetWidget();
  
  delete pIt;
  return pW;
}

void nuiContainer::SetChildrenLayoutAnimationDuration(float duration)
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    pItem->SetLayoutAnimationDuration(duration);
  }
  delete pIt;
}

void nuiContainer::SetChildrenLayoutAnimationEasing(const nuiEasingMethod& rMethod)
{
  CheckValid();
  IteratorPtr pIt;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    pItem->SetLayoutAnimationEasing(rMethod);
  }
  delete pIt;
}

bool nuiContainer::PreMouseClicked(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}

bool nuiContainer::PreMouseUnclicked(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}

bool nuiContainer::PreMouseMoved(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}

bool nuiContainer::PreMouseWheelMoved(const nglMouseInfo& rInfo)
{
  CheckValid();
  return false;
}


void nuiContainer::GetHoverList(nuiSize X, nuiSize Y, std::set<nuiWidget*>& rHoverSet, std::list<nuiWidget*>& rHoverList) const
{
  CheckValid();
  nuiContainer::ConstIteratorPtr pIt = NULL;
  for (pIt = GetFirstChild(); pIt && pIt->IsValid(); GetNextChild(pIt))
  {
    nuiWidgetPtr pItem = pIt->GetWidget();
    if (pItem->IsInsideFromRoot(X, Y))
    {
      rHoverList.push_back(pItem);
      rHoverSet.insert(pItem);
      nuiContainer* pChild = dynamic_cast<nuiContainer*>(pItem);
      if (pChild)
        pChild->GetHoverList(X, Y, rHoverSet, rHoverList);
    }
  }
  delete pIt;
}

