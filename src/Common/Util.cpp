// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2019-2023, The Talleo developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#include "Util.h"
#include <cstdio>

#include <boost/filesystem.hpp>

#include "CryptoNoteConfig.h"

#ifdef WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shlobj.h>
#include <strsafe.h>
#else
#include <sys/utsname.h>
#endif


namespace Tools
{
#ifdef WIN32
  std::string get_windows_version_display_string()
  {
    typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
    typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
#define BUFSIZE 10000

    char pszOS[BUFSIZE] = {0};
    OSVERSIONINFOEX osvi;
    SYSTEM_INFO si;
    PGNSI pGNSI;
    PGPI pGPI;
    BOOL bOsVersionInfoEx;
    DWORD dwType;

    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);

    if(!bOsVersionInfoEx) return pszOS;

    // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

    pGNSI = (PGNSI) GetProcAddress(
      GetModuleHandle(TEXT("kernel32.dll")),
      "GetNativeSystemInfo");
    if(NULL != pGNSI)
      pGNSI(&si);
    else GetSystemInfo(&si);

    if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId &&
      osvi.dwMajorVersion > 4 )
    {
      StringCchCopy(pszOS, BUFSIZE, TEXT("Microsoft "));

      // Test for the specific product.

      if ( osvi.dwMajorVersion == 10 )
      {
        if ( osvi.dwMinorVersion == 0 )
        {
          if ( osvi.wProductType == VER_NT_WORKSTATION )
          {
            if ( osvi.dwBuildNumber >= 22000)
              StringCchCat(pszOS, BUFSIZE, TEXT("Windows 11 "));
            else
              StringCchCat(pszOS, BUFSIZE, TEXT("Windows 10 "));
          }
          else
          {
            if (osvi.dwBuildNumber >= 19551)
              StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2022 "));
            else if (osvi.dwBuildNumber >= 17623)
              StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2019 "));
            else
              StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2016 "));
          }
        }

        pGPI = (PGPI) GetProcAddress(
          GetModuleHandle(TEXT("kernel32.dll")),
          "GetProductInfo");

        pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

        switch ( dwType )
        {
        case PRODUCT_BUSINESS:
          StringCchCat(pszOS, BUFSIZE, TEXT("Business"));
          break;
        case PRODUCT_BUSINESS_N:
          StringCchCat(pszOS, BUFSIZE, TEXT("Business N"));
          break;
        case PRODUCT_CLUSTER_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("HPC Edition"));
          break;
        case PRODUCT_CLUSTER_SERVER_V:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Hyper Core V"));
          break;
        case PRODUCT_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Home"));
          break;
        case PRODUCT_CORE_COUNTRYSPECIFIC:
          StringCchCat(pszOS, BUFSIZE, TEXT("Home China"));
          break;
        case PRODUCT_CORE_N:
          StringCchCat(pszOS, BUFSIZE, TEXT("Home N"));
          break;
        case PRODUCT_CORE_SINGLELANGUAGE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Home Single Language"));
          break;
        case PRODUCT_DATACENTER_EVALUATION_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Datacenter (evaluation installation)"));
          break;
        case PRODUCT_DATACENTER_A_SERVER_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Datacenter, Semi-Annual Channel (core installation)"));
          break;
        case PRODUCT_STANDARD_A_SERVER_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Standard, Semi-Annual Channel (core installation)"));
          break;
        case PRODUCT_DATACENTER_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Datacenter (full installation)"));
          break;
        case PRODUCT_DATACENTER_SERVER_CORE_V:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Datacenter without Hyper-V (core installation)"));
          break;
        case PRODUCT_DATACENTER_SERVER_V:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Datacenter without Hyper-V (full installation)"));
          break;
        case PRODUCT_EDUCATION:
          StringCchCat(pszOS, BUFSIZE, TEXT("Education"));
          break;
        case PRODUCT_EDUCATION_N:
          StringCchCat(pszOS, BUFSIZE, TEXT("Education N"));
          break;
        case PRODUCT_ENTERPRISE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise"));
          break;
        case PRODUCT_ENTERPRISE_E:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise E"));
          break;
        case PRODUCT_ENTERPRISE_EVALUATION:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Evaluation"));
          break;
        case PRODUCT_ENTERPRISE_N:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise N"));
          break;
        case PRODUCT_ENTERPRISE_N_EVALUATION:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise N Evaluation"));
          break;
        case PRODUCT_ENTERPRISE_S:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise 2015 LTSB"));
          break;
        case PRODUCT_ENTERPRISE_S_EVALUATION:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise 2015 LTSB Evaluation"));
          break;
        case PRODUCT_ENTERPRISE_S_N:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise 2015 LTSB N"));
          break;
        case PRODUCT_ENTERPRISE_S_N_EVALUATION:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise 2015 LTSB N Evaluation"));
          break;
        case PRODUCT_ENTERPRISE_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Enterprise (full installation)"));
          break;
        case PRODUCT_ENTERPRISE_SERVER_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Enterprise (core installation)"));
          break;
        case PRODUCT_ENTERPRISE_SERVER_CORE_V:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Enterprise without Hyper-V (core installation)"));
          break;
        case PRODUCT_ENTERPRISE_SERVER_IA64:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Enterprise for Itanium-based Systems"));
          break;
        case PRODUCT_ENTERPRISE_SERVER_V:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Enterprise without Hyper-V (full installation)"));
          break;
        case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:
          StringCchCat(pszOS, BUFSIZE, TEXT("Essential Server Solution Additional"));
          break;
        case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:
          StringCchCat(pszOS, BUFSIZE, TEXT("Essential Server Solution Additional SVC"));
          break;
        case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT:
          StringCchCat(pszOS, BUFSIZE, TEXT("Essential Server Solution Management"));
          break;
        case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:
          StringCchCat(pszOS, BUFSIZE, TEXT("Essential Server Solution Management SVC"));
          break;
        case PRODUCT_HOME_BASIC:
          StringCchCat(pszOS, BUFSIZE, TEXT("Home Basic"));
          break;
        case PRODUCT_HOME_BASIC_N:
          StringCchCat(pszOS, BUFSIZE, TEXT("Home Basic N"));
          break;
        case PRODUCT_HOME_PREMIUM:
          StringCchCat(pszOS, BUFSIZE, TEXT("Home Premium"));
          break;
        case PRODUCT_HOME_PREMIUM_N:
          StringCchCat(pszOS, BUFSIZE, TEXT("Home Premium N"));
          break;
        case PRODUCT_HYPERV:
          StringCchCat(pszOS, BUFSIZE, TEXT("Hyper-V Server"));
          break;
        case PRODUCT_IOTUAP:
          StringCchCat(pszOS, BUFSIZE, TEXT("IoT Core"));
          break;
#ifdef PRODUCT_IOTUAPCOMMERCIAL
        case PRODUCT_IOTUAPCOMMERCIAL:
          StringCchCat(pszOS, BUFSIZE, TEXT("IoT Core Commercial"));
          break;
#endif
        case PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT:
          StringCchCat(pszOS, BUFSIZE, TEXT("Essential Business Server Management Server"));
          break;
        case PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING:
          StringCchCat(pszOS, BUFSIZE, TEXT("Essential Business Server Messaging Server"));
          break;
        case PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY:
          StringCchCat(pszOS, BUFSIZE, TEXT("Essential Business Server Security Server"));
          break;
#ifdef PRODUCT_MOBILE_CORE
        case PRODUCT_MOBILE_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Mobile"));
          break;
#endif
#ifdef PRODUCT_MOBILE_ENTERPRISE
        case PRODUCT_MOBILE_ENTERPRISE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Mobile Enterprise"));
          break;
#endif
        case PRODUCT_MULTIPOINT_PREMIUM_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("MultiPoint Server Premium (full installation)"));
          break;
        case PRODUCT_MULTIPOINT_STANDARD_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("MultiPoint Server Standard (full installation)"));
          break;
        case PRODUCT_PRO_WORKSTATION:
          StringCchCat(pszOS, BUFSIZE, TEXT("Pro for Workstations"));
          break;
        case PRODUCT_PRO_WORKSTATION_N:
          StringCchCat(pszOS, BUFSIZE, TEXT("Pro for Workstations N"));
          break;
        case PRODUCT_PROFESSIONAL:
          StringCchCat(pszOS, BUFSIZE, TEXT("Pro"));
          break;
        case PRODUCT_PROFESSIONAL_N:
          StringCchCat(pszOS, BUFSIZE, TEXT("Pro N"));
          break;
        case PRODUCT_PROFESSIONAL_WMC:
          StringCchCat(pszOS, BUFSIZE, TEXT("Professional with Media Center"));
          break;
        case PRODUCT_SB_SOLUTION_SERVER_EM:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server For SB Solutions EM"));
          break;
        case PRODUCT_SERVER_FOR_SB_SOLUTIONS:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server For SB Solutions"));
          break;
        case PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server For SB Solutions EM"));
          break;
        case PRODUCT_SERVER_FOUNDATION:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Foundation"));
          break;
        case PRODUCT_SMALLBUSINESS_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server"));
          break;
        case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
          StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server Premium"));
          break;
        case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server Premium (core installation)"));
          break;
        case PRODUCT_SOLUTION_EMBEDDEDSERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("MultiPoint Server"));
          break;
        case PRODUCT_STANDARD_EVALUATION_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Standard (evaluation installation)"));
          break;
        case PRODUCT_STANDARD_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Standard (full installation)"));
          break;
        case PRODUCT_STANDARD_SERVER_CORE_V:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Standard without Hyper-V (core installation)"));
          break;
        case PRODUCT_STANDARD_SERVER_V:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Standard without Hyper-V"));
          break;
        case PRODUCT_STANDARD_SERVER_SOLUTIONS:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Solutions Premium"));
          break;
        case PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Server Solutions Premium (core installation)"));
          break;
        case PRODUCT_STARTER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Starter"));
          break;
        case PRODUCT_STARTER_N:
          StringCchCat(pszOS, BUFSIZE, TEXT("Starter N"));
          break;
        case PRODUCT_STORAGE_ENTERPRISE_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Storage Server Enterprise"));
          break;
        case PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Storage Server Enterprise (core installation)"));
          break;
        case PRODUCT_STORAGE_EXPRESS_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Storage Server Express"));
          break;
        case PRODUCT_STORAGE_EXPRESS_SERVER_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Storage Server Express (core installation)"));
          break;
        case PRODUCT_STORAGE_STANDARD_EVALUATION_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Storage Server Standard (evaluation installation)"));
          break;
        case PRODUCT_STORAGE_STANDARD_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Storage Server Standard"));
          break;
        case PRODUCT_STORAGE_STANDARD_SERVER_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Storage Server Standard (core installation)"));
          break;
        case PRODUCT_STORAGE_WORKGROUP_EVALUATION_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Storage Server Workgroup (evaluation installation)"));
          break;
        case PRODUCT_STORAGE_WORKGROUP_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Storage Server Workgroup"));
          break;
        case PRODUCT_STORAGE_WORKGROUP_SERVER_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Storage Server Workgroup (core installation)"));
          break;
        case PRODUCT_ULTIMATE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Ultimate"));
          break;
        case PRODUCT_ULTIMATE_N:
          StringCchCat(pszOS, BUFSIZE, TEXT("Ultimate N"));
          break;
        case PRODUCT_WEB_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Web Server (full installation)"));
          break;
        case PRODUCT_WEB_SERVER_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Web Server (core installation)"));
          break;
        }
      }

      if ( osvi.dwMajorVersion == 6 )
      {
        if( osvi.dwMinorVersion == 0 )
        {
          if( osvi.wProductType == VER_NT_WORKSTATION )
            StringCchCat(pszOS, BUFSIZE, TEXT("Windows Vista "));
          else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2008 " ));
        }

        if ( osvi.dwMinorVersion == 1 )
        {
          if( osvi.wProductType == VER_NT_WORKSTATION )
            StringCchCat(pszOS, BUFSIZE, TEXT("Windows 7 "));
          else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2008 R2 " ));
        }

        if ( osvi.dwMinorVersion == 2 )
        {
          if( osvi.wProductType == VER_NT_WORKSTATION )
            StringCchCat(pszOS, BUFSIZE, TEXT("Windows 8 "));
          else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2012 " ));
        }

        if ( osvi.dwMinorVersion == 3 )
        {
          if( osvi.wProductType == VER_NT_WORKSTATION )
            StringCchCat(pszOS, BUFSIZE, TEXT("Windows 8.1 "));
          else
            StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2012 R2 "));
        }

        if ( osvi.dwMinorVersion == 4 ) // Technical preview 1
        {
          if( osvi.wProductType == VER_NT_WORKSTATION )
            StringCchCat(pszOS, BUFSIZE, TEXT("Windows 10 "));
          else
            StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2016 "));
        }

        pGPI = (PGPI) GetProcAddress(
          GetModuleHandle(TEXT("kernel32.dll")),
          "GetProductInfo");

        pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

        switch( dwType )
        {
        case PRODUCT_ULTIMATE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Ultimate Edition" ));
          break;
        case PRODUCT_PROFESSIONAL:
          StringCchCat(pszOS, BUFSIZE, TEXT("Professional" ));
          break;
        case PRODUCT_HOME_PREMIUM:
          StringCchCat(pszOS, BUFSIZE, TEXT("Home Premium Edition" ));
          break;
        case PRODUCT_HOME_BASIC:
          StringCchCat(pszOS, BUFSIZE, TEXT("Home Basic Edition" ));
          break;
        case PRODUCT_ENTERPRISE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition" ));
          break;
        case PRODUCT_BUSINESS:
          StringCchCat(pszOS, BUFSIZE, TEXT("Business Edition" ));
          break;
        case PRODUCT_STARTER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Starter Edition" ));
          break;
        case PRODUCT_CLUSTER_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Cluster Server Edition" ));
          break;
        case PRODUCT_DATACENTER_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition" ));
          break;
        case PRODUCT_DATACENTER_SERVER_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Datacenter Edition (core installation)" ));
          break;
        case PRODUCT_ENTERPRISE_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition" ));
          break;
        case PRODUCT_ENTERPRISE_SERVER_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition (core installation)" ));
          break;
        case PRODUCT_ENTERPRISE_SERVER_IA64:
          StringCchCat(pszOS, BUFSIZE, TEXT("Enterprise Edition for Itanium-based Systems" ));
          break;
        case PRODUCT_SMALLBUSINESS_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server" ));
          break;
        case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
          StringCchCat(pszOS, BUFSIZE, TEXT("Small Business Server Premium Edition" ));
          break;
        case PRODUCT_STANDARD_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition" ));
          break;
        case PRODUCT_STANDARD_SERVER_CORE:
          StringCchCat(pszOS, BUFSIZE, TEXT("Standard Edition (core installation)" ));
          break;
        case PRODUCT_WEB_SERVER:
          StringCchCat(pszOS, BUFSIZE, TEXT("Web Server Edition" ));
          break;
        case PRODUCT_CORE: // This is returned by Windows 10 Home when manifest doesn't have OS version specified
          StringCchCopy(pszOS, BUFSIZE, TEXT("Microsoft Windows 10 Home" ));
        }
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
      {
        if( GetSystemMetrics(SM_SERVERR2) )
          StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Server 2003 R2, "));
        else if ( osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER )
          StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Storage Server 2003"));
        else if ( osvi.wSuiteMask & VER_SUITE_WH_SERVER )
          StringCchCat(pszOS, BUFSIZE, TEXT( "Windows Home Server"));
        else if( osvi.wProductType == VER_NT_WORKSTATION &&
          si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
        {
          StringCchCat(pszOS, BUFSIZE, TEXT( "Windows XP Professional x64 Edition"));
        }
        else StringCchCat(pszOS, BUFSIZE, TEXT("Windows Server 2003, "));

        // Test for the server type.
        if ( osvi.wProductType != VER_NT_WORKSTATION )
        {
          if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
          {
            if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
              StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Edition for Itanium-based Systems" ));
            else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
              StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise Edition for Itanium-based Systems" ));
          }

          else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
          {
            if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
              StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter x64 Edition" ));
            else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
              StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise x64 Edition" ));
            else StringCchCat(pszOS, BUFSIZE, TEXT( "Standard x64 Edition" ));
          }

          else
          {
            if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
              StringCchCat(pszOS, BUFSIZE, TEXT( "Compute Cluster Edition" ));
            else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
              StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Edition" ));
            else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
              StringCchCat(pszOS, BUFSIZE, TEXT( "Enterprise Edition" ));
            else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
              StringCchCat(pszOS, BUFSIZE, TEXT( "Web Edition" ));
            else StringCchCat(pszOS, BUFSIZE, TEXT( "Standard Edition" ));
          }
        }
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
      {
        StringCchCat(pszOS, BUFSIZE, TEXT("Windows XP "));
        if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
          StringCchCat(pszOS, BUFSIZE, TEXT( "Home Edition" ));
        else StringCchCat(pszOS, BUFSIZE, TEXT( "Professional" ));
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
      {
        StringCchCat(pszOS, BUFSIZE, TEXT("Windows 2000 "));

        if ( osvi.wProductType == VER_NT_WORKSTATION )
        {
          StringCchCat(pszOS, BUFSIZE, TEXT( "Professional" ));
        }
        else
        {
          if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
            StringCchCat(pszOS, BUFSIZE, TEXT( "Datacenter Server" ));
          else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
            StringCchCat(pszOS, BUFSIZE, TEXT( "Advanced Server" ));
          else StringCchCat(pszOS, BUFSIZE, TEXT( "Server" ));
        }
      }

      // Include service pack (if any) and build number.

      if( strlen(osvi.szCSDVersion) > 0 )
      {
        StringCchCat(pszOS, BUFSIZE, TEXT(" ") );
        StringCchCat(pszOS, BUFSIZE, osvi.szCSDVersion);
      }

      TCHAR buf[80];

      StringCchPrintf( buf, 80, TEXT(" (build %d)"), osvi.dwBuildNumber);
      StringCchCat(pszOS, BUFSIZE, buf);

      if ( osvi.dwMajorVersion >= 6 )
      {
        if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
          StringCchCat(pszOS, BUFSIZE, TEXT( ", 64-bit" ));
        else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
          StringCchCat(pszOS, BUFSIZE, TEXT(", 32-bit"));
      }

      return pszOS;
    }
    else
    {
      printf( "This sample does not support this version of Windows.\n");
      return pszOS;
    }
  }
#else
std::string get_nix_version_display_string()
{
  utsname un;

  if(uname(&un) < 0)
    return std::string("*nix: failed to get os version");
  return std::string() + un.sysname + " " + un.version + " " + un.release;
}
#endif



  std::string get_os_version_string()
  {
#ifdef WIN32
    return get_windows_version_display_string();
#else
    return get_nix_version_display_string();
#endif
  }



#ifdef WIN32
  std::string get_special_folder_path(int nfolder, bool iscreate)
  {
    namespace fs = boost::filesystem;
    char psz_path[MAX_PATH] = "";

    if(SHGetSpecialFolderPathA(NULL, psz_path, nfolder, iscreate)) {
      return psz_path;
    }

    return "";
  }

  std::wstring get_special_folder_path_w(int nfolder, bool iscreate)
  {
    namespace fs = boost::filesystem;
    wchar_t psz_path[MAX_PATH] = L"";

    if (SHGetSpecialFolderPathW(NULL, psz_path, nfolder, iscreate)) {
      return psz_path;
    }

    return L"";
  }

  std::wstring getDefaultDataDirectoryW()
  {
    std::wstring ws(strlen(CryptoNote::CRYPTONOTE_NAME), L' ');
    ws.resize(std::mbstowcs(&ws[0], CryptoNote::CRYPTONOTE_NAME, strlen(CryptoNote::CRYPTONOTE_NAME)));
    return get_special_folder_path_w(CSIDL_APPDATA, true) + std::wstring(L"/") + ws;
  }
#endif

  std::string getDefaultDataDirectory()
  {
    //namespace fs = boost::filesystem;
    // Windows < Vista: C:\Documents and Settings\Username\Application Data\CRYPTONOTE_NAME
    // Windows >= Vista: C:\Users\Username\AppData\Roaming\CRYPTONOTE_NAME
    // Mac: ~/Library/Application Support/CRYPTONOTE_NAME
    // Unix: ~/.CRYPTONOTE_NAME
    std::string config_folder;
#ifdef WIN32
    // Windows
    config_folder = get_special_folder_path(CSIDL_APPDATA, true) + "/" + CryptoNote::CRYPTONOTE_NAME;
#else
    std::string pathRet;
    char* pszHome = getenv("HOME");
    if (pszHome == NULL || strlen(pszHome) == 0)
      pathRet = "/";
    else
      pathRet = pszHome;
#ifdef MAC_OSX
    // Mac
    pathRet /= "Library/Application Support";
    config_folder =  (pathRet + "/" + CryptoNote::CRYPTONOTE_NAME);
#else
    // Unix
    config_folder = (pathRet + "/." + CryptoNote::CRYPTONOTE_NAME);
#endif
#endif

    return config_folder;
  }

  std::string getDefaultCacheFile(const std::string& dataDir) {
    static const std::string name = "cache_file";

    namespace bf = boost::filesystem;
    bf::path dir = dataDir;

    if (!bf::exists(dir) ) {
      throw std::runtime_error("Directory \"" + dir.string() + "\" doesn't exist");
    }

    if (!bf::exists(dir/name)) {
      throw std::runtime_error("File \"" + boost::filesystem::path(dir/name).string() + "\" doesn't exist");
    }

    return boost::filesystem::path(dir/name).string();
  }

  bool create_directories_if_necessary(const std::string& path)
  {
    namespace fs = boost::filesystem;
    boost::system::error_code ec;
    fs::path fs_path(path);
    if (fs::is_directory(fs_path, ec)) {
      return true;
    }

    return fs::create_directories(fs_path, ec);
  }

  std::error_code replace_file(const std::string& replacement_name, const std::string& replaced_name)
  {
    int code;
#if defined(WIN32)
    // Maximizing chances for success
    DWORD attributes = ::GetFileAttributes(replaced_name.c_str());
    if (INVALID_FILE_ATTRIBUTES != attributes)
    {
      ::SetFileAttributes(replaced_name.c_str(), attributes & (~FILE_ATTRIBUTE_READONLY));
    }

    bool ok = 0 != ::MoveFileEx(replacement_name.c_str(), replaced_name.c_str(), MOVEFILE_REPLACE_EXISTING);
    code = ok ? 0 : static_cast<int>(::GetLastError());
#else
    bool ok = 0 == std::rename(replacement_name.c_str(), replaced_name.c_str());
    code = ok ? 0 : errno;
#endif
    return std::error_code(code, std::system_category());
  }

  bool directoryExists(const std::string& path) {
    boost::system::error_code ec;
    return boost::filesystem::is_directory(path, ec);
  }

}
