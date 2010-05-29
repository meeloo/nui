/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"
#include "nuiHTMLTable.h"

// class nuiHTMLTable : public nuiHTMLItem
nuiHTMLTable::nuiHTMLTable(nuiHTMLNode* pNode, nuiHTMLNode* pAnchor, bool Inline)
: nuiHTMLItem(pNode, pAnchor, Inline)
{
  mDefaultCell.SetContents(pNode, NULL);

  mDefaultCell.mRequestedWidth = -1;
  mDefaultCell.mRequestedHeight = -1;
  mDefaultCell.mRequestedWidthUnit = ePercentage;
  mDefaultCell.mRequestedHeightUnit = ePercentage;

  SetRowCount(1);
  SetColCount(1);
}

nuiHTMLTable::~nuiHTMLTable()
{
  for (uint32 i = 0; i < GetRowCount(); i++)
  {
    for (uint32 j = 0; j < GetColCount(); j++)
    {
      nuiHTMLItem* pItem = GetCell(j, i).mpItem;
      delete pItem;
    }
  }
}

void nuiHTMLTable::Draw(nuiDrawContext* pContext)
{
  mDefaultCell.Draw(pContext);
  
  for (uint32 i = 0; i < GetRowCount(); i++)
  {
    for (uint32 j = 0; j < GetColCount(); j++)
    {
      GetCell(j, i).Draw(pContext);
    }
  }
}

void nuiHTMLTable::Layout(nuiHTMLContext& rContext)
{
  nuiHTMLContext ctx(rContext);
  float MaxWidth = ctx.mMaxWidth;
  if (mDefaultCell.mRequestedWidth >= 0)
  {
    switch (mDefaultCell.mRequestedWidthUnit)
    {
      case ePixels:
        MaxWidth = mDefaultCell.mRequestedWidth;
        break;
      case ePercentage:
        MaxWidth = (mDefaultCell.mRequestedWidth * MaxWidth) * 0.01f;
        break;
      case eProportional:
        MaxWidth = MaxWidth;
        break;
      default:
        NGL_ASSERT(0);
        break;
    } 
  }
  
  MaxWidth -= (mDefaultCell.mSpacing + mDefaultCell.mPadding + mDefaultCell.mBorder) * 2;
  
  // Calculate default ideal sizes:
  for (int32 i = 0; i < GetRowCount(); i++)
  {
    for (int32 j = 0; j < GetColCount(); j++)
    {      
      Cell& rCell(GetCell(j, i));
      if (mColumns[j].mRequestedSize >= 0)
      {
        switch (mColumns[j].mRequestedSizeUnit)
        {
          case ePixels:
            ctx.mMaxWidth = mColumns[j].mRequestedSize;
            break;
          case ePercentage:
            ctx.mMaxWidth = mColumns[j].mRequestedSize * MaxWidth * 0.01f;
            break;
          case eProportional:
            ctx.mMaxWidth = MaxWidth;
            break;
          default:
            NGL_ASSERT(0);
            break;
        } 
      }

      rCell.Layout(ctx);
      float w = rCell.mIdealWidth;
      if (rCell.mColSpan > 1)
        w /= rCell.mColSpan;
      mColumns[j].mIdealSize = MAX(mColumns[j].mIdealSize, w);
      mRows[i].mSize = MAX(mRows[i].mSize, rCell.mIdealHeight);
    }
  }

  float idealw = 0;
  float variableidealw = 0;
  float wratio = 0;
  for (int32 i = 0; i < GetColCount(); i++)
  {
    idealw += mColumns[i].mIdealSize;
    if (mColumns[i].mRequestedSizeUnit == eProportional)
    {
      float r = mColumns[i].mRequestedSize;
      float s =  mColumns[i].mIdealSize;
      wratio += s * r;
    }
  }

  float wdiff = (idealw - MaxWidth) / wratio;
  
  for (int32 i = 0; i < GetColCount(); i++)
  {
    switch (mColumns[i].mRequestedSizeUnit)
    {
      case ePixels:
      case ePercentage:
        mColumns[i].mSize = mColumns[i].mIdealSize;
        break;
      case eProportional:
        mColumns[i].mSize = mColumns[i].mIdealSize - (mColumns[i].mRequestedSize * mColumns[i].mIdealSize * wdiff);
        if (mColumns[i].mSize < 0)
          mColumns[i].mSize = 0;
        break;
      default:
        NGL_ASSERT(0);
        break;
    } 
  }

  for (int32 i = 0; i < GetRowCount(); i++)
  {
    float rowheight = 0;
    for (int32 j = 0; j < GetColCount(); j++)
    {
      ctx.mMaxWidth = mColumns[j].mSize;
      Cell& rCell(GetCell(j, i));
      rCell.Layout(ctx);
      if (ctx.mMaxWidth < rCell.mIdealWidth)
        mColumns[j].mSize = rCell.mIdealWidth;
      float h = rCell.mIdealHeight;
      rowheight = MAX(h, rowheight);
    }
    mRows[i].mSize = rowheight;
  }
  
  float x = 0;
  float y = mDefaultCell.mSpacing + mDefaultCell.mBorder;
  for (int32 i = 0; i < GetRowCount(); i++)
  {
    float rowheight = mRows[i].mSize;
    x = mDefaultCell.mSpacing + mDefaultCell.mBorder;

    for (int32 j = 0; j < GetColCount(); j++)
    {
      float w = mColumns[j].mSize;
      Cell& rCell(GetCell(j, i));
      
      nuiRect r(x, y, w, rowheight);
      //NGL_OUT(_T("Cell[%d,%d] -> %ls\n"), j, i, r.GetValue().GetChars());
      rCell.SetLayout(r);
      x += w;
    }
    y += rowheight;
  }
  
  x += (mDefaultCell.mSpacing + mDefaultCell.mBorder);
  y += (mDefaultCell.mSpacing + mDefaultCell.mBorder);
  mIdealRect = nuiRect(x, y);
  mIdealRect.RoundToBiggest();
  mDefaultCell.mIdealWidth = mIdealRect.GetWidth();
  mDefaultCell.mIdealHeight = mIdealRect.GetHeight();
}

