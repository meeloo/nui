/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#pragma once

#include "nglLock.h"
#include "nglGuard.h"
#include "nglAtomic.h"



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	LightLock class.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class nglLightLock : public nglLock
{
public:
  
  // Constructor(s) / Destructor
  nglLightLock(bool registerToThreadChecker=true);
  nglLightLock(const char* pName, bool registerToThreadChecker=true);
  virtual ~nglLightLock();

  virtual const nglString& GetLabel() const;


protected:
  
  // virtual from nglLock
  virtual void _Lock(nglThread::ID threadID);
  virtual void _Unlock();
  virtual bool _TryLock(nglThread::ID threadID);
    
private:

  // Data
  nglAtomic mAtomic;
};


// Typedef
typedef nglGuard<nglLightLock> nglLightLockGuard;
typedef nglUnGuard<nglLightLock> nglLightLockUnGuard;




