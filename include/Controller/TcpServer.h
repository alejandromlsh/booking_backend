/**
 * @file TcpServer.h
 * @brief TCP server handling both booking and administration requests
 * @details This class implements a multi-threaded TCP server that handles client connections,
 *          processes JSON and plain text requests related to movie booking and administration,
 *          and manages a thread pool for concurrent request handling. The server supports
 *          both BookingService operations (seat booking, availability queries) and
 *          AdministrationService operations (movie/theater management).
 * @author Alejandro Martinez Lopez
 * @date 2025
 */

#pragma once

#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <memory>
#include <string>

#include "Models/BookingService.h"
#include "Models/AdministrationService.h"
#include "Utils/ThreadPool.h"

namespace json = boost::json;

/**
 * @class TcpServer
 * @brief Multi-threaded TCP server for movie booking system
 * @details Handles client connections using Boost.Asio, processes both JSON and plain text
 *          requests, and delegates business logic to BookingService and AdministrationService.
 *          Uses a thread pool for concurrent client session handling to ensure scalability
 *          and responsiveness under high load conditions.
 */
class TcpServer {
public:
  /**
   * @brief Construct TCP server with booking and administration services
   * @details Initializes the TCP server with the specified configuration, binds to the given port,
   *          and sets up the thread pool for handling concurrent client connections. The server
   *          requires both booking and administration services for complete functionality.
   * @param io_context Boost.Asio IO context for asynchronous operations
   * @param port TCP port number to listen on (typically 12345)
   * @param booking_service Reference to BookingService instance for seat booking operations
   * @param admin_service Reference to AdministrationService instance for system administration
   * @param thread_pool_size Number of worker threads in the thread pool for request handling
   * @throws std::runtime_error if port binding fails or socket configuration errors occur
   */
  TcpServer(boost::asio::io_context& io_context, unsigned short port,
            IBookingService& booking_service, IAdministrationService& admin_service,
            std::size_t thread_pool_size);

  /**
   * @brief Start accepting client connections
   * @details Begins the asynchronous accept loop to handle incoming client connections.
   *          This method is non-blocking and returns immediately after starting the
   *          accept operation. The actual connection handling occurs asynchronously.
   */
  void start();

private:
  /**
   * @brief Begin asynchronous accept operation for new client connections
   * @details Initiates an asynchronous accept operation and sets up the completion handler
   *          for when a new client connects. Upon successful connection, delegates the
   *          client session to the thread pool and immediately starts accepting the next connection.
   */
  void do_accept();

  /**
   * @brief Handle a complete client session
   * @details Manages the entire lifecycle of a client connection, including reading requests,
   *          processing them through the appropriate service, and sending responses. Handles
   *          both JSON and plain text protocols. Continues processing requests until the
   *          client disconnects or an error occurs.
   * @param socket Shared pointer to the client's TCP socket connection
   */
  void handle_session(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

  /**
   * @brief Process a plain text request from client
   * @details Parses and processes plain text commands such as "LIST_MOVIES", "BOOK", etc.
   *          Extracts command parameters and delegates to the appropriate service method.
   *          Returns plain text responses compatible with simple client implementations.
   * @param request Plain text request string from client
   * @return Plain text response string to send back to client
   */
  std::string process_request(const std::string& request);

  /**
   * @brief Process a JSON request from client
   * @details Parses JSON requests, validates command structure, and processes commands
   *          through BookingService or AdministrationService as appropriate. Handles
   *          all supported commands: LIST_MOVIES, LIST_THEATERS, LIST_SEATS, and BOOK.
   *          Provides comprehensive error handling for malformed JSON and invalid requests.
   * @param request JSON request string from client
   * @return JSON response string with results or error information
   * @throws std::exception for JSON parsing errors (caught and converted to error response)
   */
  std::string process_request_json(const std::string& request);

  /**
   * @brief Generate sample JSON request formats for error responses
   * @details Creates a JSON object containing example request formats for all supported
   *          commands. Used in error responses to help clients understand the expected
   *          request structure when they send malformed or invalid requests.
   * @return JSON value containing sample request formats for all commands
   */
  json::value get_sample_format();

  boost::asio::ip::tcp::acceptor acceptor_;  ///< TCP acceptor for incoming connections
  IBookingService& booking_service_;          ///< Reference to booking service for seat operations
  IAdministrationService& admin_service_;     ///< Reference to administration service for system management
  std::size_t threadpool_size_;              ///< Number of threads in the worker thread pool
  ThreadPool thread_pool_;                   ///< Thread pool for concurrent client session handling
};
