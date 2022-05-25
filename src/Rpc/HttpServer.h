// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2016-2020, The Karbo developers
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

#pragma once

#include <unordered_set>
#include <string.h>

#include <HTTP/HttpRequest.h>
#include <HTTP/HttpResponse.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/thread/thread.hpp>

#include <System/ContextGroup.h>
#include <System/Dispatcher.h>
#include <System/Ipv4Address.h>
#include <System/TcpListener.h>
#include <System/TcpConnection.h>
#include <System/Event.h>

#include <Logging/LoggerRef.h>


namespace CryptoNote {

class HttpServer {

public:
  HttpServer(System::Dispatcher& dispatcher, Logging::ILogger& log);
  void setCerts(const std::string& chain_file, const std::string& key_file, const std::string& dh_file);
  void start(const std::string& address, uint16_t port, uint16_t port_ssl = 0,
             bool server_ssl_enable = false, uint16_t external_port = 0, uint16_t external_port_ssl = 0);
  void stop();
  virtual void processRequest(const HttpRequest& request, HttpResponse& response) = 0;
  virtual size_t get_connections_count() const;

protected:
  System::Dispatcher& m_dispatcher;

private:
  System::Ipv4Address m_server_ip;
  bool m_server_ssl_do;
  bool m_server_ssl_is_run;
  uint16_t m_port;
  uint16_t m_external_port;
  uint16_t m_external_port_ssl;
  uint16_t m_server_ssl_port;
  unsigned int m_server_ssl_clients;
  std::string m_address;
  std::string m_chain_file;
  std::string m_dh_file;
  std::string m_key_file;
  std::unordered_set<System::TcpConnection*> m_connections;
  boost::thread m_ssl_server_thread;
  System::ContextGroup workingContextGroup;
  System::TcpListener m_listener;
  Logging::LoggerRef logger;
  void acceptLoop();
  void connectionHandler(System::TcpConnection&& conn);
  void sslServerUnitControl(boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> &stream,
                            boost::system::error_code &ec,
                            bool &unit_do,
                            bool &unit_control_do,
                            size_t &stream_timeout_n);
  void sslServerUnit(boost::asio::ip::tcp::socket &socket, boost::asio::ssl::context &ctx);
  void sslServerControl(boost::asio::ip::tcp::acceptor &accept);
  void sslServer();

};

}
