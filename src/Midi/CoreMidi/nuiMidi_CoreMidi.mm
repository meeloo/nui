/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nuiMidi_CoreMidi.h"
#ifdef NUI_IOS
#import <CoreMIDI/MIDINetworkSession.h>
#endif

std::map<uint32, nuiMidiInPort_CoreMidi*> nuiMidiInPort_CoreMidi::mPorts;
std::map<uint32, nuiMidiOutPort_CoreMidi*> nuiMidiOutPort_CoreMidi::mPorts;

nuiMidiInPort_CoreMidi* nuiMidiInPort_CoreMidi::GetPort(MIDIClientRef pClient, uint32 id)
{
  nuiMidiInPort_CoreMidi* pPort = NULL;
  std::map<uint32, nuiMidiInPort_CoreMidi*>::iterator it = mPorts.find(id);
  if (it != mPorts.end())
    pPort = it->second;
  else
  {
    pPort = new nuiMidiInPort_CoreMidi(pClient, id);
    mPorts[id] = pPort;
  }
  
  pPort->Acquire();
  return pPort;
}

nuiMidiOutPort_CoreMidi* nuiMidiOutPort_CoreMidi::GetPort(MIDIClientRef pClient, uint32 id)
{
  nuiMidiOutPort_CoreMidi* pPort = NULL;
  std::map<uint32, nuiMidiOutPort_CoreMidi*>::iterator it = mPorts.find(id);
  if (it != mPorts.end())
    pPort = it->second;
  else
  {
    pPort = new nuiMidiOutPort_CoreMidi(pClient, id);
    mPorts[id] = pPort;
  }
  
  pPort->Acquire();
  return pPort;
}


//class nuiMidiInPort_CoreMidi
nuiMidiInPort_CoreMidi::nuiMidiInPort_CoreMidi(MIDIClientRef pClient, int32 id)
{
  mPortID = id;
  mpClient = pClient;
  mpSource = NULL;
  mpPort = NULL;

  CFStringRef pname, pmanuf, pmodel;
  char name[64], manuf[64], model[64];
  
  MIDIEndpointRef ep = MIDIGetSource(id);
  MIDIEntityRef ent;
  MIDIDeviceRef dev;
  MIDIEndpointGetEntity(ep, &ent);
  MIDIEntityGetDevice(ent, &dev);
  MIDIObjectGetStringProperty(ep, kMIDIPropertyName, &pname);
  MIDIObjectGetStringProperty(ep, kMIDIPropertyManufacturer, &pmanuf);
  MIDIObjectGetStringProperty(dev, kMIDIPropertyName, &pmodel);
  
  CFStringGetCString(pname, name, sizeof(name), 0);
  CFStringGetCString(pmanuf, manuf, sizeof(manuf), 0);
  CFStringGetCString(pmodel, model, sizeof(model), 0);
  CFRelease(pname);
  CFRelease(pmanuf);
  CFRelease(pmodel);
  
  mName = name;
  mManufacturer = manuf;
  mDeviceName = model;
  
  mIsPresent = true;
}

nuiMidiInPort_CoreMidi::~nuiMidiInPort_CoreMidi()
{
  Close();
}


void nuiCoreMidiReadProc(const MIDIPacketList *pktlist, void *refCon, void *connRefCon)
{
  nuiMidiInPort_CoreMidi* pPort = (nuiMidiInPort_CoreMidi*)refCon;
  pPort->ReadProc(pktlist);
}

void nuiMidiInPort_CoreMidi::ReadProc(const MIDIPacketList *pktlist)
{
  if (!mpProcessFunction)
    return;
  MIDIPacket *packet = (MIDIPacket *)pktlist->packet;
  unsigned int j;
  int i;
  for (j = 0; j < pktlist->numPackets; j++)
  {
    //midiSendPacket(packet, gOutPort, gDest);
    static const double v = 1.0 / 1000000000.0; // Convert to seconds
    mpProcessFunction(this, packet->data, packet->length, ((double)packet->timeStamp) * v);
    packet = MIDIPacketNext(packet);
  }
}


bool nuiMidiInPort_CoreMidi::Open(nuiMidiProcessFn pProcessFunction)
{
  mpProcessFunction = pProcessFunction;
  
  nglString n("Input port ");
  n.Add(mPortID);
  if (MIDIInputPortCreate(mpClient, n.ToCFString(), nuiCoreMidiReadProc, (void*)this, &mpPort) != noErr || !mpPort)
    return false;
  
  mpSource = MIDIGetSource(mPortID);
  if (!mpSource)
  {
    Close();
    return false;
  }
  
  if (MIDIPortConnectSource(mpPort, mpSource, NULL) != noErr)
  {
    Close();
    return false;
  }
  
  return true;
}

