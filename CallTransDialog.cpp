#include "CallTransDialog.h"
#include "CallTransSession.h"

#include "AmB2ABSession.h"
#include "AmRingTone.h"

CallTransDialog::CallTransDialog(const std::string& id)
  : did(id),legA(new CallTransSession()), transferrer(NULL),
      bridge(new AmSessionAudioConnector()),
	ringTone(new AmRingTone(0,2000,4000,440,480))
{
  legA->addListener(this);  
}

void CallTransDialog::addListener(CallTransDialogListener* listener)
{
  this->listener = listener;
}

CallTransSession* CallTransDialog::getLegA()
{
  return legA.get();
}

void CallTransDialog::transfer(const std::string& transferrerTag, const std::string& transfereeUri)
{
  std::string from;

  //find the tansferring leg
  if((legB->isOutgoing() ? legB->getRemoteTag():legB->getLocalTag()) == transferrerTag)
  {
    transferrer = legB.get();
    from = legA->isOutgoing() ? legA->dlg.remote_party : legA->dlg.local_party;

    DBG("legB is the transferrer; legA will be used as from: %s", from.c_str());
  }
  else if((legA->isOutgoing() ? legA->getRemoteTag():legA->getLocalTag()) == transferrerTag)
  {
    transferrer = legA.get();
    from = legB->isOutgoing() ? legB->dlg.remote_party : legB->dlg.local_party;

    DBG("legA is the transferrer; legB will be used as from: %s", from.c_str());
  }
  else
  {
    ERROR("neither legA nor legB match transferrerTag=%s", transferrerTag.c_str());
  }

  DBG("unbridging media between legA and legB and playing ringtone");
  legA->bridge(NULL);
  legB->bridge(NULL);
  legA->play(ringTone.get());
  legB->play(ringTone.get());

  DBG("initiating call to tranfer destination");
  legC.reset(new CallTransSession());
  legC->addListener(this);
  legC->call(did,from,transfereeUri);
}

void CallTransDialog::onConnect(const CallTransSession* leg)
{
  if(leg == legA.get())
  {
    DBG("legA has connected; playing ringtone and calling legB");

    legA->setCallgroup(did);
    legA->play(ringTone.get());

    legB.reset(new CallTransSession());
    legB->addListener(this);
    legB->call(did,legA->dlg.local_party,legA->dlg.remote_party);
  }
  else if(leg == legB.get())
  {
    DBG("legB has connected; stopping ringtone to legA and bridging media");
    legB->setCallgroup(did);
    legA->play(NULL);
    legA->bridge(bridge.get());
    legB->bridge(bridge.get());
  }
  else if(leg == legC.get())
  {
    DBG("legC has connected; stopping ringtone on legA and legB");
    legC->setCallgroup(did);
    
    legA->play(NULL);
    legB->play(NULL);

    //move transferrer to legC
    if(legA.get() == transferrer)
    {
      legA.release();
      legA = legC;
    }
    else
    {
      legB.release();
      legB = legC;
    }
    legC.reset(transferrer);
    transferrer = NULL;

    DBG("bridging media between tranferred and transferee; terminating call to transferrer");

    legA->bridge(bridge.get());
    legB->bridge(bridge.get());

    legC->terminate();
  }
}

void CallTransDialog::onDisconnect(const CallTransSession* leg)
{
  if(leg == legA.get() || leg == legB.get())
  {
    DBG("either legA or legB has disconnected; terminating b2b call session");
    
    legA->bridge(NULL);
    legB->bridge(NULL);
    legA->terminate();
    legB->terminate();

    if(listener != NULL)
      listener->onDisconnect(did);
  }
  else if(leg == legC.get())
  {
    DBG("legC has disconnected");

    legC->play(NULL);
    legC->terminate();
  }
}
