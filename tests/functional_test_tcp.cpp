/**
 * @file functional_test_tcp.cpp
 * @brief Comprehensive functional tests for the TCP Server and Movie Booking System
 * @details This file contains integration and functional tests that validate the complete
 *          movie booking system through its TCP server interface. Tests cover JSON protocol
 *          handling, concurrent client management, error recovery, and end-to-end workflows.
 * @author Alejandro Martinez Lopez
 * @date 2025
 */

#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include <boost/json.hpp>
#include <future>
#include <vector>
#include <atomic>

#include "Controller/TcpServer.h"
#include "Models/BookingService.h"
#include "Models/Theater.h"
#include "Models/Movie.h"

using boost::asio::ip::tcp;
namespace json = boost::json;

class TcpServerFunctionalTest : public ::testing::Test {
protected:
    static std::atomic<int> test_counter_;
    static constexpr unsigned short base_port_ = 13000;
    unsigned short port_;
    boost::asio::io_context io_context_;
    std::unique_ptr<std::thread> server_thread_;
    std::unique_ptr<TcpServer> server_;
    std::unique_ptr<BookingService> booking_service_;

    void SetUp() override {
        port_ = base_port_ + test_counter_++;
        booking_service_ = std::make_unique<BookingService>();
        
        // Setup test data
        Movie m1(1, "Inception");
        Movie m2(2, "The Matrix");
        booking_service_->add_movie(m1);
        booking_service_->add_movie(m2);

        auto t1 = std::make_shared<Theater>(1, "Cinema One");
        auto t2 = std::make_shared<Theater>(2, "Cinema Two");
        t1->add_movie(m1);
        t1->add_movie(m2);
        t2->add_movie(m2);

        booking_service_->add_theater(t1);
        booking_service_->add_theater(t2);

        server_ = std::make_unique<TcpServer>(io_context_, port_, *booking_service_, 2);
        server_thread_ = std::make_unique<std::thread>([this]() {
            try {
                server_->start();
                io_context_.run();
            } catch (const std::exception& e) {
                std::cerr << "Server error: " << e.what() << std::endl;
            }
        });

        ASSERT_TRUE(wait_for_server_ready()) << "Server failed to start within timeout";
    }

    void TearDown() override {
        io_context_.stop();
        if (server_thread_ && server_thread_->joinable()) {
            server_thread_->join();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    bool wait_for_server_ready() {
        auto start_time = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(3)) {
            try {
                boost::asio::io_context test_ctx;
                tcp::socket test_socket(test_ctx);
                test_socket.connect(tcp::endpoint(
                    boost::asio::ip::address::from_string("127.0.0.1"), port_));
                test_socket.close();
                return true;
            } catch (...) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
        return false;
    }

    // Helper for JSON communication only
    json::value send_and_receive_json(const json::value& req, int timeout_ms = 1000) {
        try {
            boost::asio::io_context ctx;
            tcp::socket socket(ctx);
            socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port_));
            
            std::string msg = json::serialize(req) + "\n";
            boost::asio::write(socket, boost::asio::buffer(msg));
            
            boost::asio::streambuf buf;
            boost::system::error_code ec;
            
            auto future = std::async(std::launch::async, [&]() {
                boost::asio::read_until(socket, buf, "\n", ec);
            });
            
            if (future.wait_for(std::chrono::milliseconds(timeout_ms)) == std::future_status::timeout) {
                socket.close();
                return json::object{{"error", "TIMEOUT"}};
            }
            
            if (ec) {
                return json::object{{"error", "NETWORK_ERROR"}};
            }
            
            std::istream is(&buf);
            std::string line;
            std::getline(is, line);
            return json::parse(line);
        } catch (const std::exception& e) {
            return json::object{{"error", std::string("EXCEPTION: ") + e.what()}};
        }
    }
};

std::atomic<int> TcpServerFunctionalTest::test_counter_{0};

// ---- Basic JSON Protocol Tests ----

TEST_F(TcpServerFunctionalTest, ListMoviesJSON) {
    json::value req = {{"command", "LIST_MOVIES"}};
    json::value resp = send_and_receive_json(req);
    
    ASSERT_FALSE(resp.as_object().contains("error"));
    ASSERT_TRUE(resp.as_object().contains("movies"));
    auto& arr = resp.at("movies").as_array();
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr[0].as_object().at("name").as_string(), "Inception");
    EXPECT_EQ(arr[1].as_object().at("name").as_string(), "The Matrix");
}

