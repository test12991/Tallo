// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2016-2018, The Karbowanec developers
// Copyright (c) 2018, The Bittorium developers
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

#pragma once

#include "HttpServer.h"

#include <functional>
#include <unordered_map>

#include <Logging/LoggerRef.h>
#include "Common/Math.h"
#include "CoreRpcServerCommandsDefinitions.h"
#include "JsonRpc.h"

namespace CryptoNote {

class Core;
class NodeServer;
struct ICryptoNoteProtocolHandler;

class RpcServer : public HttpServer {
public:
  RpcServer(System::Dispatcher& dispatcher, Logging::ILogger& log, Core& c, NodeServer& p2p, ICryptoNoteProtocolHandler& protocol);

  typedef std::function<bool(RpcServer*, const HttpRequest& request, HttpResponse& response)> HandlerFunction;
  bool enableCors(const std::vector<std::string>  domains);
  std::vector<std::string> getCorsDomains();

  bool setFeeAddress(const std::string& fee_address, const AccountPublicAddress& fee_acc);
  bool setViewKey(const std::string& view_key);
  bool setCollateralHash(const std::string& collateral_hash);
  bool masternode_check_incoming_tx(const BinaryArray& tx_blob);

  bool on_get_block_headers_range(const COMMAND_RPC_GET_BLOCK_HEADERS_RANGE::request& req, COMMAND_RPC_GET_BLOCK_HEADERS_RANGE::response& res, JsonRpc::JsonRpcError& error_resp);
  bool on_get_alternate_chains(const COMMAND_RPC_GET_ALTERNATE_CHAINS::request& req, COMMAND_RPC_GET_ALTERNATE_CHAINS::response& res);

private:

  template <class Handler>
  struct RpcHandler {
    const Handler handler;
    const bool allowBusyCore;
  };

  typedef void (RpcServer::*HandlerPtr)(const HttpRequest& request, HttpResponse& response);
  static std::unordered_map<std::string, RpcHandler<HandlerFunction>> s_handlers;

  virtual void processRequest(const HttpRequest& request, HttpResponse& response) override;
  bool processJsonRpcRequest(const HttpRequest& request, HttpResponse& response);
  bool isCoreReady();
  bool verifyCollateral();

  // binary handlers
  bool on_get_blocks(const COMMAND_RPC_GET_BLOCKS_FAST::request& req, COMMAND_RPC_GET_BLOCKS_FAST::response& res);
  bool on_query_blocks(const COMMAND_RPC_QUERY_BLOCKS::request& req, COMMAND_RPC_QUERY_BLOCKS::response& res);
  bool on_query_blocks_lite(const COMMAND_RPC_QUERY_BLOCKS_LITE::request& req, COMMAND_RPC_QUERY_BLOCKS_LITE::response& res);
  bool on_get_indexes(const COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES::request& req, COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES::response& res);
  bool on_get_random_outs(const COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::request& req, COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::response& res);
  bool onGetPoolChanges(const COMMAND_RPC_GET_POOL_CHANGES::request& req, COMMAND_RPC_GET_POOL_CHANGES::response& rsp);
  bool onGetPoolChangesLite(const COMMAND_RPC_GET_POOL_CHANGES_LITE::request& req, COMMAND_RPC_GET_POOL_CHANGES_LITE::response& rsp);
  bool onGetBlocksDetailsByHashes(const COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES::request& req, COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES::response& rsp);
  bool onGetBlockDetailsByHeight(const COMMAND_RPC_GET_BLOCK_DETAILS_BY_HEIGHT::request& req, COMMAND_RPC_GET_BLOCK_DETAILS_BY_HEIGHT::response& rsp);
  bool onGetBlocksHashesByTimestamps(const COMMAND_RPC_GET_BLOCKS_HASHES_BY_TIMESTAMPS::request& req, COMMAND_RPC_GET_BLOCKS_HASHES_BY_TIMESTAMPS::response& rsp);
  bool onGetTransactionDetailsByHashes(const COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES::request& req, COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES::response& rsp);
  bool onGetTransactionHashesByPaymentId(const COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID::request& req, COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID::response& rsp);

  // json handlers
  bool on_get_info(const COMMAND_RPC_GET_INFO::request& req, COMMAND_RPC_GET_INFO::response& res);
  bool on_get_height(const COMMAND_RPC_GET_HEIGHT::request& req, COMMAND_RPC_GET_HEIGHT::response& res);
  bool on_get_transactions(const COMMAND_RPC_GET_TRANSACTIONS::request& req, COMMAND_RPC_GET_TRANSACTIONS::response& res);
  bool on_send_raw_tx(const COMMAND_RPC_SEND_RAW_TX::request& req, COMMAND_RPC_SEND_RAW_TX::response& res);
  bool on_stop_daemon(const COMMAND_RPC_STOP_DAEMON::request& req, COMMAND_RPC_STOP_DAEMON::response& res);
  bool on_get_peers(const COMMAND_RPC_GET_PEERS::request& req, COMMAND_RPC_GET_PEERS::response& res);
  bool on_get_peersgray(const COMMAND_RPC_GET_PEERSGRAY::request& req, COMMAND_RPC_GET_PEERSGRAY::response& res);
  bool on_get_issued(const COMMAND_RPC_GET_ISSUED_COINS::request& req, COMMAND_RPC_GET_ISSUED_COINS::response& res);
  bool on_get_total(const COMMAND_RPC_GET_TOTAL_COINS::request& req, COMMAND_RPC_GET_TOTAL_COINS::response& res);
  bool on_get_fee_address(const COMMAND_RPC_GET_FEE_ADDRESS::request& req, COMMAND_RPC_GET_FEE_ADDRESS::response& res);
  bool on_get_transaction_out_amounts_for_account(const COMMAND_RPC_GET_TRANSACTION_OUT_AMOUNTS_FOR_ACCOUNT::request& req, COMMAND_RPC_GET_TRANSACTION_OUT_AMOUNTS_FOR_ACCOUNT::response& res);
  bool on_get_collateral_hash(const COMMAND_RPC_GET_COLLATERAL_HASH::request& req, COMMAND_RPC_GET_COLLATERAL_HASH::response& res);

