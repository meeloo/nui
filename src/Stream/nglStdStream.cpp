//
//  nglStdStream.cpp
//  nui3
//
//  Created by vince on 1/21/15.
//  Copyright (c) 2015 libNUI. All rights reserved.
//

#include "nui.h"
#include "nglStdStream.h"

inline nglStreamWhence seekDirToStreamWhence(std::ios_base::seekdir direction)
{
  switch (direction)
  {
  case std::ios_base::beg:
    return eStreamFromStart;
  case std::ios_base::cur:
    return eStreamForward;
  case std::ios_base::end:
    return eStreamFromEnd;
  default:
    {
      NGL_ASSERT(0);
    }
  }
  return eStreamForward;
}


#pragma mark nglIStream wrapper

nglStdIStreamBuf::nglStdIStreamBuf(nglIStream* stream)
: mpIStream(stream)
{
  NGL_ASSERT(stream);
}

std::streambuf* nglStdIStreamBuf::setbuf(char* pNotImplemented, std::streamsize notImplemented)
{
  NGL_ASSERT(0);
  return nullptr;
}

std::streampos nglStdIStreamBuf::seekoff(std::streamoff off,
                                         std::ios_base::seekdir way,
                                         std::ios_base::openmode which)
{
  nglStreamWhence whence = seekDirToStreamWhence(way);
  off_t newPos = mpIStream->SetPos((off_t)off, whence);

  if (newPos >= 0)
  {
    return (std::streampos) newPos;
  }
  NGL_ASSERT(0);
  return -1;
}

std::streampos nglStdIStreamBuf::seekpos(std::streampos sp, std::ios_base::openmode which)
{
  if (!(which & std::ios_base::in))
  {
    NGL_ASSERT(0);
    return -1;
  }

  off_t newPos = mpIStream->SetPos((off_t) sp);

  if (newPos >= 0)
  {
    return (std::streampos) newPos;
  }
  NGL_ASSERT(0);
  return -1;
}

std::streamsize nglStdIStreamBuf::showmanyc()
{
  return mpIStream->Available();
}

int nglStdIStreamBuf::sync()
{
  NGL_ASSERT(0);
  return 0;
}

std::streamsize nglStdIStreamBuf::xsgetn(char *s, std::streamsize n)
{
  std::streamsize read = mpIStream->Read((void *) s, (size_t) n, 1);
  return read;
}

int nglStdIStreamBuf::underflow()
{
  char ch;
  std::streamsize read = mpIStream->Peek(&ch, 1, 1);

  if (read == 1)
  {
    return traits_type::to_int_type(ch);
  }
  return traits_type::eof();
}

int nglStdIStreamBuf::uflow()
{
  char ch;
  std::streamsize read = mpIStream->Read(&ch, 1, 1);
  
  if (read == 1)
  {
    return traits_type::to_int_type(ch);
  }
  
  return traits_type::eof();
}

int nglStdIStreamBuf::pbackfail(int c)
{
  NGL_ASSERT(0); // Not implemented
//  if (c != EOF)
//  {
//  }
  return traits_type::eof();
}

#pragma mark -
#pragma mark nglStdIStream

nglStdIStream::nglStdIStream(nglIStream* stream, bool OwnStream)
: std::istream(NULL),
  mOwnStream(OwnStream),
  mpIStream(stream),
  mStreamBuffer(stream)
{
  std::ios::init(&mStreamBuffer);
}

nglStdIStream::~nglStdIStream()
{
  if (mOwnStream)
    delete mpIStream;
}


#pragma mark nglOStream wrapper

nglStdOStreamBuf::nglStdOStreamBuf(nglOStream* stream)
: mpOStream(stream)
{
}

std::streambuf* nglStdOStreamBuf::setbuf(char* pNotImplemented, std::streamsize notImplemented)
{
  NGL_ASSERT(0);
  return nullptr;
}

std::streampos nglStdOStreamBuf::seekoff(std::streamoff off,
                                         std::ios_base::seekdir way,
                                         std::ios_base::openmode which)
{
  nglStreamWhence whence = seekDirToStreamWhence(way);

  if (!(which & std::ios_base::out))
  {
    NGL_ASSERT(0);
    return -1;
  }

  off_t newPos = mpOStream->SetPos((off_t) off, whence);

  if (newPos >= 0)
  {
    return (std::streampos) newPos;
  }
  return -1;
}

std::streampos nglStdOStreamBuf::seekpos(std::streampos sp, std::ios_base::openmode which)
{
  if (!(which & std::ios_base::out))
    return -1;

  off_t newPos = mpOStream->SetPos((off_t) sp);
  
  if (newPos >= 0)
    return (std::streampos) newPos;
  return -1;
}

std::streamsize nglStdOStreamBuf::xsputn(const char *s, std::streamsize n)
{
  std::streamsize written = mpOStream->Write((const void *)s, (size_t)n, 1);
  return written;
}

int nglStdOStreamBuf::overflow(int c)
{
  if (c == traits_type::eof())
  {
    return c;
  }
  std::streamsize written = mpOStream->Write(&c, 1, 1);
  return (written == 1) ? c : traits_type::eof();
}

nglStdOStream::nglStdOStream(nglOStream* stream, bool OwnStream)
: std::ostream(NULL),
  mOwnStream(OwnStream),
  mpOStream(stream),
  mStreamBuffer(stream)
{
  std::ios::init(&mStreamBuffer);
}

nglStdOStream::~nglStdOStream()
{
  if (mOwnStream)
  {
    delete mpOStream;
  }
}

