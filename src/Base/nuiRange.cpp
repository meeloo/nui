/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"
#include <cmath>

nuiRange::nuiRange(double Value, double Min, double Max, double Increment, double PageIncrement, double PageSize, double Origin, float UnitCurve)
{
  mEvents = true;
  mDiscreetStepSize = false;
  mMinimum = Min;
  mMaximum = Max;
  mPageSize = PageSize;
  mUnitCurve = UnitCurve;

  if (mMinimum < mMaximum)
  {
    if (mPageSize > mMaximum - mMinimum)
      mPageSize = mMaximum - mMinimum;
  }
  else
  {
    if (mPageSize > mMinimum - mMaximum)
      mPageSize = mMinimum - mMaximum;
  }
  
  mIncrement = Increment;
  mPageIncrement = PageIncrement;
  
  mValue = Value;
  if (isfinite(Value))
  {
    SetValue(Value);
  }

  if (std::isnan(Origin))
  {
    mOrigin = Min;
  }
  else
  {
    mOrigin = Origin;
  }
}

nuiRange::nuiRange(const nuiRange& rRange)
{
  mEvents = true;
  mDiscreetStepSize = rRange.mDiscreetStepSize;
  mMinimum       = rRange.mMinimum;     
  mMaximum       = rRange.mMaximum;     
  mPageSize      = rRange.mPageSize;    
  mValue         = rRange.mValue;       
  mIncrement     = rRange.mIncrement;   
  mPageIncrement = rRange.mPageIncrement;
  mOrigin        = rRange.mOrigin;
  mUnitCurve     = rRange.mUnitCurve;
}

nuiRange& nuiRange::operator=(const nuiRange& rRange)
{
  mEvents = true;
  mDiscreetStepSize = rRange.mDiscreetStepSize;
  mMinimum       = rRange.mMinimum;     
  mMaximum       = rRange.mMaximum;     
  mPageSize      = rRange.mPageSize;    
  mValue         = rRange.mValue;       
  mIncrement     = rRange.mIncrement;   
  mPageIncrement = rRange.mPageIncrement;
  mOrigin        = rRange.mOrigin;
  mUnitCurve     = rRange.mUnitCurve;
  return *this;
}


nuiRange::~nuiRange()
{
}

bool nuiRange::IsValid() const
{
  
  if (std::isnan(mMinimum)
  |   std::isnan(mMaximum)
  |   std::isnan(mPageSize)
  |   std::isnan(mValue)
  |   std::isnan(mIncrement)
  |   std::isnan(mPageIncrement)
  |   std::isnan(mOrigin)
  |   std::isnan(mUnitCurve)
      )
  {
    return false;
  }
  
  return true;
}


void nuiRange::SetDiscreetStepSize(bool DiscreetStepSize)
{
  mDiscreetStepSize = DiscreetStepSize;
}

void nuiRange::SetUnitValue(double Value)
{
  NGL_ASSERT(IsValid());
  NGL_ASSERT(std::isfinite(Value));
  SetValue(ConvertFromUnit(nuiClamp(Value, 0.0, 1.0)));
}

void nuiRange::SetValue(double Value)
{
  NGL_ASSERT(std::isfinite(Value));
  
  double OldValue = mValue;

  mValue = Value;

  if (mDiscreetStepSize)
  {
    double diff = mValue - OldValue;
    if (fabsf(diff) >= mIncrement / 2.f)
    {
      if (mIncrement > 0)
      {
        mValue = (double)((double)OldValue + (ToNearest((double)diff / (double)mIncrement) * (double)mIncrement));
      }
    }
    else
    {
      mValue = OldValue;
    }
  }

  if (mMinimum <= mMaximum)
  {
    if (mValue < mMinimum)
      mValue = mMinimum;
    else if (mValue + mPageSize > mMaximum)
      mValue = mMaximum - mPageSize;
    if (mValue < mMinimum)
      mValue = mMinimum;
  }
  else
  {
    if (mValue > mMinimum)
      mValue = mMinimum;
    else if (mValue - mPageSize < mMaximum)
      mValue = mMaximum + mPageSize;
    if (mValue > mMinimum)
      mValue = mMinimum;
  }

  if (OldValue == mValue)
    return;

  if (mEvents)
  {
    ValueChanged();
    Changed();
  }
}

void nuiRange::SetRange(double Minimum,double Maximum)
{
  if (mMinimum == Minimum && mMaximum == Maximum)
    return;
  mMinimum = Minimum;
  mMaximum = Maximum;
  if (mEvents)
    Changed();
}

