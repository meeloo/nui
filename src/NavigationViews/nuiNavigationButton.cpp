/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot & Vincent Caron
 
 licence: see nui3/LICENCE.TXT
 */


#include "nui.h"


nuiNavigationButton::nuiNavigationButton(nuiBarButtonSystemItem systemItem)
: nuiButton()
{
  nglString text;
  SetObjectClass("nuiNavigationButton");
  //LBDEBUG TODO
  
  switch (systemItem)
  {
    case eBarButtonSystemItemDone: text = "Done"; break;
    case eBarButtonSystemItemCancel: text = "Cancel"; break;
    case eBarButtonSystemItemEdit: text = "Edit"; break;
    case eBarButtonSystemItemSave: text = "Save"; break;
    case eBarButtonSystemItemAdd: text = "Add"; break;
    case eBarButtonSystemItemReply: text = "Reply"; break;
    case eBarButtonSystemItemUndo: text = "Undo"; break;
    case eBarButtonSystemItemRedo: text = "Redo"; break;
    default:
      break;
  }
  
  if (!text.IsEmpty())
  {
    nuiLabel* pLabel = new nuiLabel(text);
    AddChild(pLabel);
    return;
  }
  
//  switch (systemItem)
//  {
////    case eBarButtonSystemItemFlexibleSpace,
////    case eBarButtonSystemItemFixedSpace,
////      eBarButtonSystemItemPageCurl
//    case eBarButtonSystemItemCompose : break;
//    case eBarButtonSystemItemAction : break; 
//    case eBarButtonSystemItemOrganize : break;
//    case eBarButtonSystemItemBookmarks : break;
//    case eBarButtonSystemItemSearch : break;
//    case eBarButtonSystemItemRefresh : break;
//    case eBarButtonSystemItemStop : break;
//    case eBarButtonSystemItemCamera : break;
//    case eBarButtonSystemItemTrash : break;
//    case eBarButtonSystemItemPlay : break;
//    case eBarButtonSystemItemPause : break;
//    case eBarButtonSystemItemRewind : break;
//    case eBarButtonSystemItemFastForward : break;
//
//  }
}


nuiNavigationButton::nuiNavigationButton()
: nuiButton()
{
  SetObjectClass("nuiNavigationButton");
}

nuiNavigationButton::nuiNavigationButton(const nglString& rText)
: nuiButton()
{
  SetObjectClass("nuiNavigationButton");
  nuiLabel* pLabel = new nuiLabel(rText);
  AddChild(pLabel);
}


nuiNavigationButton::nuiNavigationButton(nuiTexture* pIconTexture)
: nuiButton()
{
  SetObjectClass("nuiNavigationButton");
  AddChild(new nuiImage(pIconTexture));
}



// virtual 
nuiNavigationButton::~nuiNavigationButton()
{

}


  
