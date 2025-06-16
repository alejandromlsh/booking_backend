#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include <boost/json.hpp>

#include "Controller/TcpServer.h"
#include "Models/BookingService.h"
#include "Models/Theater.h"
#include "Models/Movie.h"

using boost::asio::ip::tcp;
namespace json = boost::json;

class TcpServerIntegrationTest : public ::testing::Test {
protected:
    static constexpr unsigned short kPort = 12345; // Port from main.cpp
    boost::asio::io_context io_context_;
    std::unique_ptr<std::thread> server_thread_;
    std::unique_ptr<TcpServer> server_;
    BookingService booking_service_;

    void SetUp() override {
        // Setup movies and theaters as in main.cpp
        Movie m1(1, "Inception");
        Movie m2(2, "The Matrix");
        booking_service_.add_movie(m1);
        booking_service_.add_movie(m2);

        auto t1 = std::make_shared<Theater>(1, "Cinema One");
        auto t2 = std::make_shared<Theater>(2, "Cinema Two");
        t1->add_movie(m1);
        t1->add_movie(m2);
        t2->add_movie(m2);

        booking_service_.add_theater(t1);
        booking_service_.add_theater(t2);

        // Start server in background thread
        server_ = std::make_unique<TcpServer>(io_context_, kPort, booking_service_, 2);
        server_thread_ = std::make_unique<std::thread>([this]() {
            server_->start();
            io_context_.run();
        });

        // Give server time to start
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    void TearDown() override {
        io_context_.stop();
        if (server_thread_ && server_thread_->joinable())
            server_thread_->join();
    }

    // Helper: Connect, send, receive (plain text)
    std::string send_and_receive(const std::string& msg) {
        boost::asio::io_context ctx;
        tcp::socket socket(ctx);
        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), kPort));
        boost::asio::write(socket, boost::asio::buffer(msg + "\n"));
        boost::asio::streambuf buf;
        boost::asio::read_until(socket, buf, "\n");
        std::istream is(&buf);
        std::string line;
        std::getline(is, line);
        return line;
    }

    // Helper: Connect, send, receive (JSON)
    json::value send_and_receive_json(const json::value& req) {
        boost::asio::io_context ctx;
        tcp::socket socket(ctx);
        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), kPort));
        std::string msg = json::serialize(req) + "\n";
        boost::asio::write(socket, boost::asio::buffer(msg));
        boost::asio::streambuf buf;
        boost::asio::read_until(socket, buf, "\n");
        std::istream is(&buf);
        std::string line;
        std::getline(is, line);
        return json::parse(line);
    }
};

// ---- Plain Text Protocol Tests ----

TEST_F(TcpServerIntegrationTest, ListMoviesPlainText) {
    std::string response = send_and_receive("LIST_MOVIES");
    EXPECT_NE(response.find("Inception"), std::string::npos);
    EXPECT_NE(response.find("The Matrix"), std::string::npos);
}

TEST_F(TcpServerIntegrationTest, ListTheatersPlainText) {
    std::string response = send_and_receive("LIST_THEATERS 2");
    EXPECT_NE(response.find("Cinema Two"), std::string::npos);
}

TEST_F(TcpServerIntegrationTest, ListSeatsPlainText) {
    std::string response = send_and_receive("LIST_SEATS 1 1");
    EXPECT_NE(response.find("a1"), std::string::npos);
    EXPECT_NE(response.find("a20"), std::string::npos);
}

TEST_F(TcpServerIntegrationTest, BookSeatsPlainText_Success) {
    std::string response = send_and_receive("BOOK 1 1 a1 a2");
    EXPECT_EQ(response, "BOOKED");
}

TEST_F(TcpServerIntegrationTest, BookSeatsPlainText_FailOnDoubleBook) {
    send_and_receive("BOOK 1 1 a3");
    std::string response = send_and_receive("BOOK 1 1 a3");
    EXPECT_EQ(response, "FAILED");
}

TEST_F(TcpServerIntegrationTest, UnknownCommand) {
    std::string response = send_and_receive("FOO_BAR");
    EXPECT_EQ(response, "UNKNOWN_COMMAND");
}

// ---- JSON Protocol Tests ----

TEST_F(TcpServerIntegrationTest, ListMoviesJSON) {
    json::value req = {{"command", "LIST_MOVIES"}};
    json::value resp = send_and_receive_json(req);
    ASSERT_TRUE(resp.as_object().contains("movies"));
    auto& arr = resp.at("movies").as_array();
    EXPECT_EQ(arr[0].as_object().at("name").as_string(), "Inception");
    EXPECT_EQ(arr[1].as_object().at("name").as_string(), "The Matrix");
}

TEST_F(TcpServerIntegrationTest, ListTheatersJSON) {
    json::value req = {{"command", "LIST_THEATERS"}, {"movie_id", 2}};
    json::value resp = send_and_receive_json(req);
    ASSERT_TRUE(resp.as_object().contains("theaters"));
    auto& arr = resp.at("theaters").as_array();
    EXPECT_EQ(arr[0].as_object().at("name").as_string(), "Cinema One");
    EXPECT_EQ(arr[1].as_object().at("name").as_string(), "Cinema Two");
}

TEST_F(TcpServerIntegrationTest, ListSeatsJSON) {
    json::value req = {{"command", "LIST_SEATS"}, {"theater_id", 1}, {"movie_id", 1}};
    json::value resp = send_and_receive_json(req);
    ASSERT_TRUE(resp.as_object().contains("available_seats"));
    EXPECT_EQ(resp.at("available_seats").as_array().size(), 20);
}

TEST_F(TcpServerIntegrationTest, BookSeatsJSON_Success) {
    json::array seats = {"a5", "a6"};
    json::value req = {{"command", "BOOK"}, {"theater_id", 1}, {"movie_id", 1}, {"seats", seats}};
    json::value resp = send_and_receive_json(req);
    EXPECT_EQ(resp.at("status").as_string(), "BOOKED");
}

TEST_F(TcpServerIntegrationTest, BookSeatsJSON_Failure) {
    json::array seats = {"a7"};
    json::value req1 = {{"command", "BOOK"}, {"theater_id", 1}, {"movie_id", 1}, {"seats", seats}};
    send_and_receive_json(req1); // Book first time
    json::value resp = send_and_receive_json(req1); // Try again
    EXPECT_EQ(resp.at("status").as_string(), "FAILED");
}
