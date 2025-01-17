// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2019, The Bittorium developers
// Copyright (c) 2023, The Talleo developers
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

#include "Miner.h"

#include <functional>

#include "boost/thread/thread.hpp"
#include "crypto/crypto.h"
#include "CryptoNoteCore/CachedBlock.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"

#include <System/InterruptedException.h>

namespace CryptoNote {

Miner::Miner(System::Dispatcher& dispatcher, Logging::ILogger& logger) :
  m_dispatcher(dispatcher),
  m_miningStopped(dispatcher),
  m_state(MiningState::MINING_STOPPED),
  m_logger(logger, "Miner") {
}

Miner::~Miner() {
  assert(m_state != MiningState::MINING_IN_PROGRESS);
}

BlockTemplate Miner::mine(const BlockMiningParameters& blockMiningParameters, size_t threadCount) {
  MiningState state = MiningState::MINING_STOPPED;

  if (threadCount == 0) {
    throw std::runtime_error("Miner requires at least one thread");
  }

  while (!m_state.compare_exchange_weak(state, MiningState::MINING_IN_PROGRESS)) {
    boost::this_thread::sleep_for(boost::chrono::seconds(1));
  }

  m_miningStopped.clear();

  runWorkers(blockMiningParameters, threadCount);

  assert(m_state != MiningState::MINING_IN_PROGRESS);
  if (m_state == MiningState::MINING_STOPPED) {
    m_logger(Logging::DEBUGGING) << "Mining has been stopped";
    throw System::InterruptedException();
  }

  assert(m_state == MiningState::BLOCK_FOUND);
  return m_block;
}

void Miner::stop() {
  MiningState state = MiningState::MINING_IN_PROGRESS;

  if (m_state.compare_exchange_weak(state, MiningState::MINING_STOPPED)) {
    m_miningStopped.wait();
    m_miningStopped.clear();
  }
}

void Miner::runWorkers(BlockMiningParameters blockMiningParameters, size_t threadCount) {
  assert(threadCount > 0);

  m_logger(Logging::INFO) << "Starting mining for difficulty " << blockMiningParameters.difficulty;

  m_hashCount = 0;

  m_workers.emplace_back(std::unique_ptr<System::RemoteContext<void>> (
    new System::RemoteContext<void>(m_dispatcher, std::bind(&Miner::hashWorkerFunc, this)))
  );

  try {
    blockMiningParameters.blockTemplate.nonce = Crypto::rand<uint32_t>();
    for (size_t i = 0; i < threadCount; ++i) {
      m_workers.emplace_back(std::unique_ptr<System::RemoteContext<void>> (
        new System::RemoteContext<void>(m_dispatcher, std::bind(&Miner::workerFunc, this, blockMiningParameters.blockTemplate, blockMiningParameters.difficulty, static_cast<uint32_t>(threadCount))))
      );
      blockMiningParameters.blockTemplate.nonce++;
    }

    m_workers.clear();

  } catch (std::exception& e) {
    m_logger(Logging::ERROR) << "Error occurred during mining: " << e.what();
    m_state = MiningState::MINING_STOPPED;
  }

  m_miningStopped.set();
}

void Miner::hashWorkerFunc() {
  time_t startTime = time(NULL);

  while (m_state == MiningState::MINING_IN_PROGRESS) {
    boost::this_thread::sleep_for(boost::chrono::seconds(10));
    time_t currentTime = time(NULL);
    uint64_t elapsedTime = currentTime - startTime;
    double hashesPerSecond = (double)m_hashCount / elapsedTime;
    std::string hashUnit = "H";
    if (hashesPerSecond >= 1000000) {
      hashUnit = "MH";
      hashesPerSecond /= 1000000;
    } else if (hashesPerSecond > 1000) {
      hashUnit = "kH";
      hashesPerSecond /= 1000;
    }
    m_logger(Logging::INFO) << "Current hash rate: " << std::fixed << std::setprecision(3) << hashesPerSecond << " " << hashUnit << "/s";
  }
}

void Miner::workerFunc(const BlockTemplate& blockTemplate, Difficulty difficulty, uint32_t nonceStep) {
  try {
    BlockTemplate block = blockTemplate;

    while (m_state == MiningState::MINING_IN_PROGRESS) {
      CachedBlock cachedBlock(block);
      Crypto::Hash hash = cachedBlock.getBlockLongHash();
      m_hashCount++;

      if (check_hash(hash, difficulty)) {
        if (setStateBlockFound()) {
          m_logger(Logging::INFO) << "Found block for difficulty " << difficulty;
          m_block = block;
          return;
        }
        m_logger(Logging::DEBUGGING) << "block was already found or mining stopped";
        return;
      }

      block.nonce += nonceStep;
    }
  } catch (std::exception& e) {
    m_logger(Logging::ERROR) << "Miner got error: " << e.what();
    m_state = MiningState::MINING_STOPPED;
  }
}

bool Miner::setStateBlockFound() {
  auto state = m_state.load();

  for (;;) {
    switch (state) {
      case MiningState::BLOCK_FOUND:
        return false;

      case MiningState::MINING_IN_PROGRESS:
        if (m_state.compare_exchange_weak(state, MiningState::BLOCK_FOUND)) {
          return true;
        }
        break;

      case MiningState::MINING_STOPPED:
        return false;

      default:
        assert(false);
        return false;
    }
  }
}

} //namespace CryptoNote
