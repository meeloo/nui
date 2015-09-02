/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"

//#define NUI_OBJECT_DEBUG_TOOLTIPS


using namespace std;

int64 nuiObject::ObjectCount = 0;

nuiObject::nuiObject()
: mpTrace(NULL)
{
  nglString name;
  name.CFormat("%p", this);
  Init(name);
  
//  ObjectCount++;
//  printf("[NEW] Object Count [%lld]\n", ObjectCount);

}

nuiObject::nuiObject(const nglString& rObjectName)
: mpTrace(NULL)
{
  Init(rObjectName);
//  ObjectCount++;
//  printf("[NEW] Object Count [%lld]\n", ObjectCount);

}


void nuiObject::Init(const nglString& rObjectName)
{
#ifdef _NUI_DEBUG_OBJECTS_
  {
    nglCriticalSectionGuard g(gObjectTraceCS);
    std::pair<std::map<nuiObject*, Trace>::iterator, bool> p = mObjects.insert(std::pair<nuiObject*, Trace>(this, Trace()));
    NGL_ASSERT(p.first != mObjects.end());
    std::map<nuiObject*, Trace>::iterator it = p.first; //mObjects.find(this);
    mpTrace = &(it->second);
    mpTrace->mAlive = true;

    size_t s = mObjects.size();
    //if (!(s % 500))
    {
      NGL_LOG("nuiObject", NGL_LOG_INFO, "Objects total count %d\n", s);
    }
  }
#endif

  mClassNameIndex = -1;
  if (SetObjectClass("nuiObject"))
  {
    InitAttributes();
  }
  SetObjectName(rObjectName);

  mSerializeMode = eSaveNode;

  mpToken = NULL;
}




void nuiObject::InitAttributes()
{
  nuiAttribute<const nglString&>* AttributeClass = new nuiAttribute<const nglString&>
  (nglString(_T("Class")), nuiUnitName,
   nuiAttribute<const nglString&>::GetterDelegate(this, &nuiObject::GetObjectClass));


  nuiAttribute<const nglString&>* AttributeName = new nuiAttribute<const nglString&>
  (nglString(_T("Name")), nuiUnitName,
   nuiAttribute<const nglString&>::GetterDelegate(this, &nuiObject::GetObjectName),
   nuiAttribute<const nglString&>::SetterDelegate(this, &nuiObject::SetObjectName));

	AddAttribute(_T("Class"), AttributeClass);
	AddAttribute(_T("Name"), AttributeName);
}


void nuiObject::SetSerializeMode (nuiSerializeMode eMode)
{
  CheckValid();
  mSerializeMode = eMode;
  DebugRefreshInfo();
}

nuiSerializeMode nuiObject::GetSerializeMode () const
{
  CheckValid();
  return mSerializeMode;
}

nuiObject::~nuiObject()
{
  CheckValid();
#ifdef _NUI_DEBUG_OBJECTS_
  NGL_LOG("nuiObject", NGL_LOG_DEBUG, "Deleting object '%s' (class='%s')\n", GetObjectName().GetChars(), GetObjectClass().GetChars());
#endif

//  ObjectCount--;
//  printf("[DEL] Object Count [%lld]\n", ObjectCount);

  ClearLinkedObjects();
  
  delete mpToken;

  int32 c = mClassNameIndex;
  while (c >= 0)
  {
    // clean attributes
    std::map<nglString,nuiAttributeBase*>::const_iterator it = mClassAttributes[c].begin();
    std::map<nglString,nuiAttributeBase*>::const_iterator end = mClassAttributes[c].end();

    while (it != end)
    {
      nuiAttributeBase* pAttribute = it->second;
      pAttribute->KillAttributeHolder(this);
      ++it;
    }

    c = mInheritanceMap[c];
  }

  // Kill instance attributes:
  std::map<nglString, nuiAttributeBase*>::iterator it = mInstanceAttributes.begin();
  std::map<nglString, nuiAttributeBase*>::iterator end = mInstanceAttributes.end();

  while (it != end)
  {
    nuiAttributeBase* pAttrib = it->second;
    delete pAttrib;
    ++it;
  }

  mInstanceAttributes.clear();

  if (mpTrace)
  {
    nglCriticalSectionGuard g(gObjectTraceCS);
    mpTrace->mAlive = false;
  }
}