void nuiHTMLTable::SetLayout(const nuiRect& rRect)
{
  nuiHTMLItem::SetLayout(rRect);
  mDefaultCell.mWidth = mRect.GetWidth();
  mDefaultCell.mHeight = mRect.GetHeight();

}

void nuiHTMLTable::SetRowCount(uint32 count)
{
  int32 old = GetRowCount();
  if (count == old)
    return;
  mCells.resize(count);
  mRows.resize(count);

  int32 cols = GetColCount();
  for (; old < count; old++)
    mCells[old].resize(cols, mDefaultCell);
  
  InvalidateLayout();
}

void nuiHTMLTable::SetColCount(uint32 count)
{
  if (count == GetColCount())
    return;
  for (uint32 i = 0; i < mCells.size(); i++)
    mCells[i].resize(count, mDefaultCell);
  mColumns.resize(count);
  InvalidateLayout();
}

uint32 nuiHTMLTable::GetRowCount() const
{
  return mCells.size();
}

uint32 nuiHTMLTable::GetColCount() const
{
  return mColumns.size();
}

void nuiHTMLTable::Grow(int32 col, int32 row)
{
  bool changed = false;
  if (col + 1 > GetColCount())
    SetColCount(col + 1);
  if (row + 1 > GetRowCount())
    SetRowCount(row + 1);
}

void nuiHTMLTable::SetCellSpan(int32 col, int32 row, int32 ncols, int32 nrows)
{
  ncols = MAX(ncols, 1);
  nrows = MAX(nrows, 1);
  Grow(col + ncols - 1, row + nrows - 1);
  
  Cell& rCell(GetCell(col, row));
  rCell.mColSpan = ncols;
  rCell.mRowSpan = nrows;
  for (int32 i = 0; i < nrows; i++)
  {
    for (int32 j = 0; j < nrows; j++)
    {
      if (i && j)
      {
        Cell& rSlave(GetCell(col + j - 1, row + i - 1));
        rSlave = Cell();
        rSlave.mpMasterCell = &rCell;
      }
    }
  }
  
  InvalidateLayout();
}


nuiHTMLTable::Cell& nuiHTMLTable::SetCell(int32 col, int32 row, nuiHTMLNode* pNode, nuiHTMLItem* pItem)
{
  Grow(col, row);
  
  Cell& rCell(mCells[row][col]);
  rCell.SetContents(pNode, pItem);

  nuiHTMLAttrib* pAttrib = NULL;
  
  int32 RowSpan = 1;
  int32 ColSpan = 1;
  
  pAttrib = pNode->GetAttribute(nuiHTMLAttrib::eAttrib_COLSPAN);
  if (pAttrib)
    ColSpan = pAttrib->GetValue().GetCInt();
  
  pAttrib = pNode->GetAttribute(nuiHTMLAttrib::eAttrib_ROWSPAN);
  if (pAttrib)
    RowSpan = pAttrib->GetValue().GetCInt();
  
  if (RowSpan > 1 || ColSpan > 1)
    SetCellSpan(col, row, ColSpan, RowSpan);
  
  return rCell;
}

