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

//! nglStdIStreamBuf class
class nglStdIStreamBuf : public std::streambuf
{
public:
  nglStdIStreamBuf(nglIStream* stream);
  virtual ~nglStdIStreamBuf() { }
  
  nglStdIStreamBuf(const nglStdIStreamBuf& rOther) = delete;
  nglStdIStreamBuf& operator=(const nglStdIStreamBuf& rOther) = delete;
  
protected:
  virtual std::streambuf* setbuf(char *s, std::streamsize n) override;
  virtual std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way,
                                 std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;
  virtual std::streampos seekpos(std::streampos sp, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;
  virtual std::streamsize showmanyc() override;
  virtual std::streamsize xsgetn(char *s, std::streamsize n) override;
  virtual int underflow() override;
  virtual int uflow() override;
  virtual int pbackfail(int c = traits_type::eof()) override;
  virtual int sync() override;
  
  nglIStream* mpIStream { nullptr };
};

//! nglStdIStream class
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

//! nglStdOStreamBuf class
class nglStdOStreamBuf : public std::streambuf
{
public:
  nglStdOStreamBuf(nglOStream* stream);
  virtual ~nglStdOStreamBuf() { }
  
protected:
  virtual std::streambuf* setbuf(char *s, std::streamsize n) override;
  virtual std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way,
                                 std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;
  virtual std::streampos seekpos(std::streampos sp,
                                 std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;
  virtual std::streamsize xsputn(const char *s, std::streamsize n) override;
  virtual int overflow(int c) override;
  
  nglOStream* mpOStream { nullptr };
};

//! nglStdOStream class
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
