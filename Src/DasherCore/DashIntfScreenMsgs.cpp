#include "DashIntfScreenMsgs.h"
#include "ScreenGameModule.h"

using namespace Dasher;

CDashIntfScreenMsgs::CDashIntfScreenMsgs(CSettingsStore *pSettingsStore)
 : CDashIntfSettings(pSettingsStore) {
}

void CDashIntfScreenMsgs::Message(const string &strText, bool bInterrupt) {
  //Just store the messages for Redraw...
  CDasherScreen::Label *lab = m_DasherScreen->MakeLabel(strText,GetLongParameter(LP_MESSAGE_FONTSIZE));
  if (bInterrupt) {
    m_dqModalMessages.push_back(pair<CDasherScreen::Label*,bool>(lab,false));
    SetBoolParameter(BP_DASHER_PAUSED, true);
  }
  else
    m_dqAsyncMessages.push_back(pair<CDasherScreen::Label*,unsigned long>(lab, 0));
}

bool CDashIntfScreenMsgs::FinishRender(unsigned long ulTime) {
  bool bMsgsChanged=false;
  //Finally any messages - newest that will fit at bottom, proceeding upwards.
  // Firstly clear any non-modal messages that have been onscreen for long enough
  while (!m_dqAsyncMessages.empty() && m_dqAsyncMessages.front().second && ulTime-m_dqAsyncMessages.front().second>GetLongParameter(LP_MESSAGE_TIME)) {
    delete m_dqAsyncMessages.front().first; //the Label
    m_dqAsyncMessages.pop_front(); // => stop displaying it
    bMsgsChanged=true;
  }
  if (!m_dqAsyncMessages.empty() || !m_dqModalMessages.empty()) {
    screenint iY = m_DasherScreen->GetHeight();
    const screenint iMinY((iY*3)/4), iSW(m_DasherScreen->GetWidth());
    //still messages to display...first find out longest-ago N that will fit
    for (deque<pair<CDasherScreen::Label*, unsigned long> >::iterator it = m_dqAsyncMessages.begin(); it!=m_dqAsyncMessages.end() && iY>iMinY; it++) {
      if (it->second==0) {
        //reached a not-yet-displayed asynchronous message
        if (!m_dqModalMessages.empty()) break; //don't start displaying anything while there are modal msgs
        it->second = ulTime; //display message for first time
        bMsgsChanged=true;
      } 
      iY-=m_DasherScreen->TextSize(it->first, GetLongParameter(LP_MESSAGE_FONTSIZE)).second;
    }
    if (!m_dqModalMessages.empty()) {
      bool bDisp(m_dqModalMessages.front().second != 0); //displaying anything atm?
      for (deque<pair<CDasherScreen::Label*,unsigned long> >::iterator it=m_dqModalMessages.begin(); it!=m_dqModalMessages.end() && iY>iMinY; it++) {
        if (bDisp) {
          if (it->second==0) break; //don't start displaying more until previous dismissed
        } else {
          DASHER_ASSERT(it->second==0);
          it->second = ulTime;
          bMsgsChanged = true;
        }
        iY-=m_DasherScreen->TextSize(it->first, GetLongParameter(LP_MESSAGE_FONTSIZE)).second;
      }
    }
    //Now render messages proceeding downwards - non-modal first, then oldest first
    bool bModal(false);
    for (deque<pair<CDasherScreen::Label*, unsigned long> >::iterator it = m_dqAsyncMessages.begin(); ; it++) {
      if (it==m_dqAsyncMessages.end()) {it=m_dqModalMessages.begin(); bModal=true;}
      if (it==m_dqModalMessages.end()) break;
      if (it->second==0) continue;
      pair<screenint,screenint> textDims = m_DasherScreen->TextSize(it->first, GetLongParameter(LP_MESSAGE_FONTSIZE));
      //black (5) rectangle:
      m_DasherScreen->DrawRectangle((iSW - textDims.first)/2, iY, (iSW+textDims.first)/2, iY+textDims.second, 5, -1, -1);
      //white (0) text for non-modal, yellow (111) for modal
      m_DasherScreen->DrawString(it->first, (iSW-textDims.first)/2, iY, GetLongParameter(LP_MESSAGE_FONTSIZE), bModal ? 111 : 0);
      iY+=textDims.second;
    }
  }
  return bMsgsChanged;
}

void CDashIntfScreenMsgs::ChangeScreen(CDasherScreen *pNewScreen) {
  CDasherInterfaceBase::ChangeScreen(pNewScreen);
  for (deque<pair<CDasherScreen::Label*,unsigned long> >::iterator it=m_dqAsyncMessages.begin(); ; it++) {
    if (it==m_dqAsyncMessages.end()) it = m_dqModalMessages.begin();
    if (it==m_dqModalMessages.end()) break;
    const CDasherScreen::Label *pOldLabel(it->first);
    it->first = pNewScreen->MakeLabel(pOldLabel->m_strText, pOldLabel->m_iWrapSize);
    delete pOldLabel;
  }
}

void CDashIntfScreenMsgs::Unpause(unsigned long lTime) {
  if (!GetBoolParameter(BP_DASHER_PAUSED)) return;
  while (!m_dqModalMessages.empty()) {
    if (m_dqModalMessages.front().second) {
      //Message has been displayed; delete it
      delete m_dqModalMessages.front().first; //the label
      m_dqModalMessages.pop_front();
    } else return; //there are more, not-yet displayed, modal messages!
      //These should be after any that were displayed (which have now been erased), so:
      // do not unpause; next frame will render more messages instead.
  }
  CDasherInterfaceBase::Unpause(lTime);
}

CGameModule *CDashIntfScreenMsgs::CreateGameModule(CDasherView *pView, CDasherModel *pModel) {
  return new CScreenGameModule(this, this, pView, pModel);
}