#ifndef _CALLTRANSFACTORY_H_
#define _CALLTRANSFACTORY_H_

#include "AmApi.h"

#include "CallTransListeners.h"

#include <map>

class CallTransDialog;

class CallTransFactory : 
  public AmSessionFactory, public AmDynInvokeFactory, public AmDynInvoke,
    public CallTransDialogListener
{
  public:
  CallTransFactory(const std::string& applicationName);
  
  int onLoad();

  /**
   * incoming call entry point
   */
  AmSession* onInvite(const AmSipRequest& req);

  /**
   * Dynamic Invoke Methods
   */
  CallTransFactory* getInstance();
  void invoke(const string& method, const AmArg& args, AmArg& ret);

  void onDisconnect(const std::string& did);

  private:
  std::map<const std::string, CallTransDialog* > dialogs;

  void transfer(const char* callid, const char* tag, const char* uri);
  void list();
};

#endif //_CALLTRANSFACTORY_H_
