// Copyright (c) 2021, The Talleo developers
//
// This file is part of Talleo.
//
// Talleo is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Talleo is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Talleo.  If not, see <http://www.gnu.org/licenses/>.

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <boost/program_options.hpp>
#include <chrono>
#include <mutex>
#include <random>

#include "Common/int-util.h"
#include "Common/Base58.h"
#include "Common/CommandLine.h"
#include "Common/StringTools.h"
#include "CryptoNoteCore/Account.h"
#include "CryptoNoteCore/Currency.h"
#include "Logging/LoggerGroup.h"
#include "Logging/ConsoleLogger.h"
#include "Logging/LoggerRef.h"
#include "Logging/LoggerManager.h"
#include "SimpleWallet/Tools.h"
#include "System/Dispatcher.h"
#include "System/RemoteContext.h"

#include "CryptoNoteConfig.h"
#include "CryptoTypes.h"
#include "version.h"

using namespace CryptoNote;
using namespace Logging;

namespace po = boost::program_options;

std::mutex outputMutex;

namespace command_line
{
  const command_line::arg_descriptor<std::string> arg_prefix = {"prefix", "Specify address prefix"};
  const command_line::arg_descriptor<std::string> arg_address = {"address", "Specify full address"};
  const command_line::arg_descriptor<int> arg_count = {"count", "Specify number of prefixes to find", 1};
  const command_line::arg_descriptor<int> arg_threads = {"threads", "Specify threads to use", 1};
  const command_line::arg_descriptor<bool> arg_randomize = {"randomize", "Randomize starting key", false};
  const command_line::arg_descriptor<std::string> arg_spend = {"spend", "Specify spend key to start scanning at"};
  const command_line::arg_descriptor<std::string> arg_view = {"view", "Specify view key to start scanning at"};
}

struct keys {
    union {
        Crypto::SecretKey spendKey;
        uint64_t spendQuads[4];
    };
    union {
        Crypto::SecretKey viewKey;
        uint64_t viewQuads[4];
    };
};

bool check_address_prefix(const std::string& prefix, Currency& currency, int threads, int threadId, volatile int& found, int count, keys& _keys) {
     if (found == count) {
         return true;
     }
     CryptoNote::AccountPublicAddress publicKeys;
     if (secret_key_to_public_key(_keys.spendKey, publicKeys.spendPublicKey)) {
          // Make sure the generated keys are deterministic
          AccountBase::generateViewFromSpend(_keys.spendKey, _keys.viewKey, publicKeys.viewPublicKey);
          std::string address = currency.accountAddressAsString(publicKeys);
          if ((address.substr(0, prefix.length()) == prefix)) {
              std::lock_guard<std::mutex> guard(outputMutex);
              std::cout << InformationMsg("Address:   ") << address << std::endl;
              std::cout << InformationMsg("Spend key: ") << Common::podToHex(_keys.spendKey) << std::endl;
              std::cout << InformationMsg("View key:  ") << Common::podToHex(_keys.viewKey) << std::endl << std::endl;
              found++;
          }
    }
    return false;
}

void prefix_worker(const std::string& prefix, Currency& currency, int threads, int threadId, volatile int& found, int count, const keys& startKeys) {
    keys _keys;
    uint64_t last;
    if (threads == 1) {
        last = UINT64_MAX;
    } else { // Calculate value before last round
        uint64_t last_start = UINT64_MAX - threads + 1;
        last = last_start + (((last_start % threads) + threadId) % threads);
    }
    for (uint64_t a = 0; a < UINT64_MAX; a++) {
        _keys.spendQuads[3] = SWAP64LE(a + SWAP64LE(startKeys.spendQuads[3]));
        for (uint64_t b = 0; b < UINT64_MAX; b++) {
            _keys.spendQuads[2] = SWAP64LE(b + SWAP64LE(startKeys.spendQuads[2]));
            for (uint64_t c = 0; c < UINT64_MAX; c++) {
                _keys.spendQuads[1] = SWAP64LE(c + SWAP64LE(startKeys.spendQuads[1]));
                for (uint64_t d = threadId; d < last; d+=threads) {
                    _keys.spendQuads[0] = SWAP64LE(d + SWAP64LE(startKeys.spendQuads[0]));
                    if (check_address_prefix(prefix, currency, threads, threadId, found, count, _keys)) {
                        return;
                    }
                }
                // Avoid wraparound
                _keys.spendQuads[0] = SWAP64LE(last + SWAP64LE(startKeys.spendQuads[0]));
                if (check_address_prefix(prefix, currency, threads, threadId, found, count, _keys)) {
                    return;
                }
            }
            // Avoid wraparound
            _keys.spendQuads[1] = SWAP64LE(SWAP64LE(startKeys.spendQuads[1]) - 1);
            if (check_address_prefix(prefix, currency, threads, threadId, found, count, _keys)) {
                return;
            }
        }
        // Avoid wraparound
        _keys.spendQuads[2] = SWAP64LE(SWAP64LE(startKeys.spendQuads[2]) - 1);
        if (check_address_prefix(prefix, currency, threads, threadId, found, count, _keys)) {
            return;
        }
    }
    // Avoid wraparound
    _keys.spendQuads[3] = SWAP64LE(SWAP64LE(startKeys.spendQuads[3]) - 1);
    (void)check_address_prefix(prefix, currency, threads, threadId, found, count, _keys);
    return;
}