TEST_F(TcpServerFunctionalTest, ListTheatersJSON) {
    json::value req = {{"command", "LIST_THEATERS"}, {"movie_id", 2}};
    json::value resp = send_and_receive_json(req);
    
    ASSERT_FALSE(resp.as_object().contains("error"));
    ASSERT_TRUE(resp.as_object().contains("theaters"));
    auto& arr = resp.at("theaters").as_array();
    EXPECT_EQ(arr.size(), 2); // Both theaters show The Matrix
}

TEST_F(TcpServerFunctionalTest, ListSeatsJSON) {
    json::value req = {{"command", "LIST_SEATS"}, {"theater_id", 1}, {"movie_id", 1}};
    json::value resp = send_and_receive_json(req);
    
    ASSERT_FALSE(resp.as_object().contains("error"));
    ASSERT_TRUE(resp.as_object().contains("available_seats"));
    EXPECT_EQ(resp.at("available_seats").as_array().size(), 20);
}

TEST_F(TcpServerFunctionalTest, BookSeatsJSON_Success) {
    json::array seats = {"a1", "a2"};
    json::value req = {{"command", "BOOK"}, {"theater_id", 1}, {"movie_id", 1}, {"seats", seats}};
    json::value resp = send_and_receive_json(req);
    
    ASSERT_FALSE(resp.as_object().contains("error"));
    EXPECT_EQ(resp.at("status").as_string(), "BOOKED");
}

TEST_F(TcpServerFunctionalTest, BookSeatsJSON_Failure) {
    json::array seats = {"a3"};
    json::value req = {{"command", "BOOK"}, {"theater_id", 1}, {"movie_id", 1}, {"seats", seats}};
    
    // Book first time
    auto resp1 = send_and_receive_json(req);
    EXPECT_EQ(resp1.at("status").as_string(), "BOOKED");
    
    // Try booking same seat again
    auto resp2 = send_and_receive_json(req);
    EXPECT_EQ(resp2.at("status").as_string(), "FAILED");
}

// ---- Error Handling Tests ----

TEST_F(TcpServerFunctionalTest, UnknownCommandJSON) {
    json::value req = {{"command", "INVALID_COMMAND"}};
    json::value resp = send_and_receive_json(req);
    
    ASSERT_TRUE(resp.as_object().contains("error"));
    EXPECT_EQ(resp.at("error").as_string(), "UNKNOWN_COMMAND");
}

TEST_F(TcpServerFunctionalTest, MalformedJSONHandling) {
    try {
        boost::asio::io_context ctx;
        tcp::socket socket(ctx);
        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port_));
        
        // Send malformed JSON
        std::string malformed_json = "{\"command\": \"LIST_MOVIES\", \"invalid\": }\n";
        boost::asio::write(socket, boost::asio::buffer(malformed_json));
        
        boost::asio::streambuf buf;
        auto future = std::async(std::launch::async, [&]() {
            boost::asio::read_until(socket, buf, "\n");
        });
        
        ASSERT_NE(future.wait_for(std::chrono::seconds(2)), std::future_status::timeout);
        
        std::istream is(&buf);
        std::string response;
        std::getline(is, response);
        
        auto json_response = json::parse(response);
        EXPECT_TRUE(json_response.as_object().contains("error"));
        
    } catch (const std::exception& e) {
        FAIL() << "Exception during malformed JSON test: " << e.what();
    }
}

// ---- Concurrency Tests ----
/**
 * @brief Test concurrent booking attempts for the same seat
 * @details Validates that when multiple clients simultaneously attempt to book
 *          the same seat, only one succeeds while others receive failure responses.
 *          Uses atomic counters for synchronization to ensure true concurrency.
 * @test Verifies thread safety and race condition handling in booking system
 * @note Uses 5 concurrent clients with atomic synchronization for C++17 compatibility
 */
