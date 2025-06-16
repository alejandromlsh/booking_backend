#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/json.hpp>

namespace json = boost::json;
using boost::asio::ip::tcp;

void print_menu() {
  std::cout << "\nAvailable commands:\n"
            << "1. List all movies\n"
            << "2. List theaters for a movie\n"
            << "3. List seats for a theater and movie\n"
            << "4. Book seats\n"
            << "5. Quit\n"
            << "Enter choice: ";
}

// Helper to send and receive a JSON message
std::string send_json_request(tcp::socket& socket, const json::value& request) {
  std::string req_str = json::serialize(request) + "\n";
  boost::asio::write(socket, boost::asio::buffer(req_str));

  boost::asio::streambuf buf;
  boost::asio::read_until(socket, buf, "\n");
  std::istream is(&buf);
  std::string response;
  std::getline(is, response);
  return response;
}

int main(int argc, char* argv[]) {
  std::string server_ip = "127.0.0.1";
  int port = 12345;
  if (argc > 1) server_ip = argv[1];
  if (argc > 2) port = std::stoi(argv[2]);

  try {
      boost::asio::io_context io_context;
      tcp::socket socket(io_context);
      socket.connect(tcp::endpoint(boost::asio::ip::make_address(server_ip), port));
      std::cout << "Connected to server at " << server_ip << ":" << port << "\n";

      while (true) {
        print_menu();
        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Clear newline

        json::object request;

        if (choice == 1) {
            request["command"] = "LIST_MOVIES";
        } else if (choice == 2) {
            int movie_id;
            std::cout << "Enter movie id: ";
            std::cin >> movie_id;
            std::cin.ignore();
            request["command"] = "LIST_THEATERS";
            request["movie_id"] = movie_id;
        } else if (choice == 3) {
            int theater_id, movie_id;
            std::cout << "Enter theater id: ";
            std::cin >> theater_id;
            std::cout << "Enter movie id: ";
            std::cin >> movie_id;
            std::cin.ignore();
            request["command"] = "LIST_SEATS";
            request["theater_id"] = theater_id;
            request["movie_id"] = movie_id;
        } else if (choice == 4) {
            int theater_id, movie_id, n;
            std::cout << "Enter theater id: ";
            std::cin >> theater_id;
            std::cout << "Enter movie id: ";
            std::cin >> movie_id;
            std::cout << "How many seats to book? ";
            std::cin >> n;
            std::cin.ignore();
            json::array seats;
            for (int i = 0; i < n; ++i) {
                std::string seat_id;
                std::cout << "Enter seat id #" << (i + 1) << ": ";
                std::getline(std::cin, seat_id);
                seats.push_back(boost::json::value(seat_id));
            }
            request["command"] = "BOOK";
            request["theater_id"] = theater_id;
            request["movie_id"] = movie_id;
            request["seats"] = seats;
        } else if (choice == 5) {
            std::cout << "Goodbye!\n";
            break;
        } else {
            std::cout << "Invalid choice!\n";
            continue;
        }

        std::string response = send_json_request(socket, request);
        std::cout << "\nServer response:\n" << response << std::endl;
    }

    socket.close();
  } catch (const std::exception& e) {
    std::cerr << "Client error: " << e.what() << std::endl;
  }

  return 0;
}
