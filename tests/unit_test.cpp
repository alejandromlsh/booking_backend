#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_set>
#include <future>
#include <chrono>
#include <random>


#include "Models/Movie.h"
#include "Models/Seat.h"
#include "Models/Theater.h"
#include "Models/BookingService.h"
 

// ---- Movie Tests ----
TEST(MovieTest, ConstructorAndGetters) {
    Movie m(42, "Blade Runner");
    EXPECT_EQ(m.get_id(), 42);
    EXPECT_EQ(m.get_name(), "Blade Runner");
}

// ---- Seat Tests ----
TEST(SeatTest, ConstructorAndGetters) {
    Seat s("a1");
    EXPECT_EQ(s.get_id(), "a1");
    EXPECT_TRUE(s.is_available());
}

TEST(SeatTest, BookSeatOnce) {
    Seat s("a2");
    EXPECT_TRUE(s.is_available());
    EXPECT_TRUE(s.book());
    EXPECT_FALSE(s.is_available());
}

TEST(SeatTest, BookSeatTwiceFails) {
    Seat s("a3");
    EXPECT_TRUE(s.book());
    EXPECT_FALSE(s.book()); // Second booking should fail
}

// ---- Theater Tests ----
TEST(TheaterTest, ConstructorAndGetters) {
    Theater t(1, "Grand Cinema");
    EXPECT_EQ(t.get_id(), 1);
    EXPECT_EQ(t.get_name(), "Grand Cinema");
}

TEST(TheaterTest, AddMovieAndInitializeSeats) {
    Theater t(2, "Indie Cinema");
    Movie m(77, "Arrival");
    t.add_movie(m);
    auto seats = t.get_available_seats(m.get_id());
    EXPECT_EQ(seats.size(), 20); // Default seat count
    std::unordered_set<std::string> expected;
    for (int i = 1; i <= 20; ++i) expected.insert("a" + std::to_string(i));
    for (const auto& seat_id : seats) {
        EXPECT_TRUE(expected.count(seat_id));
    }
}

TEST(TheaterTest, BookSeatsSuccess) {
    Theater t(3, "SciFi Cinema");
    Movie m(55, "Dune");
    t.add_movie(m);
    std::vector<std::string> to_book = {"a1", "a2"};
    EXPECT_TRUE(t.book_seats(m.get_id(), to_book));
    auto available = t.get_available_seats(m.get_id());
    EXPECT_EQ(available.size(), 18);
    EXPECT_FALSE(std::find(available.begin(), available.end(), "a1") != available.end());
}

TEST(TheaterTest, BookSeatsFailureAlreadyBooked) {
    Theater t(4, "Classic Cinema");
    Movie m(88, "Casablanca");
    t.add_movie(m);
    std::vector<std::string> to_book = {"a1", "a2"};
    EXPECT_TRUE(t.book_seats(m.get_id(), to_book));
    // Try booking again
    EXPECT_FALSE(t.book_seats(m.get_id(), {"a1"}));
}

TEST(TheaterTest, BookSeatsFailureNonExistentSeat) {
    Theater t(5, "Tiny Cinema");
    Movie m(99, "Short Film");
    t.add_movie(m);
    EXPECT_FALSE(t.book_seats(m.get_id(), {"a21"})); // Only a1-a20 exist
}

TEST(TheaterTest, ShowsMovie) {
    Theater t(6, "Test Cinema");
    Movie m1(1, "Movie1");
    Movie m2(2, "Movie2");
    t.add_movie(m1);
    EXPECT_TRUE(t.shows_movie(1));
    EXPECT_FALSE(t.shows_movie(2));
    t.add_movie(m2);
    EXPECT_TRUE(t.shows_movie(2));
}

// ---- BookingService Tests ----
TEST(BookingServiceTest, AddMovieAndGetAllMovies) {
    BookingService svc;
    Movie m1(1, "Inception");
    Movie m2(2, "Matrix");
    svc.add_movie(m1);
    svc.add_movie(m2);
    auto movies = svc.get_all_movies();
    ASSERT_EQ(movies.size(), 2);
    EXPECT_EQ(movies[0].get_id(), 1);
    EXPECT_EQ(movies[1].get_id(), 2);
}

TEST(BookingServiceTest, AddTheaterAndGetTheatersShowingMovie) {
    BookingService svc;
    Movie m(1, "Interstellar");
    svc.add_movie(m);
    auto t = std::make_shared<Theater>(10, "CinemaX");
    t->add_movie(m);
    svc.add_theater(t);
    auto theaters = svc.get_theaters_showing_movie(1);
    ASSERT_EQ(theaters.size(), 1);
    EXPECT_EQ(theaters[0]->get_id(), 10);
}

