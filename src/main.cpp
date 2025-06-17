#include <iostream>
#include <memory>
#include <thread>
#include "Controller/TcpServer.h"
#include "Interfaces/IDataStore.h"
#include "Interfaces/IBookingService.h"
#include "Interfaces/IAdministrationService.h"
#include "Models/CentralDataStore.h"
#include "Models/BookingService.h"
#include "Models/AdministrationService.h"
#include "Models/Movie.h"
#include "Models/Theater.h"

int main() {
  try {
    // Create concrete implementations through interfaces
    std::shared_ptr<IDataStore> data_store = std::make_shared<CentralDataStore>();
    std::unique_ptr<IBookingService> booking_service = std::make_unique<BookingService>(data_store);
    std::unique_ptr<IAdministrationService> admin_service = std::make_unique<AdministrationService>(data_store);

    std::cout << "Initializing system..." << std::endl;

    // Setup sample movies
    Movie m1(1, "Inception");
    Movie m2(2, "The Matrix");
    admin_service->add_movie(std::move(m1));
    admin_service->add_movie(std::move(m2));

    // Setup sample theaters and schedule movies
    std::shared_ptr<ITheater> t1 = std::make_shared<Theater>(1, "Cinema One");  // Cast to interface
    std::shared_ptr<ITheater> t2 = std::make_shared<Theater>(2, "Cinema Two");  // Cast to interface
    
    admin_service->add_theater(t1);
    admin_service->add_theater(t2);
    admin_service->schedule_movie_in_theater(1, Movie(1,"Inception"));
    admin_service->schedule_movie_in_theater(1, Movie(2,"The Matrix"));
    admin_service->schedule_movie_in_theater(2, Movie(2,"The Matrix"));

    std::cout << "System initialized with " << booking_service->get_all_movies().size()
              << " movies and " << admin_service->get_all_theaters().size() << " theaters." << std::endl;

    const unsigned short port = 12345;
    const std::size_t thread_pool_size = std::thread::hardware_concurrency();

    std::cout << "Creating TCP server..." << std::endl;
    boost::asio::io_context io_context;
    TcpServer server(io_context, port, *booking_service, *admin_service, thread_pool_size);

    std::cout << "Starting server..." << std::endl;
    server.start();
    std::cout << "Server running. Use 'netstat -tlnp | grep " << port << "' to verify" << std::endl;

    io_context.run();
  } catch (const std::exception& e) {
    std::cerr << "Server error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