bool find_prefix(const po::variables_map& vm, Currency& currency, System::Dispatcher& dispatcher, uint64_t* start) {
    std::string prefix = command_line::get_arg(vm, command_line::arg_prefix);
    int count = command_line::get_arg(vm, command_line::arg_count);
    int threads = std::max(1, command_line::get_arg(vm, command_line::arg_threads));
    int found = 0;

    if ((prefix.substr(0, 2) != "TA") || prefix.length() > 97) {
        std::cerr << WarningMsg("Invalid address prefix!") << std::endl;
        return false;
    }

    std::string data;
    if (!Tools::Base58::decode(prefix, data)) {
        std::cerr << WarningMsg("Invalid character in prefix!") << std::endl;
        return false;
    }

    keys startKeys;
    startKeys.spendQuads[0] = start[7];
    startKeys.spendQuads[1] = start[6];
    startKeys.spendQuads[2] = start[5];
    startKeys.spendQuads[3] = start[4];
    std::cout << InformationMsg("Trying to find prefix \"") << prefix << InformationMsg("\", starting from ") << Common::podToHex(startKeys.spendKey) << InformationMsg("...") << std::endl;

    std::vector<std::unique_ptr<System::RemoteContext<void>>> m_workers;

    for (int i = 0; i < threads; i++) {
        m_workers.emplace_back(
               new System::RemoteContext<void>(dispatcher, [&, i]() {
                   prefix_worker(prefix, currency, threads, i, found, count, startKeys);
               })
        );
    }

    m_workers.clear();
    return found != 0;
}

bool check_address(const std::string& address, Currency& currency, int threads, int threadId, bool& found, keys& _keys) {
    CryptoNote::AccountPublicAddress publicKeys;
    if (secret_key_to_public_key(_keys.viewKey, publicKeys.viewPublicKey) && secret_key_to_public_key(_keys.spendKey, publicKeys.spendPublicKey)) {
        std::string _address = currency.accountAddressAsString(publicKeys);
        if (address == _address) {
            std::lock_guard<std::mutex> guard(outputMutex);
            std::cout << InformationMsg("Spend key: ") << Common::podToHex(_keys.spendKey) << std::endl;
            std::cout << InformationMsg("View key:  ") << Common::podToHex(_keys.viewKey) << std::endl << std::endl;
            return true;
        }
    }
    return false;
}

