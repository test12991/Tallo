// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2017 XDN - project developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018-2019, The Karbo developers
// Copyright (c) 2019-2022, The Talleo developers
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

#include "PaymentServiceConfiguration.h"

#include <iostream>
#include <algorithm>
#include <boost/program_options.hpp>

#include "Logging/ILogger.h"
#include "CryptoNoteConfig.h"

namespace po = boost::program_options;

namespace PaymentService {

Configuration::Configuration() {
  generateNewContainer = false;
  daemonize = false;
  registerService = false;
  unregisterService = false;
  containerPassword = "";
  logFile = "walletd.log";
  testnet = false;
  printAddresses = false;
  syncFromZero = false;
  logLevel = Logging::INFO;
  m_bind_address = "";
  m_bind_port = 0;
  m_bind_port_ssl = 0;
  m_rpcPassword = "";
  secretViewKey = "";
  secretSpendKey = "";
  mnemonicSeed = "";
  m_enable_ssl = false;
  m_chain_file = "";
  m_key_file = "";
  m_dh_file = "";
  legacySecurity = false;
}

void Configuration::initOptions(po::options_description& desc) {
  desc.add_options()
      ("bind-address", po::value<std::string>()->default_value("127.0.0.1"), "payment service bind address")
      ("bind-port", po::value<uint16_t>()->default_value((uint16_t) CryptoNote::GATE_RPC_DEFAULT_PORT), "payment service bind port")
      ("bind-port-ssl", po::value<uint16_t>()->default_value((uint16_t) CryptoNote::GATE_RPC_DEFAULT_SSL_PORT), "payment service bind port ssl")
      ("rpc-password", po::value<std::string>(), "Specify the password to access the RPC server.")
      ("rpc-ssl-enable", po::bool_switch(), "Enable SSL for RPC service")
      ("rpc-chain-file", po::value<std::string>()->default_value(std::string(CryptoNote::RPC_DEFAULT_CHAIN_FILE)), "SSL chain file")
      ("rpc-key-file", po::value<std::string>()->default_value(std::string(CryptoNote::RPC_DEFAULT_KEY_FILE)), "SSL key file")
      ("rpc-dh-file", po::value<std::string>()->default_value(std::string(CryptoNote::RPC_DEFAULT_DH_FILE)), "SSL DH file")
      ("rpc-legacy-security", "Enable legacy mode (no password for RPC). WARNING: INSECURE. USE ONLY AS A LAST RESORT.")
      ("container-file,w", po::value<std::string>(), "container file")
      ("container-password,p", po::value<std::string>(), "container password")
      ("generate-container,g", "generate new container file with one wallet and exit")
      ("view-key", po::value<std::string>(), "generate a container with this secret view key")
      ("spend-key", po::value<std::string>(), "generate a container with this secret spend key")
      ("mnemonic-seed", po::value<std::string>(), "generate a container with this mnemonic seed")
      ("daemon,d", "run as daemon in Unix or as service in Windows")
#ifdef _WIN32
      ("register-service", "register service and exit (Windows only)")
      ("unregister-service", "unregister service and exit (Windows only)")
#endif
      ("log-file,l", po::value<std::string>(), "log file")
      ("server-root", po::value<std::string>(), "server root. The service will use it as working directory. Don't set it if don't want to change it")
      ("log-level", po::value<size_t>(), "log level")
      ("SYNC_FROM_ZERO", "sync from timestamp 0")
      ("address", "print wallet addresses and exit");
}

void Configuration::init(const po::variables_map& options) {
  if (options.count("daemon") != 0) {
    daemonize = true;
  }

  if (options.count("register-service") != 0) {
    registerService = true;
  }

  if (options.count("unregister-service") != 0) {
    unregisterService = true;
  }

  if (registerService && unregisterService) {
    throw ConfigurationError("It's impossible to use both \"register-service\" and \"unregister-service\" at the same time");
  }

  if (options["testnet"].as<bool>()) {
    testnet = true;
  }

  if (options.count("log-file") != 0) {
    logFile = options["log-file"].as<std::string>();
  }

  if (options.count("log-level") != 0) {
    logLevel = options["log-level"].as<size_t>();
    if (logLevel > Logging::TRACE) {
      std::string error = "log-level option must be in " + std::to_string(Logging::FATAL) +  ".." + std::to_string(Logging::TRACE) + " interval";
      throw ConfigurationError(error.c_str());
    }
  }

  if (options.count("server-root") != 0) {
    serverRoot = options["server-root"].as<std::string>();
  }

  if (options.count("bind-address") != 0 && (!options["bind-address"].defaulted() || m_bind_address.empty())) {
    m_bind_address = options["bind-address"].as<std::string>();
  }

  if (options.count("bind-port") != 0 && (!options["bind-port"].defaulted() || m_bind_port == 0)) {
    m_bind_port = options["bind-port"].as<uint16_t>();
  }

  if (options.count("bind-port-ssl") != 0 && (!options["bind-port-ssl"].defaulted() || m_bind_port_ssl == 0)) {
    m_bind_port_ssl = options["bind-port-ssl"].as<uint16_t>();
  }

  if (options.count("rpc-password") != 0) {
    m_rpcPassword = options["rpc-password"].as<std::string>();
  }

  if (options["rpc-ssl-enable"].as<bool>()){
    m_enable_ssl = true;
  }

  if (options.count("rpc-chain-file") != 0 && (!options["rpc-chain-file"].defaulted() || m_chain_file.empty())) {
    m_chain_file = options["rpc-chain-file"].as<std::string>();
  }

  if (options.count("rpc-key-file") != 0 && (!options["rpc-key-file"].defaulted() || m_key_file.empty())) {
    m_key_file = options["rpc-key-file"].as<std::string>();
  }

  if (options.count("rpc-dh-file") != 0 && (!options["rpc-dh-file"].defaulted() || m_dh_file.empty())) {
    m_dh_file = options["rpc-dh-file"].as<std::string>();
  }

  if (options.count("container-file") != 0) {
    containerFile = options["container-file"].as<std::string>();
  }

  if (options.count("container-password") != 0) {
    containerPassword = options["container-password"].as<std::string>();
  }

  if (options.count("generate-container") != 0) {
    generateNewContainer = true;
  }

  if (options.count("view-key") != 0) {
    if (!generateNewContainer) {
      throw ConfigurationError("generate-container parameter is required");
    }
    secretViewKey = options["view-key"].as<std::string>();
  }

  if (options.count("spend-key") != 0) {
    if (!generateNewContainer) {
      throw ConfigurationError("generate-container parameter is required");
    }
    secretSpendKey = options["spend-key"].as<std::string>();
  }

  if (options.count("mnemonic-seed") != 0) {
    if (!generateNewContainer) {
      throw ConfigurationError("generate-container parameter is required");
    } else if (options.count("spend-key") != 0 || options.count("view-key") != 0) {
      throw ConfigurationError("Cannot specify import via both mnemonic seed and private keys");
    }
    mnemonicSeed = options["mnemonic-seed"].as<std::string>();
  }

  if (options.count("address") != 0) {
    printAddresses = true;
  }

  if (options.count("SYNC_FROM_ZERO") != 0) {
    syncFromZero = true;
  }
  if (!registerService && !unregisterService) {
    if (containerFile.empty() && containerPassword.empty()) {
      throw ConfigurationError("Both container-file and container-password parameters are required");
    }
	if (containerPassword.empty()) {
		if (pwd_container.read_password()) {
			containerPassword = pwd_container.password();
		}
    }
  }

  // If generating a container skip the authentication parameters.
  if (generateNewContainer) {
    return;
  }

  // Check for the authentication parameters
  if ((options.count("rpc-password") == 0) && (options.count("rpc-legacy-security") == 0)) {
    throw ConfigurationError("Please specify an RPC password or use the --rpc-legacy-security flag.");
  }

  if (options.count("rpc-legacy-security") != 0) {
    legacySecurity = true;
  }
  else {
    m_rpcPassword = options["rpc-password"].as<std::string>();
  }

}

} //namespace PaymentService
