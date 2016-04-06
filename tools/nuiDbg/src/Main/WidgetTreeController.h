//
//  WidgetTreeController.h
//  nuiDbg
//
//  Created by Sebastien Metrot on 06/03/2016.
//
//

#pragma once

class WidgetTreeController : public nuiWidget
{
public:
  WidgetTreeController();
  ~WidgetTreeController();

  void Built();

protected:
  nuiTreeView* mpWindows = nullptr;
  nuiEventSink<WidgetTreeController> mSink;
};
