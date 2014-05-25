#ifndef _CALLTRANSSESSION_H_
#define _CALLTRANSSESSION_H_

#include "AmSession.h"

class CallTransSession;
class AmSessionAudioConnector;
class CallTransSessionListener;

class CallTransSession : public AmSession
{
  public:
  CallTransSession();

  /** communication with dialog */
  void addListener(CallTransSessionListener* listener);

  /** close audio; send bye */
  void terminate();
  /** connect/disconnect audio generator */
  void play(AmAudio* audio);
  /** connect/disconnect audio bridge */
  void bridge(AmSessionAudioConnector* connector);
  /** initiate outgoing call */
  void call(const std::string& callid, const std::string& to, const std::string& from);
  /** determine whether this call was incoming or outgoing */
  bool isOutgoing() const;

  /** AmSession overrides */
  void onSessionStart(const AmSipRequest& req);
  void onSipReply(const AmSipReply& reply, int old_dlg_status, const string& trans_method);
  void process(AmEvent* evt);
  void onBye(const AmSipRequest& req);

  private:
  /** communication with dialog to which this session belongs */
  CallTransSessionListener* listener;
  /** state whether this session was originated by incoming or outgoing call */
  enum{INCOMING,OUTGOING} origination;
};

#endif //_CALLTRANSSESSION_H_
