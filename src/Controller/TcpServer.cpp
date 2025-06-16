#include "Controller/TcpServer.h"
#include <iostream>
#include <sstream>
#include <boost/json.hpp>

namespace json = boost::json;

enum class CommandType {
    ListMovies,
    ListTheaters,
    ListSeats,
    Book,
    Unknown
};

CommandType parse_command(const std::string& cmd) {
    if (cmd == "LIST_MOVIES") return CommandType::ListMovies;
    if (cmd == "LIST_THEATERS") return CommandType::ListTheaters;
    if (cmd == "LIST_SEATS") return CommandType::ListSeats;
    if (cmd == "BOOK") return CommandType::Book;
    return CommandType::Unknown;
}

TcpServer::TcpServer(boost::asio::io_context & io_context,unsigned short port,
    BookingService & booking_service, std::size_t thread_pool_size) : //acceptor_(io_context,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port)),
    acceptor_(io_context),booking_service_(booking_service),threadpool_size_(thread_pool_size) {
    
    using namespace boost::asio;
    boost::system::error_code ec;

    // Open the acceptor
    acceptor_.open(ip::tcp::v4(), ec);
    if (ec) {
        throw std::runtime_error("Open error: " + ec.message());
    }

    // Set reuse address option
    acceptor_.set_option(ip::tcp::acceptor::reuse_address(true), ec);
    if (ec) {
        // Non-fatal error, log but continue
        std::cerr << "Set option warning: " << ec.message() << std::endl;
    }

    // Bind to port
    acceptor_.bind(ip::tcp::endpoint(ip::tcp::v4(), port), ec);
    if (ec) {
        throw std::runtime_error("Bind error: " + ec.message());
    }

    // Start listening
    acceptor_.listen(socket_base::max_listen_connections, ec);
    if (ec) {
        throw std::runtime_error("Listen error: " + ec.message());
    }

    std::cout << "Server bound to port " << port << std::endl;

    }

void TcpServer::start() {
  do_accept();
}

void TcpServer::do_accept() {

  auto socket = std::make_shared<boost::asio::ip::tcp::socket>(acceptor_.get_executor());

  acceptor_.async_accept(*socket,[this,socket](boost::system::error_code ec) {
    if (!ec) {
      std::thread([this,socket](){handle_session(socket);}).detach();
    }
    do_accept();
  });
}
// Synchronous
void TcpServer::handle_session(std::shared_ptr<boost::asio::ip::tcp::socket>socket) {
  try {
    boost::asio::streambuf buf;

    while (true) {
      boost::system::error_code ec;
      std::size_t n = boost::asio::read_until(*socket,buf,"\n",ec);
      if (ec) {
        if (ec == boost::asio::error::eof) {
          std::cout << "Client disconected:" << std::endl;
          break;
        } else {
          std::cerr << "Read error: " << ec.message() << std::endl;
          break;
        }
      }
      std::istream is(&buf);
      std::string request;
      std::getline(is, request);

      std::string response;
      try {
        response = process_request_json(request);
      } catch (const std::exception &e) {
        response = std::string("{\"error\":\"") + e.what() + "\"}";
      }
      boost::asio::write(*socket, boost::asio::buffer(response + "\n"));
    }

  } catch (const std::exception& e) {
    std::cerr << "Session error: " << e.what() << std::endl;
  }
  // try {
  //   boost::asio::streambuf buf;
  //   boost::asio::read_until(*socket,buf,"\n");
  //   std::istream is(&buf);
  //   std::string request;
  //   std::getline(is,request);

  //   std::string response = process_request_json(request);
  //   boost::asio::write(*socket,boost::asio::buffer(response));

  // } catch (std::exception & e) {
  //   // to be developed
  // }
}

std::string TcpServer::process_request(const std::string&request) {
  std::istringstream iss(request);
  std::string command_str;
  iss>> command_str;
  CommandType command = parse_command(command_str);

  switch (command) {
    case (CommandType::ListMovies) : {
      auto movies = booking_service_.get_all_movies();
      std::ostringstream oss;
      for (const auto & m : movies) {
        oss << m.get_id() << m.get_name() << "\n";
      }
    }
    case CommandType::ListTheaters: {
      int movieId;
      iss >> movieId;
      auto theaters = booking_service_.get_theaters_showing_movie(movieId);
      std::ostringstream oss;
      for (const auto& t : theaters) {
          oss << t->get_id() << ": " << t->get_name() << "\n";
      }
      return oss.str();
    }
    case CommandType::ListSeats: {
      int theaterId, movieId;
      iss >> theaterId >> movieId;
      auto seats = booking_service_.get_available_seats(theaterId, movieId);
      std::ostringstream oss;
      for (const auto& s : seats) {
          oss << s << " ";
      }
      oss << "\n";
      return oss.str();
    }
    case CommandType::Book: {
      int theaterId, movieId;
      iss >> theaterId >> movieId;
      std::vector<std::string> seatIds;
      std::string seat;
      while (iss >> seat) seatIds.push_back(seat);
      bool success = booking_service_.book_seats(theaterId, movieId, seatIds);
      return success ? "BOOKED\n" : "FAILED\n";
    }
    case CommandType::Unknown:
    default:
        return "UNKNOWN_COMMAND\n";
    }
}