bool nuiMidiInPort_CoreMidi::Close()
{
  if (mpSource)
    MIDIPortDisconnectSource(mpPort, mpSource);
  mpSource = NULL;
  if (mpPort)
    MIDIPortDispose(mpPort);
  mpPort = NULL;

}

//class nuiMidiOutPort_CoreMidi
nuiMidiOutPort_CoreMidi::nuiMidiOutPort_CoreMidi(MIDIClientRef pClient, int32 id)
{
  mPortID = id;
  mpClient = pClient;
  mpDestination = NULL;
  mpPort = NULL;

  CFStringRef pname, pmanuf, pmodel;
  char name[64], manuf[64], model[64];
  
  MIDIEndpointRef ep = MIDIGetDestination(id);
  MIDIEntityRef ent;
  MIDIDeviceRef dev;
  MIDIEndpointGetEntity(ep, &ent);
  MIDIEntityGetDevice(ent, &dev);
  MIDIObjectGetStringProperty(ep, kMIDIPropertyName, &pname);
  MIDIObjectGetStringProperty(ep, kMIDIPropertyManufacturer, &pmanuf);
  MIDIObjectGetStringProperty(dev, kMIDIPropertyName, &pmodel);
  
  CFStringGetCString(pname, name, sizeof(name), 0);
  CFStringGetCString(pmanuf, manuf, sizeof(manuf), 0);
  CFStringGetCString(pmodel, model, sizeof(model), 0);
  CFRelease(pname);
  CFRelease(pmanuf);
  CFRelease(pmodel);
  
  mName = name;
  mManufacturer = manuf;
  mDeviceName = model;
  mpPort = NULL;
  mpDestination = NULL;
  
  mIsPresent = true;
}

nuiMidiOutPort_CoreMidi::~nuiMidiOutPort_CoreMidi()
{
  Close();
}

bool nuiMidiOutPort_CoreMidi::Open()
{
  nglString n("Output port ");
  n.Add(mPortID);
  if (MIDIOutputPortCreate(mpClient, n.ToCFString(), &mpPort) != noErr || !mpPort)
    return false;
  
  mpDestination = MIDIGetDestination(mPortID);
  if (!mpDestination)
  {
    Close();
    return false;
  }
  
  return true;
}

bool nuiMidiOutPort_CoreMidi::Close()
{
  if (mpDestination)
    MIDIPortDisconnectSource(mpPort, mpDestination);
  if (mpPort)
    MIDIPortDispose(mpPort);
  mpPort = NULL;
  
}

bool nuiMidiOutPort_CoreMidi::Send(const uint8* pData, uint32 size)
{
  uint32 s = size;
  const uint8* p = pData;
  
  while (s > 0)
  {
    uint32 l = MIN(256, s);
    struct MIDIPacketList pktlist;
    pktlist.numPackets = 1;
    pktlist.packet[0].timeStamp = 0;
    pktlist.packet[0].length = l;
    memcpy(pktlist.packet[0].data, p, l);

    if (noErr != MIDISend(mpPort, mpDestination, &pktlist))
      return false;

    p += l;
    s -= l;
  }

  return true;
}


/////////////////////

nuiMidiPortAPI_CoreMidi::nuiMidiPortAPI_CoreMidi()
{
  mpMidiClientRef = NULL;
  MIDIClientCreate(CFSTR("nui MIDI Client"), NULL, NULL, &mpMidiClientRef);
#ifdef NUI_IOS
  MIDINetworkSession* session = [MIDINetworkSession defaultSession];
  session.enabled = YES;
  session.connectionPolicy = MIDINetworkConnectionPolicy_Anyone;
#endif
}

nuiMidiPortAPI_CoreMidi::~nuiMidiPortAPI_CoreMidi()
{
  if (mpMidiClientRef)
    MIDIClientDispose(mpMidiClientRef);
  mpMidiClientRef = NULL;
}

uint32 nuiMidiPortAPI_CoreMidi::GetInPortCount() const
{
  return MIDIGetNumberOfSources();
}

uint32 nuiMidiPortAPI_CoreMidi::GetOutPortCount() const
{
  return MIDIGetNumberOfDestinations();
}

nuiMidiInPort* nuiMidiPortAPI_CoreMidi::GetInPort(uint32 index)
{
  return nuiMidiInPort_CoreMidi::GetPort(mpMidiClientRef, index);
}

nuiMidiOutPort* nuiMidiPortAPI_CoreMidi::GetOutPort(uint32 index)
{
  return nuiMidiOutPort_CoreMidi::GetPort(mpMidiClientRef, index);
}

nuiSingletonHolder<nuiMidiPortAPI_CoreMidi> CoreMidiAPI;