nuiHTMLTable::Cell& nuiHTMLTable::GetCell(int32 col, int32 row)
{
  NGL_ASSERT(col < mColumns.size());
  NGL_ASSERT(row < mRows.size());
  return mCells[row][col];
}

const nuiHTMLTable::Cell& nuiHTMLTable::GetCell(int32 col, int32 row) const
{
  return mCells[row][col];
}

void nuiHTMLTable::GetItemsAt(std::vector<nuiHTMLItem*>& rHitItems, float X, float Y) const
{
  for (uint32 i = 0; i < GetRowCount(); i++)
  {
    for (uint32 j = 0; j < GetColCount(); j++)
    {
      nuiHTMLItem* pItem = GetCell(j, i).mpItem;
      if (pItem && pItem->IsInside(X, Y))
        rHitItems.push_back(pItem);
    }
  }
}

// class nuiHTMLTable::Cell
nuiHTMLTable::Cell::Cell()
: mpMasterCell(NULL),
  mColSpan(1), mRowSpan(1),
  mpItem(NULL),
  mIdealWidth(-1), mIdealHeight(-1),
  mRequestedWidth(-1), mRequestedHeight(-1),
  mWidth(0), mHeight(0),
  mRequestedWidthUnit(ePixels), mRequestedHeightUnit(ePixels),
  mHeader(false),
  mFrame(eBorder),
  mBorder(1),
  mSpacing(1),
  mPadding(2)
{
  
}

nuiHTMLTable::Cell::~Cell()
{
}
  
void nuiHTMLTable::Cell::SetContents(nuiHTMLNode* pNode, nuiHTMLItem* pItem)
{
  mpNode = pNode;
  delete mpItem;
  mpItem = pItem;
  
  nuiHTMLAttrib* pAttrib = NULL;
  pAttrib = pNode->GetAttribute(nuiHTMLAttrib::eAttrib_BORDER);
  if (pAttrib)
    mBorder = pAttrib->GetValue().GetCFloat();
  
  pAttrib = pNode->GetAttribute(nuiHTMLAttrib::eAttrib_CELLPADDING);
  if (pAttrib)
    mPadding = pAttrib->GetValue().GetCFloat();
  
  pAttrib = pNode->GetAttribute(nuiHTMLAttrib::eAttrib_CELLSPACING);
  if (pAttrib)
    mSpacing = pAttrib->GetValue().GetCFloat();
  
  pAttrib = pNode->GetAttribute(nuiHTMLAttrib::eAttrib_WIDTH);
  if (pAttrib)
  {  
    const nglString &v(pAttrib->GetValue());
    mRequestedWidth = v.GetCFloat();
    
    if (v.Contains(_T("%")))
      mRequestedWidthUnit = ePercentage;
    else if (v.Contains(_T("*")))
      mRequestedWidthUnit = eProportional;
    else if (v.Contains(_T("px"), false))
      mRequestedWidthUnit = ePixels;
  }
  
  pAttrib = pNode->GetAttribute(nuiHTMLAttrib::eAttrib_HEIGHT);
  if (pAttrib)
  {  
    const nglString &v(pAttrib->GetValue());
    mRequestedHeight = v.GetCFloat();
    
    if (v.Contains(_T("%")))
      mRequestedHeightUnit = ePercentage;
    else if (v.Contains(_T("*")))
      mRequestedHeightUnit = eProportional;
    else if (v.Contains(_T("px"), false))
      mRequestedHeightUnit = ePercentage;
  }
  
  pAttrib = pNode->GetAttribute(nuiHTMLAttrib::eAttrib_FRAME);
  if (pAttrib)
  {
    nglString v(pAttrib->GetValue());
    v.Trim();
    if (v.Compare(_T("void"), false) == 0)
      mFrame = eVoid;
    else if (v.Compare(_T("above"), false) == 0)
      mFrame = eAbove;
    else if (v.Compare(_T("below"), false) == 0)
      mFrame = eBelow;
    else if (v.Compare(_T("hsides"), false) == 0)
      mFrame = eHSides;
    else if (v.Compare(_T("vsides"), false) == 0)
      mFrame = eVSides;
    else if (v.Compare(_T("lhs"), false) == 0)
      mFrame = eLHS;
    else if (v.Compare(_T("rhs"), false) == 0)
      mFrame = eRHS;
    else if (v.Compare(_T("box"), false) == 0)
      mFrame = eBox;
    else if (v.Compare(_T("border"), false) == 0)
      mFrame = eBorder;
  }

  /*
  pAttrib = pNode->GetAttribute(nuiHTMLAttrib::eAttrib_RULES);
  if (pAttrib)
  {
    nglString v(pAttrib->GetValue());
    v.Trim();
    if (v.Compare(_T("none"), false) == 0)
      mRules = eNone;
    else if (v.Compare(_T("groups"), false) == 0)
      mRules = eGroups;
    else if (v.Compare(_T("rows"), false) == 0)
      mRules = eRows;
    else if (v.Compare(_T("cols"), false) == 0)
      mRules = eCols;
    else if (v.Compare(_T("all"), false) == 0)
      mRules = eAll;
  }
  */
  
}