std::string TcpServer::process_request_json(const std::string& request) {
    try {
        // Parse JSON request
        json::value request_json = json::parse(request); // Parse the input hson into a BOOST JSON VALUE
        
        const std::string command = json::value_to<std::string>(request_json.at("command")); // Extract the command field as a string
        json::value response_json;                               // prepare a variable for the response
        
        switch (parse_command(command)) {                   // string to enum
            case CommandType::ListMovies: {
                auto movies = booking_service_.get_all_movies();   // Get all movies from the booking service
                json::array movies_array;                          // json array for movies
                
                for (const auto& m : movies) {
                    movies_array.push_back(json::object{
                        {"id", m.get_id()},
                        {"name", m.get_name()}//,
                        //{"duration", m.get_duration()}, 
                        //{"rating", m.get_rating()}
                    });
                }
                response_json = json::object{{"movies", movies_array}}; // Set the response
                break;
            }
            
            case CommandType::ListTheaters: {
                int movie_id = request_json.at("movie_id").as_int64();  // Get movie if from the request
                auto theaters = booking_service_.get_theaters_showing_movie(movie_id); // Get theaters showing movie from the service
                
                json::array theaters_array;
                for (const auto& t : theaters) {
                    theaters_array.push_back(json::object{
                        {"id", t->get_id()},
                        {"name", t->get_name()}//,
                        // {"location", t->getLocation()},
                        // {"capacity", t->getCapacity()}
                    });
                }
                response_json = json::object{{"theaters", theaters_array}}; // set the response to contain theaters array
                break;
            }
            
            case CommandType::ListSeats: {
                int theater_id = request_json.at("theater_id").as_int64();
                int movie_id = request_json.at("movie_id").as_int64();
                auto seats = booking_service_.get_available_seats(theater_id, movie_id);
                
                json::array seats_array;
                for (const auto& s : seats) {
                    seats_array.push_back(json::value(s));           /////////// NOTA
                }
                response_json = json::object{
                    {"theater_id", theater_id},
                    {"movie_id", movie_id},
                    {"available_seats", seats_array},
                    {"total_available", seats.size()}
                };
                break;
            }
            
            case CommandType::Book: {
                int theater_id = request_json.at("theater_id").as_int64();
                int movie_id = request_json.at("movie_id").as_int64();
                
                json::array seats_json = request_json.at("seats").as_array();
                std::vector<std::string> seats;
                for (const auto& seat : seats_json) {
                    seats.push_back(json::value_to<std::string>(seat)); // convert json value to string and  push to vector
                }
                
                bool success = booking_service_.book_seats(theater_id, movie_id, seats);
                
                response_json = json::object{
                    {"status", success ? "BOOKED" : "FAILED"},
                    {"theater_id", theater_id},
                    {"movie_id", movie_id},
                    {"seats", seats_json},
                    {"timestamp", std::time(nullptr)}
                    //,{"confirmation", generate_confirmation_code()} // Optional
                };
                break;
            }
            
            default: {
                response_json = json::object{
                    {"error", "UNKNOWN_COMMAND"},
                    {"received_command", command},
                    {"valid_commands", json::array{"LIST_MOVIES", "LIST_THEATERS", "LIST_SEATS", "BOOK"}}
                };
                break;
            }
        }
        
        return json::serialize(response_json) + "\n";
        
    } catch (const std::exception& e) {
        // Handle JSON parsing errors or missing fields
        return json::serialize(json::object{
            {"error", "INVALID_REQUEST"},
            {"message", e.what()},
            {"sample_format", get_sample_format()} // Helper function shown below
        }) + "\n";
    }
}

// Helper function for error messages
json::value TcpServer::get_sample_format() {
    return json::object{
        {"LIST_MOVIES", json::object{{"command", "LIST_MOVIES"}}},
        {"LIST_THEATERS", json::object{
            {"command", "LIST_THEATERS"},
            {"movie_id", 123}
        }},
        {"LIST_SEATS", json::object{
            {"command", "LIST_SEATS"},
            {"theater_id", 456},
            {"movie_id", 789}
        }},
        {"BOOK", json::object{
            {"command", "BOOK"},
            {"theater_id", 456},
            {"movie_id", 789},
            {"seats", json::array{{"A1", "A2", "B3"}}
        }}}
    };
}