void nuiRange::SetIncrement(double Increment)
{
  if (mIncrement == Increment)
    return;
  mIncrement = Increment;
  if (mEvents)
    Changed();
}

void nuiRange::SetPageIncrement(double PageIncrement)
{
  if (PageIncrement == mPageIncrement)
    return;
  mPageIncrement = PageIncrement;
  if (mEvents)
    Changed();
}

void nuiRange::SetPageSize(double PageSize)
{
  double OldPageSize = mPageSize;

  mPageSize = PageSize;

  if (mMinimum < mMaximum)
  {
    if (mPageSize > mMaximum - mMinimum)
      mPageSize = mMaximum - mMinimum;
  }
  else
  {
    if (mPageSize > mMinimum - mMaximum)
      mPageSize = mMinimum - mMaximum;
  }

  if (OldPageSize == mPageSize)
    return;
  if (mEvents)
    Changed();
}

void nuiRange::SetOrigin(double Origin)
{
  double OldOrigin = mOrigin;

  mOrigin = Origin;

  if (OldOrigin == mOrigin)
    return;

  if (mEvents)
    Changed();
}

void nuiRange::SetValueAndSize(double Value, double PageSize)
{
  
  //Value
  double OldValue = mValue;
  mValue = Value;
  
  //Size
  double OldPageSize = mPageSize;
  mPageSize = PageSize;
  
  
  if (mMinimum < mMaximum)
  {
    if (mPageSize > mMaximum - mMinimum)
      mPageSize = mMaximum - mMinimum;
  }
  else
  {
    if (mPageSize > mMinimum - mMaximum)
      mPageSize = mMinimum - mMaximum;
  }
  
  if (mDiscreetStepSize)
  {
    double diff = mValue - OldValue;
    if (fabsf(diff) >= mIncrement / 2.f)
    {
      if (mIncrement > 0)
      {
        mValue = (double)((double)OldValue + (ToNearest((double)diff / (double)mIncrement) * (double)mIncrement));
      }
    }
    else
    {
      mValue = OldValue;
    }
  }
  
  if (mMinimum <= mMaximum)
  {
    if (mValue < mMinimum)
      mValue = mMinimum;
    else if (mValue + mPageSize > mMaximum)
      mValue = mMaximum - mPageSize;
    if (mValue < mMinimum)
      mValue = mMinimum;
  }
  else
  {
    if (mValue > mMinimum)
      mValue = mMinimum;
    else if (mValue - mPageSize < mMaximum)
      mValue = mMaximum + mPageSize;
    if (mValue > mMinimum)
      mValue = mMinimum;
  }

  if ((OldPageSize == mPageSize) && (OldValue == mValue))
    return;
  
  //Events
  if (mEvents)
  {
    ValueChanged();
    Changed();
  }
  
}

void nuiRange::Increment()
{
  if (mValue + mIncrement > mMaximum)
    SetValue(mMaximum);
  else
    SetValue((double)((double)mValue + (double)mIncrement));
}

void nuiRange::Decrement()
{
  if (mValue + mIncrement < mMinimum)
    SetValue(mMinimum);
  else
    SetValue((double)((double)mValue - (double)mIncrement));
}

void nuiRange::PageIncrement()
{
  if (mValue + mPageIncrement > mMaximum)
    SetValue(mMaximum);
  else
    SetValue((double)((double)mValue + (double)mPageIncrement));
}

void nuiRange::PageDecrement()
{
  if (mValue - mPageIncrement < mMinimum)
    SetValue(mMinimum);
  else
    SetValue((double)((double)mValue - (double)mPageIncrement));
}

double nuiRange::GetValue() const
{
  return mValue;
}

double nuiRange::GetUnitValue() const
{
  NGL_ASSERT(IsValid());
  const double v = ConvertToUnit(GetValue());
  NGL_ASSERT(std::isfinite(v));
  return v;
}

double nuiRange::GetMinimum() const
{
  return mMinimum;
}

double nuiRange::GetMaximum() const
{
  return mMaximum;
}

double nuiRange::GetIncrement() const
{
  return mIncrement;
}

double nuiRange::GetPageIncrement() const
{
  return mPageIncrement;
}

double nuiRange::GetPageSize() const
{
  return mPageSize;
}

double nuiRange::GetOrigin() const
{
  return mOrigin;
}

double nuiRange::GetRange() const
{
  return mMaximum - mMinimum;
}

