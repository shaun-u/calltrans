#ifndef _CALLTRANSSESSION_H_
#define _CALLTRANSSESSION_H_

#include "AmSession.h"

class CallTransSession;

class CallTransSessionListener
{
  public:
  virtual void onConnect(CallTransSession* leg) = 0;
  virtual void onDisconnect(CallTransSession* leg) = 0;
};

class CallTransSession : public AmSession
{
  public:
  void addListener(CallTransSessionListener* listener);

  void start();
  void terminate();

  private:
  CallTransSessionListener* listener;
};

#endif //_CALLTRANSSESSION_H_
