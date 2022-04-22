// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2022, The Talleo developers
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

#include "NodeRpcStub.h"

namespace CryptoNote {

	NodeRpcStub::~NodeRpcStub() {
	}

	bool NodeRpcStub::addObserver(CryptoNote::INodeObserver* observer) {
		return true;
	}

	bool NodeRpcStub::removeObserver(CryptoNote::INodeObserver* observer) {
		return true;
	}

	void NodeRpcStub::init(const Callback& callback) {
	}

	bool NodeRpcStub::shutdown() {
		return true;
	}

	size_t NodeRpcStub::getPeerCount() const {
		return 0;
	}

	uint32_t NodeRpcStub::getLastLocalBlockHeight() const {
		return 0;
	}

	uint32_t NodeRpcStub::getLastKnownBlockHeight() const {
		return 0;
	}

	uint32_t NodeRpcStub::getLocalBlockCount() const {
		return 0;
	}

	uint32_t NodeRpcStub::getKnownBlockCount() const {
		return 0;
	}

	uint64_t NodeRpcStub::getLastLocalBlockTimestamp() const {
		return 0;
	}

	std::string NodeRpcStub::getLastFeeAddress() const
	{
		return "";
	}

	std::string NodeRpcStub::getLastCollateralHash() const {
		return "";
	}

	void NodeRpcStub::getBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount, std::vector<Crypto::Hash>& blockHashes, const Callback& callback) {
		callback(std::error_code());
	}

	void NodeRpcStub::getTransactionHashesByPaymentId(const Crypto::Hash& paymentId, std::vector<Crypto::Hash>& transactionHashes, const Callback& callback) {
		callback(std::error_code());
	}

	CryptoNote::BlockHeaderInfo NodeRpcStub::getLastLocalBlockHeaderInfo() const {
		return CryptoNote::BlockHeaderInfo();
	}

	void NodeRpcStub::relayTransaction(const CryptoNote::Transaction& transaction, const Callback& callback) {
		callback(std::error_code());
	}

	void NodeRpcStub::getRandomOutsByAmounts(std::vector<uint64_t>&& amounts, uint16_t outsCount,
		                                     std::vector<CryptoNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& result, const Callback& callback) {
	}

	void NodeRpcStub::getNewBlocks(std::vector<Crypto::Hash>&& knownBlockIds, std::vector<CryptoNote::RawBlock>& newBlocks, uint32_t& startHeight, const Callback& callback) {
		startHeight = 0;
		callback(std::error_code());
	}

	void NodeRpcStub::getTransactionOutsGlobalIndices(const Crypto::Hash& transactionHash, std::vector<uint32_t>& outsGlobalIndices, const Callback& callback) {
	}

	void NodeRpcStub::queryBlocks(std::vector<Crypto::Hash>&& knownBlockIds, uint64_t timestamp, std::vector<CryptoNote::BlockShortEntry>& newBlocks, uint32_t& startHeight,
		                          const Callback& callback) {
		startHeight = 0;
		callback(std::error_code());
	};

	void NodeRpcStub::getPoolSymmetricDifference(std::vector<Crypto::Hash>&& knownPoolTxIds, Crypto::Hash knownBlockId, bool& isBcActual,
		                                         std::vector<std::unique_ptr<CryptoNote::ITransactionReader>>& newTxs, std::vector<Crypto::Hash>& deletedTxIds, const Callback& callback) {
		isBcActual = true;
		callback(std::error_code());
	}

	void NodeRpcStub::getBlocks(const std::vector<uint32_t>& blockHeights, std::vector<std::vector<CryptoNote::BlockDetails>>& blocks, const Callback& callback) {
		callback(std::error_code());
	}

	void NodeRpcStub::getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<CryptoNote::BlockDetails>& blocks, const Callback& callback) {
		callback(std::error_code());
	}

	void NodeRpcStub::getBlock(const uint32_t blockHeight, CryptoNote::BlockDetails &block, const Callback& callback) {
		callback(std::error_code());
	}

	void NodeRpcStub::getTransactions(const std::vector<Crypto::Hash>& transactionHashes, std::vector<CryptoNote::TransactionDetails>& transactions,
			                          const Callback& callback) {
		callback(std::error_code());
	}

	void NodeRpcStub::getFeeAddress(std::string& feeAddress, const Callback& callback) {
		callback(std::error_code());
	}

	void NodeRpcStub::getCollateralHash(std::string& collateralHash, const Callback& callback) {
		callback(std::error_code());
	}

	void NodeRpcStub::isSynchronized(bool& syncStatus, const Callback& callback) {
		callback(std::error_code());
	}

	void NodeRpcStub::setRootCert(const std::string &path) {
	}

	void NodeRpcStub::disableVerify() {
	}

}