const nglString& nuiObject::GetObjectClass() const
{
  CheckValid();
	return GetClassNameFromIndex(mClassNameIndex);
}



const nglString& nuiObject::GetObjectName() const
{
  CheckValid();
	return mObjectName;
}


void nuiObject::SetObjectName(const nglString& rName)
{
  CheckValid();
  if (mpTrace)
  {
    nglCriticalSectionGuard g(gObjectTraceCS);
    mpTrace->mName = rName;
  }

  mObjectName = rName;

  DebugRefreshInfo();

#ifdef NUI_OBJECT_DEBUG_TOOLTIPS
  {
    // Enable this to debug your tooltips and classes
    nglString tt;
    tt.Add(GetObjectClass()).Add(_T(" - ")).Add(GetObjectName());
    SetProperty(_T("ToolTip"), tt);
  }
#endif

}

std::vector<int32> nuiObject::mInheritanceMap;

bool nuiObject::SetObjectClass(const nglString& rClass)
{
  CheckValid();
#ifdef _DEBUG_
  mpTrace->mClass = rClass;
#endif

  int32 c = GetClassNameIndex(rClass);
  bool first = mInheritanceMap[c] < -1;
  mInheritanceMap[c] = GetObjectClassNameIndex();

//	const nglString propname = _T("Class");
//  mProperties[propname] = rClass;

  mClassNameIndex = c;

  DebugRefreshInfo();

#ifdef NUI_OBJECT_DEBUG_TOOLTIPS
  {
    // Enable this to debug your tooltips and classes
    nglString tt;
    tt.Add(GetObjectClass()).Add(_T(" - ")).Add(GetObjectName());
    SetProperty(_T("ToolTip"), tt);
  }
#endif
  return first;
}

void nuiObject::GetObjectInheritance(std::vector<nglString>& rClasses) const
{
  CheckValid();
  int32 c = mClassNameIndex;

  do
  {
    rClasses.push_back(GetClassNameFromIndex(c));
    c = mInheritanceMap[c];
  }
  while (c >= 0);
}


bool nuiObject::IsOfClass(const nglString& rClass) const
{
  CheckValid();
  int32 c = GetClassNameIndex(rClass);
  return IsOfClass(c);
}

bool nuiObject::IsOfClass(int32 ClassIndex) const
{
  CheckValid();
  NGL_ASSERT(ClassIndex < mInheritanceMap.size());

  int32 c = mClassNameIndex;

  while (c && c > ClassIndex)
  {
    c = mInheritanceMap[c];
  }

  return c == ClassIndex;
}

bool nuiObject::HasProperty(const nglString& rName) const
{
  return GetAttribute(rName).IsValid();
}

bool nuiObject::HasProperty(const char* pName) const
{
  return HasProperty(nglString(pName));
}

void nuiObject::SetProperty (const nglString& rName, const nglString& rValue)
{
  CheckValid();
//  if (rName == _T("ToolTip") && GetObjectClass() == _T("HelpLabel"))
//  {
//    NGL_OUT(_T("nuiObject::SetProperty for 0x%x %s / %s = %s\n"), this, GetObjectClass().GetChars(), GetObjectName().GetChars(), rValue.GetChars());
//  }

  nuiAttribBase attrib(GetAttribute(rName));
  if (attrib.IsValid())
  {
    attrib.FromString(rValue);
  }
  else
  {
    nuiValueAttribute<nglString>* a = new nuiValueAttribute<nglString>(rName, rValue);
    AddAttribute(rName, a);
  }
  DebugRefreshInfo();
}

nglString nuiObject::GetProperty (const nglString& rName) const
{
  CheckValid();
  nuiAttribBase attrib(GetAttribute(rName));
  if (attrib.IsValid())
  {
    nglString val;
    attrib.ToString(val);
    return val;
  }

  return nglString::Null;
}

nglString nuiObject::GetProperty(const char* pName) const
{
  CheckValid();
  nglString tmp(pName);
  return GetProperty(tmp);
}

void nuiObject::SetProperty(const char* pName, const nglString& rValue)
{
  CheckValid();
  nglString tmp(pName);
  SetProperty(tmp, rValue);
}

void nuiObject::SetProperty(const char* pName, const char* pValue)
{
  CheckValid();
  nglString tmp(pValue);
  SetProperty(pName, tmp);
}

