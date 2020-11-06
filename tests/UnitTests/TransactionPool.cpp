// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2019, The Bittorium developers
// Copyright (c) 2020, The Talleo developers
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

#include "gtest/gtest.h"

#include <algorithm>

#include <boost/filesystem/operations.hpp>

#include "CryptoNoteCore/Account.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/Currency.h"
#include "CryptoNoteCore/ITransactionValidator.h"
#include "CryptoNoteCore/ITimeProvider.h"
#include "CryptoNoteCore/TransactionExtra.h"
#include "CryptoNoteCore/TransactionPool.h"

#include <Logging/ConsoleLogger.h>
#include <Logging/LoggerGroup.h>

#include "TransactionApiHelpers.h"

using namespace CryptoNote;
using namespace CryptoNote;

class TransactionValidator : public CryptoNote::ITransactionValidator {
  virtual bool checkTransactionInputs(const CryptoNote::Transaction& tx, BlockInfo& maxUsedBlock) override {
    return true;
  }

  virtual bool checkTransactionInputs(const CryptoNote::Transaction& tx, BlockInfo& maxUsedBlock, BlockInfo& lastFailed) override {
    return true;
  }

  virtual bool haveSpentKeyImages(const CryptoNote::Transaction& tx) override {
    return false;
  }

  virtual bool checkTransactionSize(size_t blobSize) override {
    return true;
  }
};

class FakeTimeProvider : public ITimeProvider {
public:
  FakeTimeProvider(time_t currentTime = time(nullptr))
    : timeNow(currentTime) {}

  time_t timeNow;
  virtual time_t now() override { return timeNow; }
};


class TestTransactionGenerator {

public:

  TestTransactionGenerator(const CryptoNote::Currency& currency, size_t ringSize) :
    m_currency(currency),
    m_ringSize(ringSize),
    m_miners(ringSize),
    m_miner_txs(ringSize),
    m_public_keys(ringSize),
    m_public_key_ptrs(ringSize)
  {
    rv_acc.generate();
  }

  bool createSources() {

    size_t real_source_idx = m_ringSize / 2;

    std::vector<TransactionSourceEntry::OutputEntry> output_entries;
    for (uint32_t i = 0; i < m_ringSize; ++i)
    {
      m_miners[i].generate();

      if (!m_currency.constructMinerTx(BLOCK_MAJOR_VERSION_1, 0, 0, 0, 2, 0, m_miners[i].getAccountKeys().address, m_miner_txs[i])) {
        return false;
      }

      KeyOutput tx_out = boost::get<KeyOutput>(m_miner_txs[i].outputs[0].target);
      output_entries.push_back(std::make_pair(i, tx_out.key));
      m_public_keys[i] = tx_out.key;
      m_public_key_ptrs[i] = &m_public_keys[i];
    }

    m_source_amount = m_miner_txs[0].outputs[0].amount;

    TransactionSourceEntry source_entry;
    source_entry.amount = m_source_amount;
    source_entry.realTransactionPublicKey = getTransactionPublicKeyFromExtra(m_miner_txs[real_source_idx].extra);
    source_entry.realOutputIndexInTransaction = 0;
    source_entry.outputs.swap(output_entries);
    source_entry.realOutput = real_source_idx;

    m_sources.push_back(source_entry);

    m_realSenderKeys = m_miners[real_source_idx].getAccountKeys();

    return true;
  }

  void construct(uint64_t amount, uint64_t fee, size_t outputs, Transaction& tx) {

    std::vector<TransactionDestinationEntry> destinations;
    uint64_t amountPerOut = (amount - fee) / outputs;

    for (size_t i = 0; i < outputs; ++i) {
      destinations.push_back(TransactionDestinationEntry(amountPerOut, rv_acc.getAccountKeys().address));
    }

    constructTransaction(m_realSenderKeys, m_sources, destinations, std::vector<uint8_t>(), tx, 0, m_logger);
  }

  std::vector<AccountBase> m_miners;
  std::vector<Transaction> m_miner_txs;
  std::vector<TransactionSourceEntry> m_sources;
  std::vector<Crypto::PublicKey> m_public_keys;
  std::vector<const Crypto::PublicKey*> m_public_key_ptrs;

  Logging::LoggerGroup m_logger;
  const CryptoNote::Currency& m_currency;
  const size_t m_ringSize;
  AccountKeys m_realSenderKeys;
  uint64_t m_source_amount;
  AccountBase rv_acc;
};

class tx_pool : public ::testing::Test {
public:

  tx_pool() :
    currency(CryptoNote::CurrencyBuilder(logger).currency()) {}

protected:
  virtual void SetUp() override {
    m_configDir = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("test_data_%%%%%%%%%%%%");
  }

  virtual void TearDown() override {
    boost::system::error_code ignoredErrorCode;
    boost::filesystem::remove_all(m_configDir, ignoredErrorCode);
  }

protected:
  Logging::ConsoleLogger logger;
  CryptoNote::Currency currency;
  boost::filesystem::path m_configDir;
};

namespace
{
  template <typename Validator, typename TimeProvider>
  class TestPool {
  public:

    Validator validator;
    TimeProvider timeProvider;

    TestPool(const CryptoNote::Currency& currency, Logging::ILogger& logger) {}
  };

  class TxTestBase {
  public:
    TxTestBase(size_t ringSize) :
      m_currency(CryptoNote::CurrencyBuilder(m_logger).currency()),
      txGenerator(m_currency, ringSize)
    {
      txGenerator.createSources();
    }

    void construct(uint64_t fee, size_t outputs, Transaction& tx) {
      txGenerator.construct(txGenerator.m_source_amount, fee, outputs, tx);
    }

    Logging::ConsoleLogger m_logger;
    CryptoNote::Currency m_currency;
    CryptoNote::RealTimeProvider m_time;
    TestTransactionGenerator txGenerator;
    TransactionValidator validator;
  };

}
