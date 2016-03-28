/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"


nuiFileList::nuiFileList(const nglPath& rPath)
  : nuiList(nuiVertical),
    mFileListSink(this)
{
  SetObjectClass("nuiFileList");
  nuiLabel* pLabel = new nuiLabel("..");
  pLabel->SetProperty("Path", rPath.GetParent().GetAbsolutePath().GetPathName());

  Populate(rPath);
  mFileListSink.Connect(Activated, &nuiFileList::Selected, this); 
}

nuiFileList::~nuiFileList()
{
}

void nuiFileList::Selected(const nuiEvent& rEvent)
{
  nuiWidgetList Items;
//  NGL_OUT("Selected!\n");
  if (GetSelected(Items))
  {
    nuiWidgetPtr pItem = Items.front();
    if (pItem)
    {
      nglPath path(pItem->GetProperty("Path"));
      if (!path.IsLeaf())
      {
//        NGL_OUT("Exploring %s\n",path.GetAbsolutePath().GetChars());
        Clear();
        nuiLabel* pLabel = new nuiLabel("..");
        pLabel->SetProperty("Path",path.GetParent().GetAbsolutePath().GetPathName());
      
        Populate(path);
      }
//      else
//      {
//        NGL_OUT("'%s' is a file:\nSize: %d\nType: %s\n\n",path.GetAbsolutePath().GetChars(),path.GetSize(),path.GetMimeType().GetChars());
//      }
    }
//    else
//      NGL_OUT("Selected item is NULL!!! (?)\n");
  }
//  else
//    NGL_OUT("?!? No item selected!?!\n");
}