void nuiObject::SetToken(nuiTokenBase* pToken)
{
  CheckValid();
  if (mpToken != pToken)
    delete mpToken;
  mpToken = pToken;
}

nuiTokenBase* nuiObject::GetToken() const
{
  CheckValid();
  return mpToken;
}




//** nuiAttributes system *******************************************

uint32 nuiObject::mUniqueAttributeOrder = 0;


void nuiObject::GetAttributes(std::map<nglString, nuiAttribBase>& rAttributeMap) const
{
  CheckValid();
  rAttributeMap.clear();

  // Add instance attributes:
  {
    std::map<nglString,nuiAttributeBase*>::const_iterator it = mInstanceAttributes.begin();
    std::map<nglString,nuiAttributeBase*>::const_iterator end = mInstanceAttributes.end();

    while (it != end)
    {
      rAttributeMap.insert(make_pair(it->first, nuiAttribBase(const_cast<nuiObject*>(this), it->second)));
      ++it;
    }
  }

  // Add classes attributes:
  int32 c = mClassNameIndex;
  while (c >= 0)
  {
    //printf("\t\tattr for class %s\n", GetClassNameFromIndex(c).GetChars());
    std::map<nglString,nuiAttributeBase*>::const_iterator it = mClassAttributes[c].begin();
    std::map<nglString,nuiAttributeBase*>::const_iterator end = mClassAttributes[c].end();

    while (it != end)
    {
      rAttributeMap.insert(make_pair(it->first, nuiAttribBase(const_cast<nuiObject*>(this), it->second)));
      ++it;
    }

    c = mInheritanceMap[c];
  }
}

void nuiObject::GetAttributesOfClass(uint32 ClassIndex, std::map<nglString, nuiAttributeBase*>& rAttributeMap)
{
  rAttributeMap.clear();

  // Add classes attributes:
  int32 c = ClassIndex;
  while (c >= 0)
  {
    //printf("\t\tattr for class %s\n", GetClassNameFromIndex(c).GetChars());
    std::map<nglString,nuiAttributeBase*>::const_iterator it = mClassAttributes[c].begin();
    std::map<nglString,nuiAttributeBase*>::const_iterator end = mClassAttributes[c].end();

    while (it != end)
    {
      rAttributeMap.insert(*it);
      ++it;
    }

    c = mInheritanceMap[c];
  }
}

static bool NUIATTRIBUTES_COMPARE(const nuiAttribBase& rLeft, const nuiAttribBase& rRight)
{
  return (rLeft.GetOrder() < rRight.GetOrder());
}

void nuiObject::GetSortedAttributes(std::list<nuiAttribBase>& rListToFill) const
{
  CheckValid();
  rListToFill.clear();

  // Add classes attributes
  int32 c = mClassNameIndex;
  while (c >= 0)
  {
    std::map<nglString,nuiAttributeBase*>::const_iterator it = mClassAttributes[c].begin();
    std::map<nglString,nuiAttributeBase*>::const_iterator end = mClassAttributes[c].end();
    while (it != end)
    {
      nuiAttributeBase* pBase = it->second;
      rListToFill.push_back(nuiAttribBase(const_cast<nuiObject*>(this), pBase));

      ++it;
    }

    c = mInheritanceMap[c];
  }

  // Add instance attributes
  {
    std::map<nglString,nuiAttributeBase*>::const_iterator it = mInstanceAttributes.begin();
    std::map<nglString,nuiAttributeBase*>::const_iterator end = mInstanceAttributes.end();
    while (it != end)
    {
      nuiAttributeBase* pBase = it->second;
      rListToFill.push_back(nuiAttribBase(const_cast<nuiObject*>(this), pBase));

      ++it;
    }
  }

  rListToFill.sort(NUIATTRIBUTES_COMPARE);
}




