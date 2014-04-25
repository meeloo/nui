/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"

#include <AvailabilityMacros.h>

#if !defined(__COREAUDIO_USE_FLAT_INCLUDES__)
#include <AudioToolbox/AudioToolbox.h>
#else
#include <AudioToolbox.h>
#endif


class nuiAudioDecoderPrivate
{
public:
  nuiAudioDecoderPrivate(nglIStream& rStream);
  virtual ~nuiAudioDecoderPrivate();

  bool Init();
  void Clear();

  nglIStream& mrStream;

  AudioFileID mAudioFileID;
  ExtAudioFileRef mExtAudioFileRef;
  SInt64 mPos;
	nglFileSize mSize;

  void AdjustPreroll(int channels, int frames)
  {
    mPreroll.resize(channels * frames);
    mPrerollBuffers.clear();
    for (int i = 0; i < channels; i++)
      mPrerollBuffers.push_back(&mPreroll[i * frames]);
  }

  std::vector<float> mPreroll;
  std::vector<void*> mPrerollBuffers;
};


OSStatus MyAudioFile_ReadProc(void* pInClientData, SInt64 inPosition, UInt32 requestCount, void* pBuffer, UInt32* pActualCount)
{
  nuiAudioDecoderPrivate* pAudioFile = (nuiAudioDecoderPrivate*)pInClientData;
	nglIStream* pStream = &(pAudioFile->mrStream);
  if (pAudioFile->mPos != inPosition)
  {
    //printf("current: %lld  (asked %d at pos %lld)\n", pAudioFile->mPos, (uint32)requestCount, inPosition);
    pStream->SetPos(inPosition);
    pAudioFile->mPos = inPosition;
  }
  
	uint8* pOut = (uint8*)pBuffer;
	*pActualCount = (UInt32)pStream->ReadUInt8(pOut, requestCount);
	pAudioFile->mPos += *pActualCount;
  //printf("\t actual read: %d (new pos: %lld)\n", (uint32)*pActualCount, pAudioFile->mPos);
	return noErr;
}

SInt64 MyAudioFile_GetSizeProc(void* pInClientData)
{
  nuiAudioDecoderPrivate* pAudioFile = (nuiAudioDecoderPrivate*)pInClientData;
	return pAudioFile->mSize;
}


//
//
// nuiAudioDecoderPrivate
//
//
nuiAudioDecoderPrivate::nuiAudioDecoderPrivate(nglIStream& rStream)
: mrStream(rStream),
	mAudioFileID(NULL),
	mExtAudioFileRef(NULL),
  mPos(-1)
{
}

nuiAudioDecoderPrivate::~nuiAudioDecoderPrivate()
{
	Clear();
}


bool nuiAudioDecoderPrivate::Init()
{
	AudioFileTypeID typeID = 0;

  // we only want to read (not write) so give NULL for write callbacks (seems to work...)
  mrStream.SetPos(0);
	mSize = mrStream.Available();
  OSStatus err =  AudioFileOpenWithCallbacks(this, &MyAudioFile_ReadProc, NULL, &MyAudioFile_GetSizeProc, NULL, typeID, &mAudioFileID);

	if (err != noErr)
		return false;
	
	err = ExtAudioFileWrapAudioFileID(mAudioFileID, false /*not for writing*/, &mExtAudioFileRef);
	
	return (err == noErr);
}

void nuiAudioDecoderPrivate::Clear()
{
	if (mAudioFileID)
		AudioFileClose(mAudioFileID);
}




//
//
// nuiAudioDecoder
//
//

void nuiAudioDecoder::Clear()
{
  if (mpPrivate)
    delete mpPrivate;
}

bool nuiAudioDecoder::CreateAudioDecoderPrivate()
{
  mpPrivate = new nuiAudioDecoderPrivate(mrStream);
  bool res = mpPrivate->Init();
  
  if (!res)
  {
    delete mpPrivate;
    mpPrivate = NULL;
  }
  return res;
}

bool nuiAudioDecoder::ReadInfo()
{
  if (!mpPrivate)
    return false;
  
  AudioStreamBasicDescription FileDesc = { 0 };
  AudioStreamBasicDescription ClientDesc = { 0 };
  UInt32 PropDataSize;
  OSStatus res;
  
  
  // #FIXME: the value given by this function is sometimes wrong:
  // with some m4a files and with Mac OS 10.5, SampleFrames will be 16x too big
  
  // get length
  SInt64 SampleFrames = 0;  
  PropDataSize = sizeof(SampleFrames);
  res = ExtAudioFileGetProperty(mpPrivate->mExtAudioFileRef, kExtAudioFileProperty_FileLengthFrames, &PropDataSize, &SampleFrames); 
  if (res != noErr)
    return false;
  
  PropDataSize = sizeof(FileDesc);
  res = ExtAudioFileGetProperty(mpPrivate->mExtAudioFileRef, kExtAudioFileProperty_FileDataFormat, &PropDataSize, &FileDesc);
  if (res != noErr)
    return false;
  
  double SampleRate = FileDesc.mSampleRate;
  if (IsSampleRateForced())
    SampleRate = GetForcedSampleRate();
  int32 channels = FileDesc.mChannelsPerFrame;
  int32 BitsPerSample = 32;
  ClientDesc.mSampleRate = SampleRate;
  ClientDesc.mChannelsPerFrame = channels;
  ClientDesc.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked | kAudioFormatFlagIsNonInterleaved;
  ClientDesc.mFormatID = kAudioFormatLinearPCM;
  ClientDesc.mFramesPerPacket = 1;
  ClientDesc.mBitsPerChannel = BitsPerSample;
  ClientDesc.mBytesPerFrame = BitsPerSample / 8;
  ClientDesc.mBytesPerPacket = BitsPerSample / 8;

  PropDataSize = sizeof(ClientDesc);  
  res = ExtAudioFileSetProperty(mpPrivate->mExtAudioFileRef, kExtAudioFileProperty_ClientDataFormat, PropDataSize, &ClientDesc);
  if (res != noErr)
    return false;
  
  
  
  mInfo.SetSampleFrames(SampleFrames);
  mInfo.SetSampleRate(SampleRate);
  mInfo.SetChannels(channels);
  mInfo.SetBitsPerSample(BitsPerSample);
  mInfo.SetFileFormat(eAudioCompressed);
  mInfo.SetStartFrame(0);
  mInfo.SetStopFrame(mInfo.GetSampleFrames());
  
  return true;
}

