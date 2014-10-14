/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

#if 0
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

nuiContainer::nuiContainer()
: nuiContainer()
{
  SetObjectClass(_T("nuiContainer"));
}

#if 0
bool nuiContainer::Load(const nuiXMLNode* pNode)
{
	bool res=true;
	res &= LoadAttributes(pNode);
	res &= LoadChildren(pNode);
  return res;
}


bool nuiContainer::LoadChildren(const nuiXMLNode* pNode)
{
  CheckValid();
	bool res=true;
  int i, count = pNode->GetChildrenCount();
  for (i = 0; i < count; i++)
  {
    nuiXMLNode* pChild = pNode->GetChild(i);
    nuiWidget* pWidget = nuiBuilder::Get().CreateWidget(pChild->GetName());
    if (!pWidget)
    {
      // If the child is not a creatable element then may be a text property of the object.
      nuiXMLNode* pText = pChild->GetChild(nglString("##text"));
      if (pText)
        SetProperty(pChild->GetName(),pText->GetValue());
    }
    else
    {
      pWidget->Load(pChild);
      AddChild(pWidget);
    }
  }
  
  return res;
}
#endif

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

// We need to by pass nuiContainer as is has a special implementation of SetObjectXXX in order to prevent calling pure virtual methods
bool nuiContainer::SetObjectClass(const nglString& rName)
{
  return nuiWidget::SetObjectClass(rName);
}

// We need to by pass nuiContainer as is has a special implementation of SetObjectXXX in order to prevent calling pure virtual methods
void nuiContainer::SetObjectName(const nglString& rName)
{
  nuiWidget::SetObjectName(rName);
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

nuiWidgetPtr nuiContainer::GetChild(nuiSize X, nuiSize Y)
{
  return nuiContainer::GetChild(X, Y);
}

nuiWidgetPtr nuiContainer::GetChild(const nglString& rName, bool ResolveNameAsPath)
{
  return nuiContainer::GetChild(rName, ResolveNameAsPath);
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

#endif