bool nuiRange::MakeInRange(double Position, double size)
{
  double tmp = mValue;
  double val = mValue;
  if (mMinimum < mMaximum)
  {
    if (Position < mMinimum)
      Position = mMinimum;
    if (Position + size >= mMaximum)
      size = MAX(0, mMaximum - Position);
    
    double end = mValue + mPageSize;
    double newend = Position + size;
    if (newend > end)
      val += newend - end;

    if (Position < mValue)
      val = Position;

    end = val + mPageSize;
    if (newend > end)
      val += newend - end;

    if (val < mMinimum)
      val = mMinimum;
    SetValue(val);
  }
  else
  {
    nuiRange bis;
    bis.mMaximum = -mMaximum;
    bis.mMinimum = -mMinimum;
    bis.mPageSize = mPageSize;
    bis.mValue = -mValue;
    bis.mIncrement = -mIncrement;
    bis.mPageIncrement = -mPageIncrement;
    bis.mOrigin = -mOrigin;
    bis.mUnitCurve = mUnitCurve;
    bis.mDiscreetStepSize = mDiscreetStepSize;
    bis.mEvents = mEvents;

    bis.MakeInRange(-Position, size);
    SetValue(-bis.GetValue());
  }
  return tmp != mValue;
}

bool nuiRange::MakeInRangeVisual(double Position, double size)
{
  double tmp = mValue;

  if (Position >= mValue && ((Position+size) <= (mValue+mPageSize)))
    return tmp != mValue;

  if (mMinimum < mMaximum)
  {
    if (Position < mMinimum)
    {
      Position = mMinimum;
    }
    if (Position + size >= mMaximum)
    {
      Position = MAX(0, mMaximum - mPageSize);
    }

    if (size > mPageSize)
    {
      // Focus on start shifted down to give some context
      SetValue(Position + mPageSize * 0.1);
      return tmp != mValue;
    }

    // Center
    SetValue(Position + (size * 0.5) - mPageSize * 0.5);
  }
  else
  {
    if (mMaximum != mMinimum)
    {
      nuiRange bis;
      bis.mMaximum = -mMaximum;
      bis.mMinimum = -mMinimum;
      bis.mPageSize = mPageSize;
      bis.mValue = -mValue;
      bis.mIncrement = -mIncrement;
      bis.mPageIncrement = -mPageIncrement;
      bis.mOrigin = -mOrigin;
      bis.mUnitCurve = mUnitCurve;
      bis.mDiscreetStepSize = mDiscreetStepSize;
      bis.mEvents = mEvents;

      bis.MakeInRangeVisual(-Position, size);
      SetValue(-bis.GetValue());
    }
    else
      SetValue(mMinimum);
  }
  return tmp != mValue;
}


double nuiRange::ConvertToUnit(double RangeValue) const
{
  NGL_ASSERT(IsValid());
  NGL_ASSERT(std::isfinite(RangeValue));
  double t = RangeValue - mMinimum;
  double r = mMaximum - mMinimum;
  double v = t / r;
  return pow(v, mUnitCurve);
}

double nuiRange::ConvertFromUnit(double UnitValue) const
{
  NGL_ASSERT(IsValid());
  NGL_ASSERT(std::isfinite(UnitValue));
  double r = mMaximum - mMinimum;
  double v = pow(UnitValue, 1.0 / mUnitCurve);
  return v * r + mMinimum;
}

void nuiRange::EnableEvents(bool Set)
{
  mEvents = Set;
}

bool nuiRange::AreEventsEnabled()
{
  return mEvents;
}


void nuiRange::ToString(nglString& str)
{
//double Value, double Min, double Max, double Increment, double PageIncrement, double PageSize, double Origin
  str.CFormat("[%g %g %g %g %g %g %g]", 
    mValue, mMinimum, mMaximum, mIncrement, mPageIncrement, mPageSize, mOrigin);
}

bool nuiRange::FromString(const nglString& Value)
{
  double value=0,min=0,max=0,increment=0,pageincrement=0,pagesize=0,origin=0;
  long res;
  nglString str=Value;
  if (!str.GetLength() || str[0]!='[' || str[str.GetLength()-1]!=']')
    return false;
    
  str.DeleteLeft(1);
  str.DeleteRight(1);
  std::vector<nglString> vec;
  res = str.Tokenize(vec);
  
  if (vec.size() == 7)
  {
    value         = vec[0].GetCDouble();
    min           = vec[1].GetCDouble();
    max           = vec[2].GetCDouble();
    increment     = vec[3].GetCDouble();
    pageincrement = vec[4].GetCDouble();
    pagesize      = vec[5].GetCDouble();
    origin        = vec[6].GetCDouble();
    
    SetValue(value);
    SetRange(min,max);
    SetIncrement(increment);
    SetPageIncrement(pageincrement);
    SetPageSize(pagesize);
    SetOrigin(origin);
    return true;
  }
  return false;
}


