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
      NGL_ASSERT(!"WTF");
    }
  }
  return eStreamForward;
}


#pragma mark nglIStream wrapper

nglStdIStreamBuf::nglStdIStreamBuf(nglIStream* stream)
: mpIStream(stream)
{
}

auto nglStdIStreamBuf::setbuf(char* pNotImplemented, std::streamsize notImplemented) -> std::streambuf*
{
  NGL_ASSERT(!"WTF");
  return nullptr;
}

auto nglStdIStreamBuf::seekoff(std::streamoff off,
                                         std::ios_base::seekdir way,
                                         std::ios_base::openmode which) -> std::streampos
{
  nglStreamWhence whence = seekDirToStreamWhence(way);
  off_t newPos = mpIStream->SetPos((off_t)off, whence);

  if (newPos >= 0)
  {
    return (std::streampos) newPos;
  }
  NGL_ASSERT(!"POS INVALID");
  return -1;
}

auto nglStdIStreamBuf::seekpos(std::streampos sp, std::ios_base::openmode which) -> std::streampos
{
  if (!(which & std::ios_base::in))
  {
    NGL_ASSERT(!"NOT A INPUT FILE");
    return -1;
  }

  off_t newPos = mpIStream->SetPos((off_t) sp);

  if (newPos >= 0)
  {
    return (std::streampos) newPos;
  }
  NGL_ASSERT(!"NEW POS < 0");
  return -1;
}

auto nglStdIStreamBuf::showmanyc() -> std::streamsize
{
  return mpIStream->Available();
}

auto nglStdIStreamBuf::sync() -> int
{
  NGL_ASSERT(!"sync");
}

auto nglStdIStreamBuf::xsgetn(char *s, std::streamsize n) -> std::streamsize
{
  std::streamsize read = mpIStream->Read((void *) s, (size_t)n, 1);
  return read;
}

auto nglStdIStreamBuf::underflow() -> int
{
  char ch;
  std::streamsize read = mpIStream->Peek(&ch, 1, 1);

  if (read == 1)
  {
    return traits_type::to_int_type(ch);
  }
  return traits_type::eof();
}

auto nglStdIStreamBuf::uflow() -> int
{
  char ch;
  std::streamsize read = mpIStream->Read(&ch, 1, 1);
  
  if (read == 1)
  {
    return traits_type::to_int_type(ch);
  }
  
  return traits_type::eof();
}

auto nglStdIStreamBuf::pbackfail(int c) -> int
{
  NGL_ASSERT(!"Not Implemented");
//  if (c != EOF)
//  {
//  }
  return std::char_traits<char>::eof();
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
  NGL_ASSERT(!"not implemented");
  return NULL;
}

std::streampos nglStdOStreamBuf::seekoff(std::streamoff off,
                                         std::ios_base::seekdir way,
                                         std::ios_base::openmode which)
{
  nglStreamWhence whence = seekDirToStreamWhence(way);

  if (!(which & std::ios_base::out))
  {
    NGL_ASSERT(!"which & std::ios_base::out");
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
  if (c == EOF)
    return c;
  std::streamsize written = mpOStream->Write(&c, 1, sizeof(char));
  return (written == 1) ? c : EOF;
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
    delete mpOStream;
}