void nuiHTMLTable::Cell::SetLayout(const nuiRect& rRect)
{
  mX = rRect.Left() + mSpacing + mBorder;
  mY = rRect.Top() + mSpacing + mBorder;
  mWidth = rRect.GetWidth() - (mSpacing + mBorder) * 2;
  mHeight = rRect.GetHeight() - (mSpacing + mBorder) * 2;
  if (mpItem)
  {
    nuiRect r(mX + mPadding, mY + mPadding, mWidth - mPadding * 2, mHeight - mPadding * 2);
    mpItem->SetLayout(r);
  }
}

void nuiHTMLTable::Cell::Layout(nuiHTMLContext& rCtx)
{
  nuiHTMLContext ctx(rCtx);
  ctx.mMaxWidth -= (mSpacing + mPadding) * 2;
  if (!mpItem)
  {
    mIdealWidth = 0;
    mIdealHeight = 0;
  }

  float rw = 0;
  if (mRequestedWidth >= 0)
  {
    switch (mRequestedWidthUnit)
    {
      case ePixels:
        rw = mRequestedWidth;
        break;
      case ePercentage:
        rw = mRequestedWidth * rCtx.mMaxWidth * 0.01f;
        break;
      case eProportional:
        rw = 0;
        break;
      default:
        NGL_ASSERT(0);
        break;
    }
    
    if (rw > ctx.mMaxWidth)
      ctx.mMaxWidth = rw;
  }
  
  nuiRect r;
  if (mpItem)
  {
    mpItem->Layout(rCtx);
    r = mpItem->GetIdealRect();
  }
  r.RoundToBiggest();
  mIdealWidth = r.GetWidth() + (mSpacing + mPadding + mBorder) * 2;
  mIdealHeight = r.GetHeight() + (mSpacing + mPadding + mBorder) * 2;
  
  if (mRequestedWidth >= 0)
  {
    if (rw > mIdealWidth)
      mIdealWidth = rw;
  }
  
  if (mRequestedHeight >= 0)
  {
    float rr = 0;
    switch (mRequestedHeightUnit)
    {
      case ePixels:
        rr = mRequestedHeight;
        break;
      case ePercentage:
        rr = 0;
        break;
      case eProportional:
        rr = 0;
        break;
      default:
        NGL_ASSERT(0);
        break;
    }
    
    if (rr > mIdealHeight)
      mIdealHeight = rr;
  }
}


void nuiHTMLTable::Cell::Draw(nuiDrawContext* pContext)
{
  if (mBorder > 0)
  {
    pContext->PushState();
    pContext->SetLineWidth(mBorder);

    if (mFrame & 1)
      pContext->DrawLine(mX, mY, mX, mY + mHeight);
    if (mFrame & 2)
      pContext->DrawLine(mX, mY, mX + mWidth, mY);
    if (mFrame & 4)
      pContext->DrawLine(mX + mWidth, mY, mX + mWidth, mY + mHeight);
    if (mFrame & 8)
      pContext->DrawLine(mX, mY + mHeight, mX + mWidth, mY + mHeight);
    
    pContext->PopState();
  }
  if (mpItem)
    mpItem->CallDraw(pContext);
  
}

// class Col
nuiHTMLTable::Col::Col()
{
  mRequestedSize = 1;
  mRequestedSizeUnit = eProportional;
  mSize = 0;
  mIdealSize = 0;
}


// class Row
nuiHTMLTable::Row::Row()
{
  mSize = 0;
}

