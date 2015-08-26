/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#ifndef __nuiWidgetInspector_h__
#define __nuiWidgetInspector_h__

#include "nuiTreeView.h"
#include "nuiWidget.h"

class nuiVBox;
class nuiGrid;
class nuiWidgetProxy;
class nuiFolderPane;
class nuiMetaPainterInspector;

class nuiWidgetInspectorNode : public nuiTreeNode
{
public:
  nuiWidgetInspectorNode(nuiWidget* pTarget);
  virtual ~nuiWidgetInspectorNode();

  virtual void Open(bool Opened);
  virtual bool IsEmpty() const;
  nuiWidget* GetTarget();
private:
  nuiEventSink<nuiWidgetInspectorNode> mInspectorNodeSink;
  void UpdateInfos(const nuiEvent& rEvent);
  nuiWidget* mpTarget;
  nuiWidgetInspectorNode* mpTree;
};


class nuiWidgetInfo : public nuiWidget
{
public:
  nuiWidgetInfo(nuiWidget* pTarget);

  void SetTarget(nuiWidget* pWidget);
  void RebuildInfo(bool Reconstruct);
protected:
  virtual ~nuiWidgetInfo();
  void OnTimerTick(const nuiEvent& rEvent);
  void OnDebugUpdate(const nuiEvent& rEvent);
  void OnTrashed(const nuiEvent& rEvent);
  nuiEventSink<nuiWidgetInfo> mWISink;
  nuiWidget* mpTarget;
  nuiTimer* mpTimer;
  bool mNeedUpdate;
  
  nuiWidget* mpDummy;
  nuiVBox* mpInfos;

  nuiLabel* mpName;
  nuiLabel* mpClass;
  nuiLabel* mpCType;
#ifndef __GNUC__
  nuiLabel* mpCRawType;
#endif
  nuiLabel* mpIdealRect;
  nuiLabel* mpUserRect;
  nuiLabel* mpHasUserSize;
  nuiLabel* mpHasUserPos;
  nuiLabel* mpRect;
  nuiLabel* mpNeedLayout;
  nuiLabel* mpNeedSelfRedraw;
  nuiLabel* mpNeedIdealRectCalc;
  nuiLabel* mpGlobalRect;
  nuiGrid* mpMatrix;
  nuiGrid* mpAttributeGrid;
  nuiWidgetProxy* mpProxy;
  nuiMetaPainterInspector* mpPainterInspector;
  void BuildInfo();
};

class nuiWidgetInspector : public nuiWidget
{
public:
  nuiWidgetInspector(nuiWidget* pTarget);

protected:
  virtual ~nuiWidgetInspector();
  nuiEventSink<nuiWidgetInspector> mInspectorSink;
  nuiWidget* mpTarget;
  nuiTreeNode* mpTree;
  nuiWidgetInfo* mpWidgetInfo;
  void OnSelectionChanged(const nuiEvent& rEvent);
};

#endif //ifndef __nuiWidgetInspector_h__
