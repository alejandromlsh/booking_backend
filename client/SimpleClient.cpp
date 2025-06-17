#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <limits>

namespace json = boost::json;
using boost::asio::ip::tcp;

/**
 * @brief Display the main menu options
 */
void print_menu() {
  std::cout << "\nAvailable commands:\n"
            << "1. List all movies\n"
            << "2. List theaters for a movie\n"
            << "3. List seats for a theater and movie\n"
            << "4. Book seats\n"
            << "5. Quit\n"
            << "Enter choice: ";
}

/**
 * @brief Safely read an integer with validation and error handling
 * @param prompt Message to display to user
 * @return Valid integer input from user
 */
int get_valid_integer(const std::string& prompt) {
  int value;
  while (true) {
    std::cout << prompt;
    std::cin >> value;
    
    if (std::cin.fail()) {
      // Clear error state and ignore invalid input
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Invalid input! Please enter a valid number.\n";
    } else {
      // Clear any remaining characters in the input buffer
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      return value;
    }
  }
}

/**
 * @brief Safely read menu choice with range validation
 * @return Valid menu choice (1-5)
 */
int get_menu_choice() {
  int choice;
  while (true) {
    choice = get_valid_integer("");
    if (choice >= 1 && choice <= 5) {
      return choice;
    }
    std::cout << "Invalid choice! Please enter a number between 1 and 5.\n";
    print_menu();
  }
}

/**
 * @brief Send JSON request and receive response
 * @param socket TCP socket for communication
 * @param request JSON request object
 * @return Server response as string
 */
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
      int choice = get_menu_choice();

      json::object request;

      if (choice == 1) {
        request["command"] = "LIST_MOVIES";
        
      } else if (choice == 2) {
        int movie_id = get_valid_integer("Enter movie id: ");
        request["command"] = "LIST_THEATERS";
        request["movie_id"] = movie_id;
        
      } else if (choice == 3) {
        int theater_id = get_valid_integer("Enter theater id: ");
        int movie_id = get_valid_integer("Enter movie id: ");
        request["command"] = "LIST_SEATS";
        request["theater_id"] = theater_id;
        request["movie_id"] = movie_id;
        
      } else if (choice == 4) {
        int theater_id = get_valid_integer("Enter theater id: ");
        int movie_id = get_valid_integer("Enter movie id: ");
        int n = get_valid_integer("How many seats to book? ");
        
        // Validate seat count
        if (n <= 0) {
          std::cout << "Invalid number of seats! Must be greater than 0.\n";
          continue;
        }
        
        json::array seats;
        std::cout << "Enter seat IDs (format: a1, b2, etc.):\n";
        for (int i = 0; i < n; ++i) {
          std::string seat_id;
          std::cout << "Enter seat id #" << (i + 1) << ": ";
          std::getline(std::cin, seat_id);
          
          // Validate seat ID is not empty
          if (seat_id.empty()) {
            std::cout << "Seat ID cannot be empty! Please try again.\n";
            --i; // Retry this seat
            continue;
          }
          
          seats.push_back(boost::json::value(seat_id));
        }
        
        request["command"] = "BOOK";
        request["theater_id"] = theater_id;
        request["movie_id"] = movie_id;
        request["seats"] = seats;
        
      } else if (choice == 5) {
        std::cout << "Goodbye!\n";
        break;
      }

      try {
        std::string response = send_json_request(socket, request);
        std::cout << "\nServer response:\n" << response << std::endl;
      } catch (const std::exception& e) {
        std::cout << "Communication error: " << e.what() << std::endl;
        std::cout << "Please try again.\n";
      }
    }

    socket.close();
    
  } catch (const std::exception& e) {
    std::cerr << "Client error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
