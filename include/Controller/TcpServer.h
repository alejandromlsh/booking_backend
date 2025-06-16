#pragma once
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <thread>
#include <vector>
#include <memory>
#include "Models/BookingService.h"

namespace json = boost::json;


class TcpServer {
public:
  TcpServer(boost::asio::io_context & io_context, unsigned short port, BookingService & booking_service, std::size_t thread_pool_size = 4);
  void start();
private:
  void do_accept();
  void handle_session(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
  std::string process_request(const std::string & request);
  std::string process_request_json(const std::string& request);
  json::value get_sample_format();

  boost::asio::ip::tcp::acceptor acceptor_;
  BookingService & booking_service_;
  std::size_t threadpool_size_;
};