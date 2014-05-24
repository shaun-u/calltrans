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

CallTransSession* CallTransDialog::getLegA()
{
  return legA.get();
}

void CallTransDialog::transfer(const std::string& transfererTag, const std::string& transfereeUri)
{
  std::string from;

  //find the tansferring leg
  if((legB->isOutgoing() ? legB->getRemoteTag():legB->getLocalTag()) == transfererTag)
  {
    transferrer = legB.get();
    from = legA->isOutgoing() ? legA->dlg.remote_party : legA->dlg.local_party;
  }
  else if((legA->isOutgoing() ? legA->getRemoteTag():legA->getLocalTag()) == transfererTag)
  {
    transferrer = legA.get();
    from = legB->isOutgoing() ? legB->dlg.remote_party : legB->dlg.local_party;
  }

  //play music
  legA->bridge(NULL);
  legB->bridge(NULL);
  legA->play(ringTone.get());
  legB->play(ringTone.get());

  //start call to destination
  legC.reset(new CallTransSession());
  legC->addListener(this);
  legC->call(did,from,transfereeUri);
}

void CallTransDialog::onConnect(const CallTransSession* leg)
{
  if(leg == legA.get())
  {
    legA->setCallgroup(did);
    legA->play(ringTone.get());

    legB.reset(new CallTransSession());
    legB->addListener(this);
    legB->call(did,legA->dlg.local_party,legA->dlg.remote_party);
  }
  else if(leg == legB.get())
  {
    legB->setCallgroup(did);
    legA->play(NULL);
    legA->bridge(bridge.get());
    legB->bridge(bridge.get());
  }
  else if(leg == legC.get())
  {
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

    legA->bridge(bridge.get());
    legB->bridge(bridge.get());

    legC->terminate();
  }
}

void CallTransDialog::onDisconnect(const CallTransSession* leg)
{
  if(leg == legA.get() || leg == legB.get())
  {
    legA->bridge(NULL);
    legB->bridge(NULL);
    legA->terminate();
    legB->terminate();
  }
  else if(leg == legC.get())
  {
    legC->play(NULL);
    legC->terminate();
  }
}
