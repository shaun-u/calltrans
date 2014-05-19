#include "CallTransDialog.h"
#include "CallTransSession.h"

#include "AmB2ABSession.h"
#include "AmRingTone.h"

CallTransDialog::CallTransDialog(const std::string& id)
  : did(id),legA(new CallTransSession()),bridge(new AmSessionAudioConnector()),
    ringTone(new AmRingTone(0,2000,4000,440,480))
{
  legA->addListener(this);  
}

CallTransSession* CallTransDialog::getLegA()
{
  return legA.get();
}

void CallTransDialog::removeLegA()
{

}

void CallTransDialog::removeLegB()
{

}

void CallTransDialog::removeLegC()
{

}

void CallTransDialog::playRingTone()
{

}

void CallTransDialog::playAnnouncement()
{

}

void CallTransDialog::bridgeAudio()
{

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
    legA->play(NULL);

    legB->setCallgroup(did);
  }
}

void CallTransDialog::onDisconnect(const CallTransSession* leg)
{

}
