#include "CallTransSession.h"

#include "AmMediaProcessor.h"
#include "AmSessionContainer.h"
#include "AmB2ABSession.h"

struct CallTransEvent : public AmEvent
{
  enum
  {
    DoConnected = 100,
    DoDisconnected = 110
  };

  CallTransEvent(int eventID) : AmEvent(eventID){}
};

static CallTransEvent DISCONNECTED_EVENT(CallTransEvent::DoDisconnected);
static CallTransEvent CONNECTED_EVENT(CallTransEvent::DoConnected);

CallTransSession::CallTransSession()
  : listener(NULL)
{
  RTPStream()->setPlayoutType(ADAPTIVE_PLAYOUT);
}

void CallTransSession::addListener(CallTransSessionListener* listener)
{
  this->listener = listener;
}

void CallTransSession::start()
{

}

void CallTransSession::terminate()
{

}

void CallTransSession::play(AmAudio* audio)
{
  if(audio == NULL)
  {
    AmMediaProcessor::instance()->removeSession(this);
  }
  else
  {
    setInOut(NULL,audio);
    AmMediaProcessor::instance()->addSession(this, getCallgroup());
  }
}

void CallTransSession::bridge(AmSessionAudioConnector* connector)
{
  if(connector == NULL)
  {
    AmMediaProcessor::instance()->removeSession(this);
    connector->disconnectSession(this);
  }
  else
  {
    connector->connectSession(this);
    AmMediaProcessor::instance()->addSession(this, getCallgroup());
  }
}

void CallTransSession::call(
  const std::string& callid, const std::string& to, const std::string& from)
{
  dlg.local_tag = AmSession::getNewId();
  dlg.callid = callid;
  dlg.local_party = from;
  dlg.remote_party = to;
  dlg.remote_uri = to;

  std::string sdp_offer;
  sdp.genRequest(advertisedIP(),RTPStream()->getLocalPort(),sdp_offer);

  dlg.sendRequest("INVITE","application/sdp",sdp_offer,"");
  start();

  //to receive sip responses, this must be stored under local tag
  AmSessionContainer::instance()->addSession(dlg.local_tag, this);
}

void CallTransSession::onSessionStart(const AmSipRequest& req)
{
  try
  {
    std::string sdpReply;
    acceptAudio(req.body,req.hdrs,&sdpReply);
    if(dlg.reply(req,200,"OK","application/sdp",sdpReply) != 0)
    {
      throw AmSession::Exception(500,"calltrans server error");
    }
    postEvent(&CONNECTED_EVENT);
  }
  catch(const AmSession::Exception& e)
  {
    ERROR("%i %s\n",e.code,e.reason.c_str());
    setStopped();
    dlg.reply(req,e.code,e.reason);
    postEvent(&DISCONNECTED_EVENT);
  }

  AmSession::onSessionStart(req);
}

void CallTransSession::onSipReply(
  const AmSipReply& reply, int old_dlg_status, const string& trans_method)
{
  if(dlg.getStatus() == AmSipDialog::Connected)
  {
    acceptAudio(reply.body,reply.hdrs);
    postEvent(&CONNECTED_EVENT);
  }
}

void CallTransSession::process(AmEvent* evt)
{
  const CallTransEvent* cevt = dynamic_cast<const CallTransEvent*>(evt);
  if(cevt)
  {
    switch(cevt->event_id)
    {
      case CallTransEvent::DoConnected:
      {
	if(listener)
	  listener->onConnect(this);
	break;
      }
      case CallTransEvent::DoDisconnected:
      {
	if(listener)
	  listener->onDisconnect(this);
	break;
      }
      default:
      {
	ERROR("unexpected CallTransEvent");
      }
    }
  }
}
