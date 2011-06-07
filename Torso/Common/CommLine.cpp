// Get needed include files
#include "stdafx.h"
#include "CommLine.h"

// Constructor
CSrvCommandLineInfo::CSrvCommandLineInfo()
    : m_bRegister(FALSE),
      m_bUnregister(FALSE)
{ }


// Overriden public methods
void CSrvCommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
   CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);

   // Is this a flag?
   if (!bFlag)
      return;

   // Is this a register or unregister request?
   CString strFlag(lpszParam);
   strFlag.MakeUpper();

   if (strFlag == "REGSERVER")
      m_bRegister = TRUE;
   if (strFlag == "UNREGSERVER")
      m_bUnregister = TRUE;
}
