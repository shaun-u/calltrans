#ifndef _CALLTRANSSESSION_H_
#define _CALLTRANSSESSION_H_

#include "AmSession.h"

class CallTransSession;
class AmSessionAudioConnector;

class CallTransSessionListener
{
  public:
  virtual void onConnect(const CallTransSession* leg) = 0;
  virtual void onDisconnect(const CallTransSession* leg) = 0;
};

class CallTransSession : public AmSession
{
  public:
  CallTransSession();

  void addListener(CallTransSessionListener* listener);

  void start();
  void terminate();
  void play(AmAudio* audio);
  void bridge(AmSessionAudioConnector* connector);
  void call(const std::string& callid, const std::string& to, const std::string& from);
  bool isOutgoing() const;

  void onSessionStart(const AmSipRequest& req);
  void onSipReply(const AmSipReply& reply, int old_dlg_status, const string& trans_method);
  void process(AmEvent* evt);
  void onBye(const AmSipRequest& req);

  private:
  CallTransSessionListener* listener;
  enum{INCOMING,OUTGOING} origination;
};

#endif //_CALLTRANSSESSION_H_