nuiAttribBase nuiObject::GetAttribute(const nglString& rName) const
{
  CheckValid();
  // Search Instance Attributes:
  {
    std::map<nglString,nuiAttributeBase*>::const_iterator it = mInstanceAttributes.find(rName);
    std::map<nglString,nuiAttributeBase*>::const_iterator end = mInstanceAttributes.end();

    if (it != end)
      return nuiAttribBase(const_cast<nuiObject*>(this), it->second);
  }


  // Search classes attributes:
  int32 c = mClassNameIndex;
  while (c >= 0)
  {
    std::map<nglString,nuiAttributeBase*>::const_iterator it = mClassAttributes[c].find(rName);
    std::map<nglString,nuiAttributeBase*>::const_iterator end = mClassAttributes[c].end();

    if (it != end)
      return nuiAttribBase(const_cast<nuiObject*>(this), it->second);

    c = mInheritanceMap[c];
  }

  return nuiAttribBase();
}


void nuiObject::AddAttribute(const nglString& rName, nuiAttributeBase* pAttribute)
{
  CheckValid();

  if (pAttribute->IsInstanceAttribute())
  {
    AddInstanceAttribute(rName, pAttribute);
    return;
  }

  mUniqueAttributeOrder++;
  pAttribute->SetOrder(mUniqueAttributeOrder);

  NGL_ASSERT(mClassNameIndex < mClassAttributes.size());
  mClassAttributes[mClassNameIndex][rName] = pAttribute;
}

void nuiObject::AddAttribute(nuiAttributeBase* pAttribute)
{
  CheckValid();

  if (pAttribute->IsInstanceAttribute())
  {
    AddInstanceAttribute(pAttribute);
    return;
  }


  mUniqueAttributeOrder++;
  pAttribute->SetOrder(mUniqueAttributeOrder);

  mClassAttributes[mClassNameIndex][pAttribute->GetName()] = pAttribute;
}

void nuiObject::AddInstanceAttribute(const nglString& rName, nuiAttributeBase* pAttribute)
{
  CheckValid();
  mUniqueAttributeOrder++;
  pAttribute->SetOrder(mUniqueAttributeOrder);
  pAttribute->SetAsInstanceAttribute(true);

  mInstanceAttributes[rName] = pAttribute;
}

void nuiObject::AddInstanceAttribute(nuiAttributeBase* pAttribute)
{
  CheckValid();
  mUniqueAttributeOrder++;
  pAttribute->SetOrder(mUniqueAttributeOrder);
  pAttribute->SetAsInstanceAttribute(true);

  mInstanceAttributes[pAttribute->GetName()] = pAttribute;
}


std::vector<nglString> nuiObject::mObjectClassNames;
std::map<nglString, int32> nuiObject::mObjectClassNamesMap;
std::vector<std::map<nglString,nuiAttributeBase*> > nuiObject::mClassAttributes;

int32 nuiObject::GetObjectClassNameIndex() const
{
  CheckValid();
  return mClassNameIndex;
}

int32 nuiObject::GetClassNameIndex(const nglString& rName)
{
  std::map<nglString, int32>::iterator it = mObjectClassNamesMap.find(rName);
  if (it == mObjectClassNamesMap.end())
  {
    int32 index = mObjectClassNames.size();
    mObjectClassNamesMap[rName] = index;
    mObjectClassNames.push_back(rName);
    mClassAttributes.resize(index + 1);
    mInheritanceMap.push_back(-2); // -1 = not parent, -2 = not initialized
    NGL_DEBUG( NGL_LOG("nuiObject", NGL_LOG_INFO, "New class: %s [%d]\n", rName.GetChars(), index); )

    return index;
  }
  return mObjectClassNamesMap[rName];
}

const nglString& nuiObject::GetClassNameFromIndex(int32 index)
{
  return mObjectClassNames[index];
}

//////////////////////////// Global Properties
nuiPropertyMap nuiObject::mGlobalProperties;


void nuiObject::SetGlobalProperty(const nglString& rName, const nglString& rValue)
{
  mGlobalProperties[rName] = rValue;
}

void nuiObject::SetGlobalProperty(const char* pName, const nglString& rValue)
{
  SetGlobalProperty(nglString(pName), rValue);
}

void nuiObject::SetGlobalProperty(const char* pName, const char* pValue)
{
  SetGlobalProperty(nglString(pName), nglString(pValue));
}

const nglString& nuiObject::GetGlobalProperty(const nglString& rName)
{
  const nuiPropertyMap::const_iterator it = mGlobalProperties.find(rName);
  if (it == mGlobalProperties.end())
    return nglString::Null;

  return it->second;
}

const nglString& nuiObject::GetGlobalProperty(const char* pName)
{
  return GetGlobalProperty(nglString(pName));
}

