#ifndef _CALLTRANSDIALOG_H_
#define _CALLTRANSDIALOG_H_

#include "CallTransSession.h"

#include <string>
#include <memory>

class AmSessionAudioConnector;
class CallTransSession;
class AmRingTone;

class CallTransDialog : private CallTransSessionListener
{
  public:
  CallTransDialog(const std::string& id);

  void addLegA();
  void addLegB(const std::string& uri);
  void addLegC();

  CallTransSession* getLegA();

  void removeLegA();
  void removeLegB();
  void removeLegC();

  void playRingTone();
  void playAnnouncement();
  void bridgeAudio();
  void breakAudio();

  private:
  std::auto_ptr<CallTransSession> legA, legB, legC;
  std::string did;
  std::auto_ptr<AmSessionAudioConnector> bridge;

  std::auto_ptr<AmRingTone> ringTone;

  void onConnect(const CallTransSession* leg);
  void onDisconnect(const CallTransSession* leg);
};

#endif //_CALLTRANSDIALOG_H_