bool nuiAudioDecoder::Seek(int64 SampleFrame)
{
  OSStatus err = ExtAudioFileSeek(mpPrivate->mExtAudioFileRef, SampleFrame);
  if (err == noErr)
    mCurrentPosition = SampleFrame;
  return (err == noErr);
}

int32 nuiAudioDecoder::ReadDE(std::vector<void*> buffers, int32 sampleframes, nuiSampleBitFormat format)
{
  if (!mInitialized)
    return 0;

  if (mPosition != mCurrentPosition)
  {
    // We need to preroll :-(
    int prerollsize = 2000;
    int offset = mPosition - prerollsize;
    if (offset < 0)
    {
      prerollsize += offset;
      offset = 0;
    }
    mpPrivate->AdjustPreroll(buffers.size(), prerollsize);

    Seek(offset);
    if (InternalReadDE(mpPrivate->mPrerollBuffers, prerollsize, eSampleFloat32) != prerollsize)
      return 0;

    // We should have read enough dummy samples now:
    mCurrentPosition = mPosition;
  }

  return InternalReadDE(buffers, sampleframes, format);
}


int32 nuiAudioDecoder::InternalReadDE(std::vector<void*> buffers, int32 sampleframes, nuiSampleBitFormat format)
{
  int32 length = mInfo.GetSampleFrames();
  if (mPosition >= length)
    return 0;
  sampleframes = MIN(sampleframes, length - mPosition);
  
  int32 channels = mInfo.GetChannels();
  if (buffers.size() != channels)
    return 0;
  
  std::vector<float*> temp(channels);
  for (int32 c = 0; c < channels; c++)
  {
    if (format == eSampleFloat32)
      temp[c] = (float*)(buffers[c]);
    else
      temp[c] = new float[sampleframes];
  }
  
  //
  int64 BytesToRead = SampleFramesToBytes(sampleframes);
  int32 listSize = sizeof(AudioBufferList) + sizeof(AudioBuffer)* (channels-1);
  AudioBufferList* pBufList = reinterpret_cast<AudioBufferList*> (new Byte[listSize]);
  pBufList->mNumberBuffers = channels; // we query non-interleaved samples, so we need as many buffers as channels
  for (int32 c = 0; c < pBufList->mNumberBuffers; c++)
  {
    // each AudioBuffer represents one channel (non-interleaved samples)
    pBufList->mBuffers[c].mNumberChannels   = 1;
    pBufList->mBuffers[c].mDataByteSize     = BytesToRead / channels;
    pBufList->mBuffers[c].mData             = temp[c];
  }
  
  //
  UInt32 frames = sampleframes;
  OSStatus err = ExtAudioFileRead(mpPrivate->mExtAudioFileRef, &frames, pBufList);
  if (err != noErr)
    frames = 0;

  if (format == eSampleInt16)
  {
    for (int32 c = 0; c < channels; c++)
    {
      if (err != noErr)
      {
        nuiAudioConvert_FloatBufferTo16bits(temp[c], (int16*)(buffers[c]), frames);
      }
      delete[] temp[c];
    }
  }
  
  delete[] pBufList;
  
  mPosition += frames;
  mCurrentPosition += frames;
  return frames;
}


int32 nuiAudioDecoder::ReadIN(void* pBuffer, int32 sampleframes, nuiSampleBitFormat format)
{
  //don't increment mPosition: it's already done in ReadDE
  int32 channels = mInfo.GetChannels();
  
  int32 length = mInfo.GetSampleFrames();
  if (mPosition >= length)
    return 0;

  sampleframes = MIN(sampleframes, length - mPosition);
  
  std::vector<float*> temp(channels);
  std::vector<void*> tempVoid(channels);
  for (int32 c= 0; c < channels; c++)
  {
    temp[c] = new float[sampleframes];
    tempVoid[c] = (void*)(temp[c]);
  }
  
  int32 sampleFramesRead = ReadDE(tempVoid, sampleframes, eSampleFloat32);
  if (format == eSampleFloat32)
  {
    float* pFloatBuffer = (float*)pBuffer;
    //just interleave samples
    for (int32 c = 0; c < channels; c++)
    {
      for (int32 s = 0; s < sampleFramesRead; s++)
      {
        pFloatBuffer[s * channels + c] = temp[c][s];
      }
    }
  }
  else
  {
    //16 bits int are required, so interleave samples and convert them into float
    int16* pInt16Buffer = (int16*)pBuffer;
    for (int32 c = 0; c < channels; c++)
    {
      nuiAudioConvert_DEfloatToINint16(temp[c], pInt16Buffer, c, channels, sampleFramesRead);
    }
    
  }
  
  for (int32 c= 0; c < channels; c++)
  {
    delete[] temp[c];
  }
  
  return sampleFramesRead;
}