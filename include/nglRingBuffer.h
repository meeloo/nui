/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/


#pragma once

#include "nui.h"
#include "nglAtomic.h"
#include "nuiNonCopyable.h"

#pragma once


#include <vector>
#include <string.h>
#include <stdlib.h>

#define NGL_MAKE_POWER_OF_TWO_RINGBUFFER(X) { int Y = 1; while (X>Y) Y = Y << 1; X = Y; }
// RingBuffer schematics legend:
// [ and ] delimits the complete buffer. (= mSize)
// * is a selected sample.
// - is an unselected sample.
// R is the read index.
// W is the write index.


class  nglRingBuffer
{
public:

  nglRingBuffer(int countOfElement, int sizeOfElement, int nbChannels = 1, int leftOver = 3)
  {
    NGL_MAKE_POWER_OF_TWO_RINGBUFFER(countOfElement);
    mSize = countOfElement * sizeOfElement;
    mSizeOfElement = sizeOfElement;
    mReadIndex = 0;
    mWriteIndex = 0;
    mEffectiveSize = countOfElement;
    mLeftOver = leftOver;

    // we add enough space (leftOver) at the end of the ring buffer for the interpolation
    //
    mpBuffers.resize(nbChannels);
    for (int i = 0; i < nbChannels; i++)
    {
      mpBuffers[i] = (unsigned char*)AlignedMalloc(mEffectiveSize + mLeftOver);
      memset(mpBuffers[i], 0, (mEffectiveSize + mLeftOver) * sizeOfElement);
    }
  }

  /////////////////////////////////////////

  virtual ~nglRingBuffer()
  {
    for (size_t i = 0; i < mpBuffers.size(); i++)
      AlignedFree(mpBuffers[i]);
  }

  inline int GetSizeOfElement() const ///< Return the number of bytes for one element in one channel.
  {
    return mSizeOfElement;
  }

  inline void Reset() ///< Reset the read and write indices and epty the buffer.
  {
    mReadIndex = 0;
    mWriteIndex = 0;
  }

  // Getters and setters:
  inline int GetReadIndex() const ///< Return the current read index in number of elements.
  {
    return mReadIndex;
  }

  inline int GetWriteIndex() const ///< Return the current write index in number of elements.
  {
    return mWriteIndex;
  }

  inline void* GetReadPointer(int Channel) const ///< Return the current read pointer for the given channel.
  {
    return mpBuffers[Channel] + (mReadIndex   * mSizeOfElement);
  }

  inline void* GetWritePointer(int Channel) const ///< Return the current write pointer for the given channel.
  {
    return mpBuffers[Channel] + (mWriteIndex  * mSizeOfElement);
  }

  // Get Available sizes for read and write:
  inline int GetReadable() const ///< Return the total number of elements we can read.
  {
    const int r = mReadIndex;
    const int w = mWriteIndex;

    if (w >= r)
    {
      // [-----R*****W-----]
      return (w - r);
    }
    else
    {
      // [*****W-----R*****]
      return (mEffectiveSize - r + w);
    }
  }

  inline int GetWritable() const ///< Return the total number of elements we can write.
  {
    const int r = mReadIndex;
    const int w = mWriteIndex;

    if ( w > r )
    {
      // [*****R-----W*****]
      return ((mEffectiveSize - w + r) - 1);
    }
    else if ( w < r )
    {
      // [-----W*****R-----]
      return ((r - w) - 1);
    }
    else
    {
      // [-----RW-----]
      return (mEffectiveSize - 1);
    }
  }

  inline int GetWritableToEnd() const ///< Return the number of elements writeable until the end of the buffer
  {
    const int r = mReadIndex;
    const int w = mWriteIndex;

    if ( r > w )
    {
      // [-----W*****R-----]
      return (r - w - 1);
    }

    if ( r != 0 )
    {
      // [-----R-----W*****]
      return (mEffectiveSize - w);
    }

    // [R-----W*****]
    return (mEffectiveSize - w - 1);
  }

  inline int GetReadableToEnd() const ///< Return the number of elements readable until the end of the buffer
  {
    const int r = mReadIndex;
    const int w = mWriteIndex;

    if ( w >= r )
    {
      // [-----R*****W-----]
      return (w - r);
    }

    // [-----W-----R*****]
    return (mEffectiveSize - r);
  }

  int GetSize() const ///< Get the size in number of elements.
  {
    return mEffectiveSize;
  }

  int GetChannelSizeInBytes() const ///< Return the number of bytes per channel.
  {
    return mSize;
  }

  bool AssertReadInRange(unsigned char* pPointer, int Channel) const ///< Return true if the given pointer is correct for the given channel.
  {
    return ( (pPointer < mpBuffers[Channel] + mSize) && (pPointer >= mpBuffers[Channel]) );
  }

