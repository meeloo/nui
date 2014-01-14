/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"

nuiAudioDecoder::nuiAudioDecoder(nglIStream& rStream) :
  nuiSampleReader(rStream),
  mpPrivate(NULL),
  mCurrentPosition(0),
  mForcedSampleRate(0)
{
  CreateAudioDecoderPrivate();
}

nuiAudioDecoder::nuiAudioDecoder(const nuiAudioDecoder& rDecoder, nglIStream& rStream) :
  nuiSampleReader(rDecoder, rStream),
  mpPrivate(NULL),
  mForcedSampleRate(0)
{
  CreateAudioDecoderPrivate();
}

nuiAudioDecoder::~nuiAudioDecoder()
{
  Clear();
}

nuiSampleReader* nuiAudioDecoder::Clone(nglIStream& rStream) const
{
  nuiAudioDecoder* pDecoder = new nuiAudioDecoder(*this, rStream);
  pDecoder->Init();
  pDecoder->SetPosition(0);
  return pDecoder;
}


void nuiAudioDecoder::SetForcedSampleRate(double forcedSR)
{
  mForcedSampleRate = forcedSR;
}

double nuiAudioDecoder::GetForcedSampleRate() const
{
  return mForcedSampleRate;
}

bool nuiAudioDecoder::IsSampleRateForced() const
{
  return mForcedSampleRate != 0;
}

