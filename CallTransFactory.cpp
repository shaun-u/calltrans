#include "CallTransFactory.h"
#include "CallTransDialog.h"
#include "CallTransSession.h"

#include "log.h"

#include <memory>
#include <string>
#include <sstream>

#define MOD_NAME "transcall"

EXPORT_SESSION_FACTORY(CallTransFactory,MOD_NAME);
EXPORT_PLUGIN_CLASS_FACTORY(CallTransFactory,MOD_NAME);

CallTransFactory::CallTransFactory(const std::string& applicationName)
  : AmSessionFactory(applicationName), AmDynInvokeFactory(applicationName)
{

}
  
int CallTransFactory::onLoad()
{
  DBG("CallTransFactory singleton loading");
  return 0;
}

AmSession* CallTransFactory::onInvite(const AmSipRequest& req)
{
  INFO("new dialog: %s",req.callid.c_str());
  DBG("from:%s; to:%s",req.from.c_str(),req.to.c_str());
  dialogs[req.callid] = new CallTransDialog(req.callid);
  dialogs[req.callid]->addListener(this);
  return dialogs[req.callid]->getLegA();
}

CallTransFactory* CallTransFactory::getInstance()
{
  return this;
}

void CallTransFactory::invoke(const string& method, const AmArg& args, AmArg& ret)
{
  DBG("method=%s",method.c_str());

  if(std::string("list").compare(method) == 0)
  {
    std::ostringstream os;
    std::map< std::string, CallTransDialog* >::iterator pos;
    for(pos = dialogs.begin();pos != dialogs.end(); ++pos)
    {
      os << "dialog: " << pos->first << std::endl;
    }
    ret.push(os.str().c_str());
  }
  else if(std::string("transfer").compare(method) == 0)
  {
    ret.push("transfer in progress");
    transfer(args.get(0).asCStr(),args.get(1).asCStr(),args.get(2).asCStr());
  }
  else
  {
    ret.push("methods: list; transfer");
  }
}

void CallTransFactory::onDisconnect(const std::string& did)
{
  DBG("removing disconnected dialog: %s", did.c_str());

  std::map< std::string, CallTransDialog* >::iterator pos = dialogs.find(did);
  if(pos != dialogs.end())
  {
    dialogs.erase(pos);
  }
  else
  {
    DBG("did not find dialog to remove");
  }
}

void CallTransFactory::transfer(
  const char* callid, const char* tag, const char* uri)
{
  DBG("callid=%s; tag=%s; uri=%s", callid, tag, uri);

  if(dialogs.find(callid) != dialogs.end())
  {
    DBG("found dialog to transfer");
    dialogs[callid]->transfer(tag,uri);
  }
  else
  {
    DBG("did not find dialog to transfer");
  }
}