  inline void AdvanceReadIndex(int Advance) ///< Advance the read index in number of elements.
  {
    mReadIndex = (mReadIndex + Advance) % mEffectiveSize;
  }

  inline void SetReadIndex(int readIndex) ///< Set the read index in number of elements.
  {
    mReadIndex = readIndex % mEffectiveSize;
  }

  inline void AdvanceWriteIndex(int Advance) ///< Advance the write index in number of elements.
  {
    if (mWriteIndex < mLeftOver)
    {
      // copy 'count' frames from the beginning of the ringbuffer to the end in order to ensure
      // the correct interpolation
      int count = mLeftOver - mWriteIndex;
      for (int i=0; i < (int)mpBuffers.size(); i++)
        memcpy(mpBuffers[i] + mSize, mpBuffers[i], count * mSizeOfElement);
    }
    mWriteIndex = (mWriteIndex + Advance) % mEffectiveSize;
  }

  int GetNbChannels() const
  {
    return (int)mpBuffers.size();
  }

  int ReadBlock(unsigned char* pPointer, int Count, int Channel)
  {
    int r = 0;
    void* pSrc = GetReadPointer(Channel);
    int size = std::min(Count, GetReadableToEnd());
    memcpy(pPointer, pSrc, size * mSizeOfElement);

    r += size;
    if (Count == size)
      return r;

    int remain = GetReadable() - GetReadableToEnd();
    pPointer += size * mSizeOfElement;
    size = std::min(remain, Count - size);

    memcpy(pPointer, &mpBuffers[Channel][0], size * mSizeOfElement);
    r += size;

    return r;
  }

  int ReadBlock(unsigned char* pPointer, int Count, int Channel, int SrcOffset)
  {
    int r = 0;
    int toskip = SrcOffset;
    int readable = std::max(0, GetReadable() - toskip);
    int readableToEnd = std::max(0, GetReadableToEnd() - toskip);

    void* pSrc = (unsigned char*)GetReadPointer(Channel) + SrcOffset * mSizeOfElement;
    int size = std::min(Count, readableToEnd);
    memcpy(pPointer, pSrc, size * mSizeOfElement);

    r += size;
    if (Count == size)
      return r;

    int remain = readable - readableToEnd;
    pPointer += size * mSizeOfElement;
    size = std::min(remain, Count - size);

    toskip -= std::min(toskip, GetReadableToEnd());
    pSrc = &mpBuffers[Channel][0] + toskip * mSizeOfElement;

    memcpy(pPointer, pSrc, size * mSizeOfElement);
    r += size;

    return r;
  }

  int WriteBlock(const unsigned char* pPointer, int Count, int Channel)
  {
    int w = 0;
    void* pDst = GetWritePointer(Channel);
    int size = std::min(Count, GetWritableToEnd());
    memcpy(pDst, pPointer, size * mSizeOfElement);

    w += size;
    if (Count == size)
      return w;

    int remain = GetWritable() - GetWritableToEnd();
    pPointer += size * mSizeOfElement;
    size = std::min(remain, Count - size);

    memcpy(&mpBuffers[Channel][0], pPointer, size * mSizeOfElement);
    w += size;

    return w;
  }

  template<typename T>
  int OverlapAddBlock(
                      const T* pPointer,
                      int Count,
                      int Tail,
                      int Channel)
  {
    int w = 0;
    T* pBase = reinterpret_cast<T*>(&mpBuffers[Channel][0]);
    T* pDst = pBase + GetWriteIndex();
    int size = std::min(Count, GetWritableToEnd());
    int head = Count - Tail;
    for (int i = 0; i < size; ++i)
    {
      pDst[i] = (w < head ? pDst[i] : 0) + *pPointer++;
      ++w;
    }

    if (Count == size)
      return w;

    int remain = GetWritable() - GetWritableToEnd();
    size = std::min(remain, Count - size);

    for (int i = 0; i < size; ++i)
    {
      pBase[i] = (w < head ? pBase[i] : 0) + *pPointer++;
      ++w;
    }
    return w;
  }

private:
  ////////////////////////////////////////////
  //

  inline  unsigned char* AlignedMalloc (int size)
  {
    unsigned char* newptr;

    newptr = (unsigned char*) malloc (size * mSizeOfElement);

    return newptr;
  }

  inline void AlignedFree (unsigned char* ptr)
  {
    if (!ptr)
      return;
    free (ptr);
  }

  std::vector<unsigned char*> mpBuffers;
  int mSize; // Size if bytes
  int mEffectiveSize; // Size if number of elements
  int mSizeOfElement; // Size in byte of ONE element

  int mReadIndex; // Read index in number of elements
  int mWriteIndex; // Write index in number of elements

  int mLeftOver; // Size added at the end of the ring buffer for interpolation
};