bool nuiObject::IsGlobalProperty(const nglString& rName)
{
  const nuiPropertyMap::const_iterator it = mGlobalProperties.find(rName);
  return (it != mGlobalProperties.end());
}

bool nuiObject::IsGlobalProperty(const char* pName)
{
  return IsGlobalProperty(nglString(pName));
}

bool nuiObject::ClearGlobalProperties()
{
  mGlobalProperties.clear();
  return true;
}

bool nuiObject::GetGlobalProperties(std::list<nglString>& rPropertyNames)
{
  nuiPropertyMap::const_iterator it = mGlobalProperties.begin();
  nuiPropertyMap::const_iterator end = mGlobalProperties.end();

  while (it != end)
  {
    rPropertyNames.push_back(it->first);
    ++it;
  }

  return true;
}

bool nuiObject::ClearGlobalProperty(const nglString& rName)
{
  nuiPropertyMap::iterator it = mGlobalProperties.find(rName);
  if (it != mGlobalProperties.end())
    return false;
  mGlobalProperties.erase(it);
  return true;
}

bool nuiObject::ClearGlobalProperty(const char* pName)
{
  return ClearGlobalProperty(nglString(pName));
}

std::map<nuiObject*, nuiObject::Trace> nuiObject::mObjects;
nglCriticalSection nuiObject::gObjectTraceCS;

#ifdef _NUI_DEBUG_OBJECTS_
bool nuiObject::IsObject(void* pointer)
{
  std::map<nuiObject*, Trace>::const_iterator it = mObjects.find((nuiObject*)pointer);
  return it != mObjects.end();
}

void nuiObject::DumpObjectInfos(const nuiObject* pointer)
{
  nglString str;
  {
    nglCriticalSectionGuard g(gObjectTraceCS);
    std::map<nuiObject*, Trace>::const_iterator it = mObjects.find(const_cast<nuiObject*>(pointer));
    if (it == mObjects.end())
    {
      str.CFormat("Operating on an invalid Object! 0x%x was never created.\n", pointer);
    }
    else if (!it->second.mAlive)
    {
      str.CFormat("Operating on a deleted Object! 0x%x (%s - %s).\n", pointer, it->second.mClass.GetChars(), it->second.mName.GetChars());
    }
    else
    {
      str.CFormat("Valid Object! 0x%x (%s - %s).\n", pointer, it->second.mClass.GetChars(), it->second.mName.GetChars());
    }
  }

  if (!str.IsEmpty())
  {
    NGL_LOG("nuiObject", 0, "%s", str.GetChars());
  }
}
#endif

void nuiObject::CheckValidInternal() const
{
#ifdef _NUI_DEBUG_OBJECTS_
  nglString str;
  {
    nglCriticalSectionGuard g(gObjectTraceCS);
    std::map<nuiObject*, Trace>::const_iterator it = mObjects.find(const_cast<nuiObject*>(this));
    if (it == mObjects.end())
    {
      str.CFormat("Operating on an invalid Object! 0x%x was never created.\n", this);
    }
    else if (!it->second.mAlive)
    {
      str.CFormat("Operating on an invalid Object! 0x%x (%s - %s).\n", this, it->second.mClass.GetChars(), it->second.mName.GetChars());
    }
    NGL_ASSERT(it != mObjects.end());
    const Trace& rTrace(it->second);
    NGL_ASSERT(rTrace.mAlive);
  }
  if (!str.IsEmpty())
  {
    NGL_LOG("nuiObject", 0, "%s", str.GetChars());
  }
#endif
}

int32 nuiObject::GetClassCount()
{
  return mObjectClassNames.size();
}


void nuiObject::LinkObject(nuiObject* pLinkedObject)
{
  mpLinkedObjects.push_back(pLinkedObject);
}

void nuiObject::UnlinkObject(nuiObject* pLinkedObject)
{
  for (auto it = mpLinkedObjects.begin(); it != mpLinkedObjects.end(); ++it)
  {
    mpLinkedObjects.erase(it++);
  }
}

void nuiObject::ClearLinkedObjects()
{
  mpLinkedObjects.clear();
}

#ifdef _NUI_DEBUG_OBJECTS_
void nuiObject::DumpInfos() const
{
  DumpObjectInfos(this);
}
#endif
