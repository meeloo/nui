//
//  nglStdStream.h
//  nui3
//
//  Created by vince on 1/21/15.
//  Copyright (c) 2015 libNUI. All rights reserved.
//

/*!
 \File  nglStdStream.h
 \brief Wrapping nglStream into std::stream
 */

#pragma once

#include <ios>
#include <istream>
#include <ostream>
#include <streambuf>

#include "nglIStream.h"
#include "nglOStream.h"

class nglStdIStreamBuf : public std::streambuf
{
public:
  nglStdIStreamBuf(nglIStream* stream);
  virtual ~nglStdIStreamBuf() { }
  
protected:
  virtual std::streambuf* setbuf(char *s, std::streamsize n);
  virtual std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way,
                                 std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);
  virtual std::streampos seekpos(std::streampos sp,
                                 std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);
  virtual std::streamsize showmanyc();
  virtual std::streamsize xsgetn(char *s, std::streamsize n);
  virtual int underflow();
  virtual int uflow();
  virtual int pbackfail(int c = EOF);
  
  nglIStream* mpIStream;
};

class nglStdIStream : public std::istream
{
public:
  nglStdIStream(nglIStream* pIStream, bool OwnStream = false);
  virtual ~nglStdIStream();
  
protected:
  nglStdIStreamBuf mStreamBuffer;
  bool mOwnStream = false;
  nglIStream* mpIStream = nullptr;
};



class nglStdOStreamBuf : public std::streambuf
{
public:
  nglStdOStreamBuf(nglOStream* stream);
  virtual ~nglStdOStreamBuf() { }
  
protected:
  virtual std::streambuf *setbuf(char *s, std::streamsize n);
  virtual std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way,
                                 std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);
  virtual std::streampos seekpos(std::streampos sp,
                                 std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);
  virtual std::streamsize xsputn(const char *s, std::streamsize n);
  virtual int overflow(int c);
  
  nglOStream* mpOStream;
};

class nglStdOStream : public std::ostream
{
public:
  nglStdOStream(nglOStream* stream, bool OwnStream = false);
  virtual ~nglStdOStream();
  
protected:
  nglStdOStreamBuf mStreamBuffer;
  bool mOwnStream = false;
  nglOStream* mpOStream = nullptr;
};