void address_worker(const std::string& address, Currency& currency, int threads, int threadId, bool& found, const keys& startKeys) {
    keys _keys;
    uint64_t last;
    if (threads == 1) {
        last = UINT64_MAX;
    } else { // Calculate value before last round
        uint64_t last_start = UINT64_MAX - threads + 1;
        last = last_start + (((last_start % threads) + threadId) % threads);
    }
    for (uint64_t a = threadId; a < last; a+=threads) {
        _keys.viewQuads[3] = SWAP64LE(a + SWAP64LE(startKeys.viewQuads[3]));
        for (uint64_t b = 0; b < UINT64_MAX; b++) {
            _keys.viewQuads[2] = SWAP64LE(b + SWAP64LE(startKeys.viewQuads[2]));
            for (uint64_t c = 0; c < UINT64_MAX; c++) {
                _keys.viewQuads[1] = SWAP64LE(c + SWAP64LE(startKeys.viewQuads[1]));
                for (uint64_t d = 0; d < UINT64_MAX; d++) {
                    _keys.viewQuads[0] = SWAP64LE(d + SWAP64LE(startKeys.viewQuads[0]));
                    for (uint64_t e = 0; e < UINT64_MAX; e++) {
                        _keys.spendQuads[3] = SWAP64LE(e + SWAP64LE(startKeys.spendQuads[3]));
                        for (uint64_t f = 0; f < UINT64_MAX; f++) {
                            _keys.spendQuads[2] = SWAP64LE(f + SWAP64LE(startKeys.spendQuads[2]));
                            for (uint64_t g = 0; g < UINT64_MAX; g++) {
                                _keys.spendQuads[1] = SWAP64LE(g + SWAP64LE(startKeys.spendQuads[1]));
                                for (uint64_t h = 0; h < UINT64_MAX; h++) {
                                    _keys.spendQuads[0] = SWAP64LE(h + SWAP64LE(startKeys.spendQuads[0]));
                                    if (check_address(address, currency, threads, threadId, found, _keys)) {
                                        return;
                                    }
                                }
                                // Avoid wraparound
                                _keys.spendQuads[0] = SWAP64LE(SWAP64LE(startKeys.spendQuads[0]) - 1);
                                if (check_address(address, currency, threads, threadId, found, _keys)) {
                                    return;
                                }
                            }
                            // Avoid wraparound
                            _keys.spendQuads[1] = SWAP64LE(SWAP64LE(startKeys.spendQuads[1]) - 1);
                            if (check_address(address, currency, threads, threadId, found, _keys)) {
                                return;
                            }
                        }
                        // Avoid wraparound
                        _keys.spendQuads[2] = SWAP64LE(SWAP64LE(startKeys.spendQuads[2]) - 1);
                        if (check_address(address, currency, threads, threadId, found, _keys)) {
                            return;
                        }
                    }
                    // Avoid wraparound
                    _keys.spendQuads[3] = SWAP64LE(SWAP64LE(startKeys.spendQuads[3]) - 1);
                    if (check_address(address, currency, threads, threadId, found, _keys)) {
                        return;
                    }
                }
                // Avoid wraparound
                _keys.viewQuads[0] = SWAP64LE(SWAP64LE(startKeys.viewQuads[0]) - 1);
                if (check_address(address, currency, threads, threadId, found, _keys)) {
                    return;
                }
            }
            // Avoid wraparound
            _keys.viewQuads[1] = SWAP64LE(SWAP64LE(startKeys.viewQuads[1]) - 1);
            if (check_address(address, currency, threads, threadId, found, _keys)) {
                return;
            }
        }
        // Avoid wraparound
        _keys.viewQuads[2] = SWAP64LE(SWAP64LE(startKeys.viewQuads[2]) - 1);
        if (check_address(address, currency, threads, threadId, found, _keys)) {
            return;
        }
    }
    // Avoid wraparound
    _keys.viewQuads[3] = SWAP64LE(last + SWAP64LE(startKeys.viewQuads[3]));
    (void)check_address(address, currency, threads, threadId, found, _keys);
    return;
}

bool find_address(const po::variables_map& vm, Currency& currency, System::Dispatcher& dispatcher, uint64_t* start) {
    std::string address = command_line::get_arg(vm, command_line::arg_address);
    int threads = std::max(1, command_line::get_arg(vm, command_line::arg_threads));
    bool found;

    if ((address.substr(0, 2) != "TA") || (address.length() != 97)) {
        std::cerr << WarningMsg("Invalid address prefix!") << std::endl;
        return false;
    }

    CryptoNote::AccountPublicAddress publicKeys;
    if (!currency.parseAccountAddressString(address, publicKeys)) {
        std::cerr << WarningMsg("Invalid address!") << std::endl;
        return false;
    }

    keys startKeys;
    startKeys.spendQuads[0] = start[7];
    startKeys.spendQuads[1] = start[6];
    startKeys.spendQuads[2] = start[5];
    startKeys.spendQuads[3] = start[4];
    startKeys.viewQuads[0] = start[3];
    startKeys.viewQuads[1] = start[2];
    startKeys.viewQuads[2] = start[1];
    startKeys.viewQuads[3] = start[0];

    std::cout << InformationMsg("Trying to find address \"") << address << InformationMsg("\", starting from:") << std::endl;
    std::cout << InformationMsg("Spend key:") << Common::podToHex(startKeys.spendKey) << std::endl;
    std::cout << InformationMsg("View key: ") << Common::podToHex(startKeys.viewKey) << std::endl << std::endl;

    std::vector<std::unique_ptr<System::RemoteContext<void>>> m_workers;

    for (int i = 0; i < threads; i++) {
        m_workers.emplace_back(
             new System::RemoteContext<void>(dispatcher, [&, i]() {
                   address_worker(address, currency, threads, i, found, startKeys);
             })
        );
    }

    m_workers.clear();
    return found;
}

