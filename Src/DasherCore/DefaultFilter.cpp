#include "../Common/Common.h"
#include "DefaultFilter.h"
#include "DasherInterfaceBase.h"
#include "Event.h"

#include "CircleStartHandler.h"
#include "TwoBoxStartHandler.h"

#include <iostream>

CDefaultFilter::CDefaultFilter(Dasher::CEventHandler * pEventHandler, CSettingsStore *pSettingsStore, CDasherInterfaceBase *pInterface, CDasherModel *m_pDasherModel, long long int iID)
  : CInputFilter(pEventHandler, pSettingsStore, pInterface, iID, 1) {
  m_pStartHandler = 0;
  m_pAutoSpeedControl = new CAutoSpeedControl(m_pEventHandler, m_pSettingsStore, m_pDasherModel->Framerate());

  CreateStartHandler();
}

CDefaultFilter::~CDefaultFilter() {
  delete m_pAutoSpeedControl;
}

bool CDefaultFilter::DecorateView(CDasherView *pView) {

  bool bDidSomething(false);

  if(GetBoolParameter(BP_DRAW_MOUSE)) {
    DrawMouse(pView);
    bDidSomething = true;
  }

  if(GetBoolParameter(BP_DRAW_MOUSE_LINE)) {
    DrawMouseLine(pView);
    bDidSomething = true;
  }
  
  if(m_pStartHandler)
    bDidSomething = m_pStartHandler->DecorateView(pView) || bDidSomething;

  return bDidSomething;
}

void CDefaultFilter::Timer(int Time, CDasherView *m_pDasherView, CDasherModel *m_pDasherModel) {
  myint iDasherX;
  myint iDasherY;

  m_pDasherView->TapOnDisplay(0, 0, Time, iDasherX, iDasherY);

  int mode;
  
  ApplyTransform(iDasherX, iDasherY);

  m_pDasherModel->Tap_on_display(iDasherX,iDasherY, Time, 0, 0);

  m_pAutoSpeedControl->SpeedControl(iDasherX, iDasherY, m_pDasherModel->Framerate(), m_pDasherView);

  if(m_pStartHandler)
    m_pStartHandler->Timer(Time, m_pDasherView, m_pDasherModel);
}

void CDefaultFilter::KeyDown(int iTime, int iId, CDasherModel *pModel) {

  switch(iId) {
  case 0: // Start on space
    // FIXME - wrap this in a 'start/stop' method (and use for buttons as well as keys)
    if(GetBoolParameter(BP_START_SPACE) && !GetBoolParameter(BP_CLICK_MODE)) {
      if(GetBoolParameter(BP_DASHER_PAUSED))
	m_pInterface->Unpause(iTime);
      else
	m_pInterface->PauseAt(0, 0);
    }
    break; 
  case 100: // Start on mouse
    if(GetBoolParameter(BP_START_MOUSE)) {
      if(GetBoolParameter(BP_DASHER_PAUSED))
	m_pInterface->Unpause(iTime);
      else
	m_pInterface->PauseAt(0, 0);
    }
    break;
  }
}

void CDefaultFilter::HandleEvent(Dasher::CEvent * pEvent) {
  if(pEvent->m_iEventType == 1) {
    Dasher::CParameterNotificationEvent * pEvt(static_cast < Dasher::CParameterNotificationEvent * >(pEvent));
   
    switch (pEvt->m_iParameter) {
    case BP_CIRCLE_START:
    case BP_MOUSEPOS_MODE:
      CreateStartHandler();
      break;
    }
  }
}

void CDefaultFilter::CreateStartHandler() {
  if(m_pStartHandler) {
    delete m_pStartHandler;
    m_pStartHandler = 0;
  }

  if(GetBoolParameter(BP_CIRCLE_START))
    m_pStartHandler = new CCircleStartHandler(m_pEventHandler, m_pSettingsStore, m_pInterface);
  else if(GetBoolParameter(BP_MOUSEPOS_MODE))
    m_pStartHandler = new CTwoBoxStartHandler(m_pEventHandler, m_pSettingsStore, m_pInterface);

}

void CDefaultFilter::DrawMouse(CDasherView *pView) {

  int iCoordinateCount(pView->GetCoordinateCount());

  myint *pCoordinates(new myint[iCoordinateCount]);

  int iType(pView->GetCoordinates(iCoordinateCount, pCoordinates));

  screenint mousex;
  screenint mousey;

  if(iCoordinateCount == 1) {
    mousex = 0;
    mousey = pCoordinates[0];
  }
  else {
    mousex = pCoordinates[0];
    mousey = pCoordinates[1];
  }

  delete[]pCoordinates;
  myint iDasherX;
  myint iDasherY;

  int mode;
  
  if(GetBoolParameter(BP_NUMBER_DIMENSIONS))
    mode = 1;
  else if(GetBoolParameter(BP_EYETRACKER_MODE))
    mode = 2;
  else
    mode = 0;
  
  pView->Input2Dasher(mousex, mousey, iDasherX, iDasherY, iType, mode);

  ApplyTransform(iDasherX, iDasherY);

  if(GetBoolParameter(BP_COLOUR_MODE) == true) {
    pView->DasherDrawCentredRectangle(iDasherX, iDasherY, 5, 2, Opts::ColorSchemes(Objects), false);
  }
  else {
    pView->DasherDrawCentredRectangle(iDasherX, iDasherY, 5, 1, Opts::ColorSchemes(Objects), false);
  }

}

void CDefaultFilter::DrawMouseLine(CDasherView *pView) {

  int iCoordinateCount(pView->GetCoordinateCount());

  myint *pCoordinates(new myint[iCoordinateCount]);

  int iType(pView->GetCoordinates(iCoordinateCount, pCoordinates));

  screenint mousex;
  screenint mousey;

  if(iCoordinateCount == 1) {
    mousex = 0;
    mousey = pCoordinates[0];
  }
  else {
    mousex = pCoordinates[0];
    mousey = pCoordinates[1];
  }

  delete[] pCoordinates;

  myint x[2];
  myint y[2];

  // Start of line is the crosshair location

  x[0] = (myint)GetLongParameter(LP_OX);
  y[0] = (myint)GetLongParameter(LP_OY);

  // End of line is the mouse cursor location - note that we should
  // probably be using a chached value rather than computing this
  // separately to TapOn

  int mode;
  
  if(GetBoolParameter(BP_NUMBER_DIMENSIONS))
    mode = 1;
  else if(GetBoolParameter(BP_EYETRACKER_MODE))
    mode = 2;
  else
    mode = 0;
 
  pView->Input2Dasher(mousex, mousey, x[1], y[1], iType, mode);

  ApplyTransform(x[1], y[1]);

  // Actually plot the line

  if(GetBoolParameter(BP_COLOUR_MODE)) {
    pView->DasherPolyline(x, y, 2, GetLongParameter(LP_LINE_WIDTH), 1);
  }
  else {
    pView->DasherPolyline(x, y, 2, GetLongParameter(LP_LINE_WIDTH), -1);
  }
}

void CDefaultFilter::ApplyTransform(myint &iDasherX, myint &iDasherY) {
}