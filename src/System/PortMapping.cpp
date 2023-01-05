// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2020-2023, The Talleo developers
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

#include <fstream>

#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>

#include "CryptoNoteConfig.h"
#include "Logging/LoggerRef.h"

#include "PortMapping.h"

using namespace Logging;

namespace System {

void addPortMapping(Logging::LoggerRef& logger, uint32_t port, uint32_t externalPort) {
  // Add UPnP port mapping
  logger(INFO) << "Attempting to add IGD port mapping.";
  int result;
#if MINIUPNPC_API_VERSION < 14
  UPNPDev* deviceList = upnpDiscover(1000, NULL, NULL, 0, 0, &result);
#else
  UPNPDev* deviceList = upnpDiscover(1000, NULL, NULL, UPNP_LOCAL_PORT_ANY, 0, 2, &result);
#endif
  UPNPUrls urls;
  IGDdatas igdData;
  char lanAddress[64];
  result = UPNP_GetValidIGD(deviceList, &urls, &igdData, lanAddress, sizeof lanAddress);
  freeUPNPDevlist(deviceList);
  if (result != 0) {
    if (result == 1) {
      std::ostringstream extPortString;
      extPortString << (externalPort ? externalPort : port);
      std::ostringstream portString;
      portString << port;
      if (UPNP_AddPortMapping(urls.controlURL, igdData.first.servicetype, extPortString.str().c_str(),
        portString.str().c_str(), lanAddress, CryptoNote::CRYPTONOTE_NAME, "TCP", 0, "0") != 0) {
        logger(ERROR) << "UPNP_AddPortMapping failed.";
      } else {
        logger(INFO) << "Added IGD port mapping from port " << extPortString.str() << " to " << portString.str() << ".";
      }
    } else if (result == 2) {
      logger(INFO) << "IGD was found but reported as not connected.";
    } else if (result == 3) {
      logger(INFO) << "UPnP device was found but not recognized as IGD.";
    } else {
      logger(ERROR) << "UPNP_GetValidIGD returned an unknown result code.";
    }

    FreeUPNPUrls(&urls);
  } else {
    logger(INFO) << "No IGD was found.";
  }
}

void deletePortMapping(Logging::LoggerRef& logger, uint32_t port, uint32_t externalPort) {
  // Remove UPnP port mapping
  logger(INFO) << "Attempting to remove IGD port mapping.";
  int result;
#if MINIUPNPC_API_VERSION < 14
  UPNPDev* deviceList = upnpDiscover(1000, NULL, NULL, 0, 0, &result);
#else
  UPNPDev* deviceList = upnpDiscover(1000, NULL, NULL, UPNP_LOCAL_PORT_ANY, 0, 2, &result);
#endif
  UPNPUrls urls;
  IGDdatas igdData;
  char lanAddress[64];
  result = UPNP_GetValidIGD(deviceList, &urls, &igdData, lanAddress, sizeof lanAddress);
  freeUPNPDevlist(deviceList);
  if (result != 0) {
    if (result == 1) {
      std::ostringstream extPortString;
      extPortString << (externalPort ? externalPort : port);
      std::ostringstream portString;
      portString << port;
      if (UPNP_DeletePortMapping(urls.controlURL, igdData.first.servicetype, extPortString.str().c_str(),
        "TCP", 0) != 0) {
        logger(ERROR) << "UPNP_RemovePortMapping failed.";
      } else {
        logger(INFO) << "Removed IGD port mapping from port " << extPortString.str() << " to " << portString.str() << ".";
      }
    } else if (result == 2) {
      logger(INFO) << "IGD was found but reported as not connected.";
    } else if (result == 3) {
      logger(INFO) << "UPnP device was found but not recognized as IGD.";
    } else {
      logger(ERROR) << "UPNP_GetValidIGD returned an unknown result code.";
    }

    FreeUPNPUrls(&urls);
  } else {
    logger(INFO) << "No IGD was found.";
  }
}

}
