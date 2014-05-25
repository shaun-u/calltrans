#ifndef _CALLTRANSDIALOG_H_
#define _CALLTRANSDIALOG_H_

#include "CallTransListeners.h"

#include <string>
#include <memory>

class AmSessionAudioConnector;
class CallTransSession;
class AmRingTone;

class CallTransDialog : private CallTransSessionListener
{
  public:
  CallTransDialog(const std::string& id);

  void addListener(CallTransDialogListener* listener);

  /**
   * LegA is the primary call in the b2b session
   * i.e. the incoming call that starts the session
   *
   * All other legs represent outgoing calls
   * i.e. from the b2b session
   */
  CallTransSession* getLegA();
  void transfer(const std::string& transferrerTag, const std::string& transfereeUri);

  private:
  /** legA: initiator; legB: responder; legC: transfer destination */
  std::auto_ptr<CallTransSession> legA, legB, legC;
  /** convenience to keep track of legA or legB having initiated transfer */
  CallTransSession* transferrer;
  /** dialog identifier : callid of all participants in b2b call */
  std::string did;
  /** audio bridge */
  std::auto_ptr<AmSessionAudioConnector> bridge;
  /** ring tone generator */
  std::auto_ptr<AmRingTone> ringTone;

  /** session listener methods */
  void onConnect(const CallTransSession* leg);
  void onDisconnect(const CallTransSession* leg);

  CallTransDialogListener* listener;
};

#endif //_CALLTRANSDIALOG_H_