  // json rpc
  bool on_getblockcount(const COMMAND_RPC_GETBLOCKCOUNT::request& req, COMMAND_RPC_GETBLOCKCOUNT::response& res);
  bool on_getblockhash(const COMMAND_RPC_GETBLOCKHASH::request& req, COMMAND_RPC_GETBLOCKHASH::response& res);
  bool on_getblocktemplate(const COMMAND_RPC_GETBLOCKTEMPLATE::request& req, COMMAND_RPC_GETBLOCKTEMPLATE::response& res);
  bool on_get_currency_id(const COMMAND_RPC_GET_CURRENCY_ID::request& req, COMMAND_RPC_GET_CURRENCY_ID::response& res);
  bool on_submitblock(const COMMAND_RPC_SUBMITBLOCK::request& req, COMMAND_RPC_SUBMITBLOCK::response& res);
  bool on_get_last_block_header(const COMMAND_RPC_GET_LAST_BLOCK_HEADER::request& req, COMMAND_RPC_GET_LAST_BLOCK_HEADER::response& res);
  bool on_get_block_hashes_by_payment_id(const COMMAND_RPC_GET_BLOCK_HASHES_BY_PAYMENT_ID_JSON::request& req, COMMAND_RPC_GET_BLOCK_HASHES_BY_PAYMENT_ID_JSON::response& rsp);
  bool on_get_block_hashes_by_transaction_hashes(const COMMAND_RPC_GET_BLOCK_HASHES_BY_TRANSACTION_HASHES::request& req, COMMAND_RPC_GET_BLOCK_HASHES_BY_TRANSACTION_HASHES::response& rsp);
  bool on_get_block_header_by_hash(const COMMAND_RPC_GET_BLOCK_HEADER_BY_HASH::request& req, COMMAND_RPC_GET_BLOCK_HEADER_BY_HASH::response& res);
  bool on_get_block_header_by_height(const COMMAND_RPC_GET_BLOCK_HEADER_BY_HEIGHT::request& req, COMMAND_RPC_GET_BLOCK_HEADER_BY_HEIGHT::response& res);
  bool on_get_block_indexes_by_transaction_hashes(const COMMAND_RPC_GET_BLOCK_INDEXES_BY_TRANSACTION_HASHES::request& req, COMMAND_RPC_GET_BLOCK_INDEXES_BY_TRANSACTION_HASHES::response& rsp);
  bool on_get_blocks_details_by_hashes(const COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES_JSON::request& req, COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES_JSON::response& rsp);
  bool on_get_transaction_details_by_hashes(const COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES_JSON::request& req, COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES_JSON::response& rsp);
  bool on_get_transaction_hashes_by_payment_id(const COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID_JSON::request& req, COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID_JSON::response& rsp);

  void fill_block_header_response(const BlockTemplate& blk, bool orphan_status, uint32_t index, const Crypto::Hash& hash, block_header_response& responce);
  RawBlockLegacy prepareRawBlockLegacy(BinaryArray&& blockBlob);

  bool f_on_blocks_list_json(const F_COMMAND_RPC_GET_BLOCKS_LIST::request& req, F_COMMAND_RPC_GET_BLOCKS_LIST::response& res);
  bool f_on_block_json(const F_COMMAND_RPC_GET_BLOCK_DETAILS::request& req, F_COMMAND_RPC_GET_BLOCK_DETAILS::response& res);
  bool f_on_transaction_json(const F_COMMAND_RPC_GET_TRANSACTION_DETAILS::request& req, F_COMMAND_RPC_GET_TRANSACTION_DETAILS::response& res);
  bool f_on_pool_transaction_json(const F_COMMAND_RPC_GET_TRANSACTION_DETAILS::request& req, F_COMMAND_RPC_GET_TRANSACTION_DETAILS::response& res);
  bool f_on_transactions_pool_json(const F_COMMAND_RPC_GET_POOL::request& req, F_COMMAND_RPC_GET_POOL::response& res);
  bool f_getMixin(const Transaction& transaction, uint64_t& mixin);

  bool populateTransactionDetails(const Crypto::Hash& Hash, F_COMMAND_RPC_GET_TRANSACTION_DETAILS::response& res);

  Logging::LoggerRef logger;
  Core& m_core;
  NodeServer& m_p2p;
  ICryptoNoteProtocolHandler& m_protocol;
  std::vector<std::string> m_cors_domains;
  std::string m_fee_address;
  Crypto::SecretKey m_view_key = NULL_SECRET_KEY;
  Crypto::Hash m_collateral_hash = NULL_HASH;
  AccountPublicAddress m_fee_acc;
};

}