TEST_F(TcpServerFunctionalTest, ConcurrentBookingSameSeat) {
    const int num_clients = 5; // Reduced from 10
    std::vector<std::future<json::value>> futures;
    
    std::atomic<int> ready_count{0};
    std::atomic<bool> all_ready{false};
    
    for (int i = 0; i < num_clients; ++i) {
        futures.push_back(std::async(std::launch::async, [this, &ready_count, &all_ready]() {
            ready_count.fetch_add(1);
            while (!all_ready.load()) {
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
            
            json::array seats = {"c3"};
            json::value req = {{"command", "BOOK"}, {"theater_id", 1}, {"movie_id", 1}, {"seats", seats}};
            return send_and_receive_json(req, 2000);
        }));
    }
    
    while (ready_count.load() < num_clients) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    
    all_ready.store(true);
    
    int successful_bookings = 0;
    int failed_bookings = 0;
    
    for (auto& future : futures) {
        auto response = future.get();
        if (!response.as_object().contains("error")) {
            if (response.at("status").as_string() == "BOOKED") {
                successful_bookings++;
            } else if (response.at("status").as_string() == "FAILED") {
                failed_bookings++;
            }
        }
    }
    
    // Only one client should succeed
    EXPECT_EQ(successful_bookings, 1);
    EXPECT_EQ(failed_bookings, num_clients - 1);
}

/**
 * @brief Test concurrent booking of different seats
 * @details Validates that multiple clients can successfully book different seats
 *          simultaneously without conflicts. Tests the system's ability to handle
 *          non-conflicting concurrent operations efficiently.
 * @test Verifies concurrent booking success for non-overlapping seat requests
 * @note Uses seats a11-a15 to ensure all requests are within valid seat range (a1-a20)
 */
TEST_F(TcpServerFunctionalTest, ConcurrentBookingDifferentSeats) {
    const int num_clients = 5; // Reduced and using valid seat range
    std::vector<std::future<json::value>> futures;
    
    std::atomic<int> ready_count{0};
    std::atomic<bool> all_ready{false};
    
    for (int i = 0; i < num_clients; ++i) {
        futures.push_back(std::async(std::launch::async, [this, i, &ready_count, &all_ready]() {
            ready_count.fetch_add(1);
            while (!all_ready.load()) {
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
            
            // Generate valid seat IDs for sqrt(20) = 5 seats per row
            char row = 'a' + (i / 5);
            int seat = (i % 5) + 1; 
            json::array seats = {"a" + std::to_string(11 + i)};
            json::value req = {{"command", "BOOK"}, {"theater_id", 1}, {"movie_id", 1}, {"seats", seats}};
            return send_and_receive_json(req, 2000);
        }));
    }
    
    while (ready_count.load() < num_clients) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    
    all_ready.store(true);
    
    int successful_bookings = 0;
    for (auto& future : futures) {
        auto response = future.get();
        if (!response.as_object().contains("error") && 
            response.at("status").as_string() == "BOOKED") {
            successful_bookings++;
        }
    }
    
    // All clients should succeed since they're booking different seats
    EXPECT_EQ(successful_bookings, num_clients);
}

// ---- Integration Workflow Test ----
/**
 * @brief Test complete booking workflow from discovery to confirmation
 * @details Validates the entire user journey: listing movies, finding theaters,
 *          checking seat availability, booking seats, and verifying the booking.
 *          This test simulates a real user interaction with the system.
 * @test Verifies end-to-end booking workflow and state consistency
 * @note Tests the complete integration of all system components through the TCP interface
 */
TEST_F(TcpServerFunctionalTest, CompleteBookingWorkflow) {
    // 1. List movies
    json::value list_movies_req = {{"command", "LIST_MOVIES"}};
    auto movies_resp = send_and_receive_json(list_movies_req);
    ASSERT_TRUE(movies_resp.as_object().contains("movies"));
    
    // 2. List theaters for a movie
    json::value list_theaters_req = {{"command", "LIST_THEATERS"}, {"movie_id", 1}};
    auto theaters_resp = send_and_receive_json(list_theaters_req);
    ASSERT_TRUE(theaters_resp.as_object().contains("theaters"));
    
    // 3. List available seats
    json::value list_seats_req = {{"command", "LIST_SEATS"}, {"theater_id", 1}, {"movie_id", 1}};
    auto seats_resp = send_and_receive_json(list_seats_req);
    ASSERT_TRUE(seats_resp.as_object().contains("available_seats"));
    EXPECT_EQ(seats_resp.at("available_seats").as_array().size(), 20);
    
    // 4. Book seats
    json::array seats = {"d3", "d4"};
    json::value book_req = {{"command", "BOOK"}, {"theater_id", 1}, {"movie_id", 1}, {"seats", seats}};
    auto book_resp = send_and_receive_json(book_req);
    EXPECT_EQ(book_resp.at("status").as_string(), "BOOKED");
    
    // 5. Verify seats are no longer available
    auto final_seats_resp = send_and_receive_json(list_seats_req);
    EXPECT_EQ(final_seats_resp.at("available_seats").as_array().size(), 18);
}