int main(int argc, char** argv) {
    LoggerManager logManager;
    LoggerRef logger(logManager, "generate");

    CurrencyBuilder builder(logManager);
    Currency currency = builder.currency();

    System::Dispatcher dispatcher;

#ifdef _WIN32
    std::string consoletitle = std::string(CryptoNote::CRYPTONOTE_NAME) + " AddressGenerator v" + std::string(PROJECT_VERSION_LONG);
    SetConsoleTitleA(consoletitle.c_str());
#endif

    std::string coinName(CryptoNote::CRYPTONOTE_NAME);
    std::cout << InformationMsg(coinName + " v" + std::string(PROJECT_VERSION) + " AddressGenerator") << std::endl;

    try {
        po::options_description desc_cmd_only("Command line options");

        command_line::add_arg(desc_cmd_only, command_line::arg_prefix);
        command_line::add_arg(desc_cmd_only, command_line::arg_address);
        command_line::add_arg(desc_cmd_only, command_line::arg_count);
        command_line::add_arg(desc_cmd_only, command_line::arg_threads);
        command_line::add_arg(desc_cmd_only, command_line::arg_randomize);
        command_line::add_arg(desc_cmd_only, command_line::arg_spend);
        command_line::add_arg(desc_cmd_only, command_line::arg_view);

        command_line::add_arg(desc_cmd_only, command_line::arg_help);

        po::options_description desc_options("Allowed options");
        desc_options.add(desc_cmd_only);


        bool r = command_line::handle_error_helper(desc_options, [&]()
        {
            po::variables_map vm;
            po::store(po::parse_command_line(argc, argv, desc_options), vm);
            uint64_t start[8] = {0, 0, 0, 0, 0, 0, 0, 0};

            if (command_line::get_arg(vm, command_line::arg_randomize)) {
                uint64_t seed1 = std::chrono::system_clock::now().time_since_epoch().count();
                std::mt19937_64 gen64(seed1);
                start[0] = gen64();
                start[1] = gen64();
                start[2] = gen64();
                start[3] = gen64();
                start[4] = gen64();
                start[5] = gen64();
                start[6] = gen64();
                start[7] = gen64();
            }

            std::string spendKey = command_line::get_arg(vm, command_line::arg_spend);
            std::string viewKey = command_line::get_arg(vm, command_line::arg_view);
            keys startKeys;

            if (!spendKey.empty()) {
                if (!Common::podFromHex(spendKey, startKeys.spendKey)) {
                    logger(ERROR, BRIGHT_RED) << "Invalid spend key!";
                    return false;
                }
                start[4] = startKeys.spendQuads[3];
                start[5] = startKeys.spendQuads[2];
                start[6] = startKeys.spendQuads[1];
                start[7] = startKeys.spendQuads[0];
            }

            if (!viewKey.empty()) {
                if (!Common::podFromHex(viewKey, startKeys.viewKey)) {
                    logger(ERROR, BRIGHT_RED) << "Invalid view key!";
                    return false;
                }
                start[0] = startKeys.viewQuads[3];
                start[1] = startKeys.viewQuads[2];
                start[2] = startKeys.viewQuads[1];
                start[3] = startKeys.viewQuads[0];
            }

            if (command_line::get_arg(vm, command_line::arg_help)) {
                std::cout << desc_options << std::endl;
                return false;
            }

            if (!command_line::get_arg(vm, command_line::arg_prefix).empty()) {
                return find_prefix(vm, currency, dispatcher, start);
            }

            if (!command_line::get_arg(vm, command_line::arg_address).empty()) {
                return find_address(vm, currency, dispatcher, start);
            }
            return true;
        });

        if (!r)
            return 1;
    } catch (const std::exception& e) {
        logger(ERROR, BRIGHT_RED) << "Exception: " << e.what();
        return 1;
    }
    return 0;
}