TEST(BookingServiceTest, GetAvailableSeats) {
    BookingService svc;
    Movie m(1, "Tenet");
    svc.add_movie(m);
    auto t = std::make_shared<Theater>(20, "CinemaY");
    t->add_movie(m);
    svc.add_theater(t);
    auto seats = svc.get_available_seats(20, 1);
    EXPECT_EQ(seats.size(), 20);
}

TEST(BookingServiceTest, BookSeatsSuccessAndFailure) {
    BookingService svc;
    Movie m(1, "Dune");
    svc.add_movie(m);
    auto t = std::make_shared<Theater>(30, "CinemaZ");
    t->add_movie(m);
    svc.add_theater(t);
    // Book valid seats
    EXPECT_TRUE(svc.book_seats(30, 1, {"a1", "a2"}));
    // Booking again should fail
    EXPECT_FALSE(svc.book_seats(30, 1, {"a1"}));
    // Booking non-existent seat
    EXPECT_FALSE(svc.book_seats(30, 1, {"a21"}));
}

// ---- Concurrency and Thread Safety Tests ----

TEST(SeatTest, ConcurrentBookingRaceCondition) {
    Seat seat("a1");
    const int num_threads = 10;
    std::vector<std::future<bool>> futures;
    
    // Launch multiple threads trying to book the same seat
    for (int i = 0; i < num_threads; ++i) {
        futures.push_back(std::async(std::launch::async, [&seat]() {
            return seat.book();
        }));
    }
    
    // Collect results
    int successful_bookings = 0;
    for (auto& future : futures) {
        if (future.get()) {
            successful_bookings++;
        }
    }
    
    // Only one thread should succeed
    EXPECT_EQ(successful_bookings, 1);
    EXPECT_FALSE(seat.is_available());
}

TEST(TheaterTest, ConcurrentSeatBookingStressTest) {
    Theater theater(1, "Stress Test Cinema");
    Movie movie(1, "Concurrent Movie");
    theater.add_movie(movie);
    
    const int num_threads = 20;
    const int seats_per_thread = 1;
    std::vector<std::future<bool>> futures;
    std::atomic<int> successful_bookings{0};
    
    // Each thread tries to book different seats
    for (int i = 0; i < num_threads; ++i) {
        futures.push_back(std::async(std::launch::async, [&theater, &movie, i, &successful_bookings]() {
            std::vector<std::string> seats_to_book = {"a" + std::to_string(i + 1)};
            bool success = theater.book_seats(movie.get_id(), seats_to_book);
            if (success) successful_bookings++;
            return success;
        }));
    }
    
    // Wait for all threads
    for (auto& future : futures) {
        future.wait();
    }
    
    EXPECT_EQ(successful_bookings.load(), num_threads);
    auto available_seats = theater.get_available_seats(movie.get_id());
    EXPECT_EQ(available_seats.size(), 20 - num_threads);
}

TEST(TheaterTest, ConcurrentSameSeatBookingConflict) {
    Theater theater(2, "Conflict Test Cinema");
    Movie movie(2, "Conflict Movie");
    theater.add_movie(movie);
    
    const int num_threads = 50;
    std::vector<std::future<bool>> futures;
    
    // All threads try to book the same seat
    for (int i = 0; i < num_threads; ++i) {
        futures.push_back(std::async(std::launch::async, [&theater, &movie]() {
            return theater.book_seats(movie.get_id(), {"a1"});
        }));
    }
    
    int successful_bookings = 0;
    for (auto& future : futures) {
        if (future.get()) {
            successful_bookings++;
        }
    }
    
    // Only one thread should succeed
    EXPECT_EQ(successful_bookings, 1);
}

TEST(BookingServiceTest, ConcurrentServiceOperations) {
    BookingService service;
    Movie movie(1, "Service Test Movie");
    service.add_movie(movie);
    
    auto theater = std::make_shared<Theater>(1, "Service Test Theater");
    theater->add_movie(movie);
    service.add_theater(theater);
    
    const int num_threads = 30;
    std::vector<std::future<bool>> futures;
    std::atomic<int> successful_bookings{0};
    
    // Multiple threads booking different seats through the service
    for (int i = 0; i < num_threads && i < 20; ++i) {
        futures.push_back(std::async(std::launch::async, [&service, i, &successful_bookings]() {
            std::vector<std::string> seats = {"a" + std::to_string(i + 1)};
            bool success = service.book_seats(1, 1, seats);
            if (success) successful_bookings++;
            return success;
        }));
    }
    
    for (auto& future : futures) {
        future.wait();
    }
    
    EXPECT_EQ(successful_bookings.load(), std::min(num_threads, 20));
}


