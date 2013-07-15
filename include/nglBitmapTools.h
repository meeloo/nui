/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

/*!
\file  nglBitmapTools.h
\brief helper for scanline color format conversions
*/

#ifndef __nglBitmapTools_h__
#define __nglBitmapTools_h__

//#include "nui.h"


typedef void (*nglCopyLineFn)(void* pDst, void* pSrc, int32 PixelCount, bool Invert);

nglCopyLineFn nglGetCopyLineFn(int32 DstBPP, int32 SrcBPP); ///< Retreive a pointer to a function that can copy any BPP to any BPP.
void nglCopyImage(void* pDst, int32 dstwidth, int32 dstheight, int32 dstbpp, void* pSrc, int32 srcwidth, int32 srcheight, int32 srcbpp, bool vmirror, bool hmirror);
void nglCopyImage(void* pDst, int32 x, int32 y, int32 dstwidth, int32 dstheight, int32 dstbpp, void* pSrc, int32 srcwidth, int32 srcheight, int32 srcbpp, bool vmirror, bool hmirror);

void nglInvertLineSwap32(char* pDst, char* pSrc, uint32 pixelcount);
void nglInvertLineSwap24(char* pDst, char* pSrc, uint32 pixelcount);
void nglLineSwap24(char* pDst, char* pSrc, uint32 pixelcount);
void nglInvertLine32(uint32* pDst, uint32* pSrc, uint32 pixelcount);
void nglInvertLine24(char* pDst, char* pSrc, uint32 pixelcount);
void nglInvertLine16(short* pDst, short* pSrc, uint32 pixelcount);

//D3D ARGB functions
void nglCopyLine15To32ARGB(void* pDst, void* pSrc, int32 PixelCount, bool Invert);
void nglCopyLine16To32ARGB(void* pDst, void* pSrc, int32 PixelCount, bool Invert);
void nglCopyLine32To32ARGB(void* pDst, void* pSrc, int32 PixelCount, bool Invert);
void nglCopyLine24To32ARGB(void* pDst, void* pSrc, int32 PixelCount, bool Invert);
void nglCopyLineL8To32ARGB(void* pDst, void* pSrc, int32 PixelCount, bool Invert);
void nglCopyLineA8To32ARGB(void* pDst, void* pSrc, int32 PixelCount, bool Invert);

void nglPreMultLine16LumA(void* pDst, void* pSrc, int32 PixelCount);
void nglPreMultLine16ALum(void* pDst, void* pSrc, int32 PixelCount);
void nglPreMultLine32ARGB(void* pDst, void* pSrc, int32 PixelCount);
void nglPreMultLine32RGBA(void* pDst, void* pSrc, int32 PixelCount);

void nglUnPreMultLine16LumA(void* pDst, void* pSrc, int32 PixelCount);
void nglUnPreMultLine16ALum(void* pDst, void* pSrc, int32 PixelCount);
void nglUnPreMultLine32ARGB(void* pDst, void* pSrc, int32 PixelCount);
void nglUnPreMultLine32RGBA(void* pDst, void* pSrc, int32 PixelCount);

#endif // __nglBitmapTools_h__

