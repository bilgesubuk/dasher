// PrefsPageBase.h
// 
// A common base class for each page in the Win32 preferences property sheet.
// Seb Wills
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 Iain Murray, Inference Group, Cavendish, Cambridge.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __PrefsPageBase_h__
#define __PrefsPageBase_h__

#include "../resource.h"
#include "../AppSettings.h"

#include "../../DasherCore/Win32/DasherInterface.h"

class CPrefsPageBase:public CWinWrap {
public:
  CPrefsPageBase(HWND Parent, CDasherInterface * DI, CAppSettings *pAppSettings);
protected:
  LRESULT WndProc(HWND Window, UINT message, WPARAM wParam, LPARAM lParam);

  CDasherInterface * m_pDasherInterface;
  CAppSettings *m_pAppSettings;
  
  HWND m_hPropertySheet; // the property sheet of which we are one page

  virtual void PopulateList() =0;
  //virtual void InitCustomBox() =0; //what are these for?
  //virtual bool UpdateInfo() =0;
  virtual bool Apply() =0;
  virtual bool Validate() =0;

};

#endif  /* #ifndef __PrefsPageBase_h__ */