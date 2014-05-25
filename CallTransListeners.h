#ifndef _CALLTRANSLISTENERS_H_
#define _CALLTRANSLISTENERS_H_

#include <string>

class CallTransSession;

class CallTransSessionListener
{ 
  public:
  virtual void onConnect(const CallTransSession* leg) = 0;
  virtual void onDisconnect(const CallTransSession* leg) = 0;
};

class CallTransDialogListener
{
  public:
  virtual void onDisconnect(const std::string& did) = 0;
};

#endif //_CALLTRANSLISTENERS_H_
