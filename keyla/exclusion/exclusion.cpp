#include "exclusion.h"

#include <iomanip>
using namespace std;

/****************************************************************************
*                             createExclusionName
* Inputs:
*       LPCTSTR GUID: The GUID for the exclusion
*       UINT kind: Kind of exclusion
*               UNIQUE_TO_SYSTEM
*               UNIQUE_TO_DESKTOP
*               UNIQUE_TO_SESSION
*               UNIQUE_TO_TRUSTEE
* Result: CString
*       A name to use for the exclusion mutex
* Effect: 
*       Creates the exclusion mutex name
* Notes:
*       The GUID is created by a declaration such as
*               #define UNIQUE _T("{44E678F7-DA79-11d3-9FE9-006067718D04}")
****************************************************************************/

tstring createExclusionName(const tstring & GUID, UINT kind)
   {
    switch(kind)
       { /* kind */
        case UNIQUE_TO_SYSTEM:
		  return GUID;
        case UNIQUE_TO_DESKTOP:
           { /* desktop */
			tstring s = GUID;
            DWORD len;
            HDESK desktop = GetThreadDesktop(GetCurrentThreadId());
            BOOL result = GetUserObjectInformation(desktop, UOI_NAME, NULL, 0, &len);
            DWORD err = ::GetLastError();
            if(!result && err == ERROR_INSUFFICIENT_BUFFER)
               { /* NT/2000 */
                LPBYTE data = new BYTE[len];
                result = GetUserObjectInformation(desktop, UOI_NAME, data, len, &len);
                s += _T("-");
                s += (LPCTSTR)data;
                delete [ ] data;
               } /* NT/2000 */
            else
               { /* Win9x */
                s += _T("-Win9x");
               } /* Win9x */
            return s;
           } /* desktop */
        case UNIQUE_TO_SESSION:
           { /* session */
            tstring s = GUID;
            HANDLE token;
            DWORD len;
            BOOL result = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token);
            if(result)
               { /* NT */
                GetTokenInformation(token, TokenStatistics, NULL, 0, &len);
                LPBYTE data = new BYTE[len];
                GetTokenInformation(token, TokenStatistics, data, len, &len);
                LUID uid = ((PTOKEN_STATISTICS)data)->AuthenticationId;
                delete [ ] data;

				tstringstream ss;
				ss << "-" << hex << setw(8) << setfill(_T('0')) << uid.HighPart << uid.LowPart;
                return s + ss.str();
               } /* NT */
            else
               { /* 16-bit OS */
                return s;
               } /* 16-bit OS */
           } /* session */
        case UNIQUE_TO_TRUSTEE:
           { /* trustee */
            tstring s = GUID;
#define NAMELENGTH 64
            TCHAR userName[NAMELENGTH];
            DWORD userNameLength = NAMELENGTH;
            TCHAR domainName[NAMELENGTH];
            DWORD domainNameLength = NAMELENGTH;
            
            if(GetUserName(userName, &userNameLength))
               { /* get network name */
                // The NetApi calls are very time consuming
                // This technique gets the domain name via an
                // environment variable
                domainNameLength = ExpandEnvironmentStrings(_T("%USERDOMAIN%"),
					                                        domainName,
															NAMELENGTH);
                s += '-';
				s += domainName;
				s += '-';
				s += userName;
               } /* get network name */
            return s;
           } /* trustee */
        default:
            assert(false);
            break;
       } /* kind */
    return GUID;
   } // createExclusionName
