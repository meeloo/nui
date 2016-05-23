/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"

/*
* Life cycle
*/

nglStream::nglStream()
{
  mEndian = eEndianNative;
  mTextFormat = eTextNative;
  mTextEncoding = eEncodingNative;
  mpConv = NULL;
}

nglStream::~nglStream()
{
  delete mpConv;
}


/*
* Endianness
*/

nglEndian nglStream::GetEndian() const
{
  return mEndian;
}

void nglStream::SetEndian (nglEndian nglEndian)
{
  mEndian = nglEndian;
}


/*
* Text encoding and line-ending convention
*/

nglTextEncoding nglStream::GetTextEncoding()
{
  return mTextEncoding;
}

bool nglStream::SetTextEncoding (nglTextEncoding Encoding)
{
  if (mTextEncoding != Encoding)
  {
    mTextEncoding = Encoding;

    // Invalidate current conversion context
    if (mpConv)
    {
      delete mpConv;
      mpConv = NULL;
    }
  }

  return true;
}

nglTextFormat nglStream::GetTextFormat()
{
  return mTextFormat;
}

void nglStream::SetTextFormat (nglTextFormat Format)
{
  mTextFormat = Format;
}
