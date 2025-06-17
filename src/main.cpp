#include <iostream>
#include <memory>
#include <boost/asio.hpp>

#include "Controller/TcpServer.h"
#include "Models/CentralDataStore.h"
#include "Models/BookingService.h"
#include "Models/AdministrationService.h"
#include "Models/Movie.h"
#include "Models/Theater.h"

int main() {
  try {
    
    auto data_store = std::make_shared<CentralDataStore>();
    BookingService booking_service(data_store);
    AdministrationService admin_service(data_store);
    
    std::cout << "Initializing system..." << std::endl;
    
    // Setup sample movies
    Movie m1(1, "Inception");
    Movie m2(2, "The Matrix");
    admin_service.add_movie(m1);
    admin_service.add_movie(m2);
    
    // Setup sample theaters and schedule movies
    auto t1 = std::make_shared<Theater>(1, "Cinema One");
    auto t2 = std::make_shared<Theater>(2, "Cinema Two");
    
    admin_service.add_theater(t1);
    admin_service.add_theater(t2);
    admin_service.schedule_movie_in_theater(1, m1);
    admin_service.schedule_movie_in_theater(1, m2);
    admin_service.schedule_movie_in_theater(2, m2);
    
    std::cout << "System initialized with " << booking_service.get_all_movies().size() 
              << " movies and " << admin_service.get_all_theaters().size() << " theaters." << std::endl;
    
    // Configuration
    const unsigned short port = 12345;
    const std::size_t thread_pool_size = std::thread::hardware_concurrency();  // Configurable thread pool size
    
    std::cout << "Creating TCP server..." << std::endl;
    boost::asio::io_context io_context;
    TcpServer server(io_context, port, booking_service, admin_service, thread_pool_size);
    
    // Note: "Server bound to port X" is printed by TcpServer constructor
    
    std::cout << "Starting server..." << std::endl;
    server.start();
    
    std::cout << "Server running. Use 'netstat -tlnp | grep " << port << "' to verify" << std::endl;
    
    // Start event loop (blocking call)
    io_context.run();
    
  } catch (const std::exception& e) {
    std::cerr << "Server error: " << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
