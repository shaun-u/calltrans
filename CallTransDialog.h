#ifndef _CALLTRANSDIALOG_H_
#define _CALLTRANSDIALOG_H_

#include <string>

class CallTransSession;

class CallTransDialog
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
  CallTransSession *legA, *legB, *legC;
};

#endif //_CALLTRANSDIALOG_H_
