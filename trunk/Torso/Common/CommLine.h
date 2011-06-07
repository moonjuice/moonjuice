#ifndef COMMLINE_H
#define COMMLINE_H

// Get needed include file
//#include "StdInc.h"


class CSrvCommandLineInfo : public CCommandLineInfo
{
public:

   // Ctor
   CSrvCommandLineInfo();

   // Overriden public methods
   virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

   // Public variables (YECH!!)
   BOOL m_bRegister;
   BOOL m_bUnregister;
};


#endif