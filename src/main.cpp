#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include "Controller/TcpServer.h"
#include "Models/BookingService.h"
#include "Models/Theater.h"
#include "Models/Movie.h"

int main() {
  std::cout << "start\n";
  try {
    // Create the service instance (no singleton)
    std::cout << "1\n";

    BookingService service;
    Movie m1(1, "Inception");
    Movie m2(2, "The Matrix");
    service.add_movie(m1);
    service.add_movie(m2);
    std::cout << "2\n";

    auto t1 = std::make_shared<Theater>(1, "Cinema One");
    auto t2 = std::make_shared<Theater>(2, "Cinema Two");
    std::cout << "2.5\n";

    t1->add_movie(m1);
    std::cout << "2.5\n";

    t1->add_movie(m2);
    t2->add_movie(m2);

    std::cout << "3\n";

    service.add_theater(t1);
    service.add_theater(t2);

    boost::asio::io_context io_context;
    //std::size_t thread_pool_size = std::thread::hardware_concurrency();
    std::cout << "Creating TCP server..." << std::endl;
    TcpServer server(io_context, 12345, service, std::thread::hardware_concurrency());

    std::cout << "Starting server..." << std::endl;
    server.start();

    std::cout << "Server running. Use 'netstat -tlnp | grep 12345' to verify" << std::endl;
    io_context.run();

  } catch (std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}