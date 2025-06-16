#include <boost/json.hpp>
#include <boost/asio.hpp>

class BookingClient {
public:
    BookingClient(const std::string& host, short port) 
        : socket_(io_context_) {
        boost::asio::ip::tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        boost::asio::connect(socket_, endpoints);
    }
    
    json::value send_command(const json::value& command) {
        // Send JSON command
        std::string request = json::serialize(command) + "\n";
        boost::asio::write(socket_, boost::asio::buffer(request));
        
        // Read JSON response
        boost::asio::streambuf buf;
        boost::asio::read_until(socket_, buf, "\n");
        std::istream is(&buf);
        std::string response_str;
        std::getline(is, response_str);
        
        return json::parse(response_str);
    }
    
    // Helper methods
    void list_movies() {
        auto response = send_command({{"command", "LIST_MOVIES"}});
        for (const auto& movie : response.at("movies").as_array()) {
            std::cout << movie.at("id").as_int64() << ": " 
                      << movie.at("name").as_string().c_str() << "\n";
        }
    }
    
    void book_seats(int theater_id, int movie_id, const std::vector<std::string>& seats) {
        json::array seats_json;
        for (const auto& seat : seats) {
            seats_json.push_back(seat);
        }
        
        auto response = send_command({
            {"command", "BOOK"},
            {"theater_id", theater_id},
            {"movie_id", movie_id},
            {"seats", seats_json}
        });
        
        std::cout << "Booking status: " 
                  << response.at("status").as_string().c_str() << "\n";
    }

private:
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::socket socket_;
};


int main() {
    BookingClient client("localhost", 1234);
    
    // List movies
    client.list_movies();
    
    // Book seats
    client.book_seats(5, 10, {"A1", "A2", "B3"});
    
    return 0;
}