//
//  ConfigViewController.h
//  nuiDbg
//
//  Created by Sebastien Metrot on 28/02/16.
//
//

#pragma once

class ConfigViewController : public nuiWidget
{
public:
  ConfigViewController();
  virtual ~ConfigViewController();

protected:
  virtual void Built();
  nuiEventSink<ConfigViewController> mEventSink;
  nuiSlotsSink mSlotSink;
  nuiButton* mpConnect;
  nuiEditText* mpServer;
  nuiEditText* mpPort;
  nuiLabel* mpConnectLabel;
};
