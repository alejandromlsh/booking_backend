/**
 * @file unit_test.cpp
 * @brief Comprehensive unit tests for the Movie Booking System
 * @details This file contains unit tests for all core components of the movie booking system,
 *          including basic functionality tests and advanced concurrency/thread safety tests.
 * @author Alejandro Martinez Lopez
 * @date 2025
 */

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
#include "Models/AdministrationService.h"
#include "Models/CentralDataStore.h"

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
  EXPECT_EQ(seats.size(), 20); // Still 20 total seats
  
  // Expected seat layout for sqrt(20) ≈ 4.47 → 5 seats per row, 4 rows
  std::unordered_set<std::string> expected{
    "a1", "a2", "a3", "a4", "a5",
    "b1", "b2", "b3", "b4", "b5",
    "c1", "c2", "c3", "c4", "c5",
    "d1", "d2", "d3", "d4", "d5"
  };
  
  for (const auto& seat_id : seats) {
    EXPECT_TRUE(expected.count(seat_id));
  }
}

TEST(TheaterTest, BookSeatsSuccess) {
  Theater t(3, "SciFi Cinema");
  Movie m(55, "Dune");
  t.add_movie(m);
  std::vector<std::string> to_book = {"a1", "b2"};
  EXPECT_TRUE(t.book_seats(m.get_id(), to_book));
  auto available = t.get_available_seats(m.get_id());
  EXPECT_EQ(available.size(), 18);
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
  EXPECT_FALSE(t.book_seats(m.get_id(), {"f1"})); // f1 doesn't exist in 5x4 grid
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

// ---- Administration Service Tests ----
TEST(AdministrationServiceTest, AddMovieAndGetAllMovies) {
  auto data_store = std::make_shared<CentralDataStore>();
  AdministrationService admin_svc(data_store);
  
  Movie m1(1, "Inception");
  Movie m2(2, "Matrix");
  admin_svc.add_movie(m1);
  admin_svc.add_movie(m2);
  
  auto movies = admin_svc.get_all_movies();
  ASSERT_EQ(movies.size(), 2);
  EXPECT_EQ(movies[0].get_id(), 1);
  EXPECT_EQ(movies[1].get_id(), 2);
}

TEST(AdministrationServiceTest, AddTheaterAndManagement) {
  auto data_store = std::make_shared<CentralDataStore>();
  AdministrationService admin_svc(data_store);
  
  Movie m(1, "Interstellar");
  admin_svc.add_movie(m);
  
  auto t = std::make_shared<Theater>(10, "CinemaX");
  t->add_movie(m);
  admin_svc.add_theater(t);
  
  auto theaters = admin_svc.get_all_theaters();
  ASSERT_EQ(theaters.size(), 1);
  EXPECT_EQ(theaters[0]->get_id(), 10);
}

// ---- Booking Service Tests ----
TEST(BookingServiceTest, GetAllMoviesReadOnly) {
  auto data_store = std::make_shared<CentralDataStore>();
  AdministrationService admin_svc(data_store);
  BookingService booking_svc(data_store);
  
  // Add movies through administration service
  Movie m1(1, "Inception");
  Movie m2(2, "Matrix");
  admin_svc.add_movie(m1);
  admin_svc.add_movie(m2);
  
  // Access movies through booking service
  auto movies = booking_svc.get_all_movies();
  ASSERT_EQ(movies.size(), 2);
  EXPECT_EQ(movies[0].get_id(), 1);
  EXPECT_EQ(movies[1].get_id(), 2);
}

TEST(BookingServiceTest, GetTheatersShowingMovie) {
  auto data_store = std::make_shared<CentralDataStore>();
  AdministrationService admin_svc(data_store);
  BookingService booking_svc(data_store);
  
  Movie m(1, "Interstellar");
  admin_svc.add_movie(m);
  
  auto t = std::make_shared<Theater>(10, "CinemaX");
  t->add_movie(m);
  admin_svc.add_theater(t);
  
  auto theaters = booking_svc.get_theaters_showing_movie(1);
  ASSERT_EQ(theaters.size(), 1);
  EXPECT_EQ(theaters[0]->get_id(), 10);
}

TEST(BookingServiceTest, GetAvailableSeats) {
  auto data_store = std::make_shared<CentralDataStore>();
  AdministrationService admin_svc(data_store);
  BookingService booking_svc(data_store);
  
  Movie m(1, "Tenet");
  admin_svc.add_movie(m);
  
  auto t = std::make_shared<Theater>(20, "CinemaY");
  t->add_movie(m);
  admin_svc.add_theater(t);
  
  auto seats = booking_svc.get_available_seats(20, 1);
  EXPECT_EQ(seats.size(), 20);
}

TEST(BookingServiceTest, BookSeatsSuccessAndFailure) {
  auto data_store = std::make_shared<CentralDataStore>();
  AdministrationService admin_svc(data_store);
  BookingService booking_svc(data_store);
  
  Movie m(1, "Dune");
  admin_svc.add_movie(m);
  
  auto t = std::make_shared<Theater>(30, "CinemaZ");
  t->add_movie(m);
  admin_svc.add_theater(t);
  
  // Book valid seats
  EXPECT_TRUE(booking_svc.book_seats(30, 1, {"a1", "a2"}));
  // Booking again should fail
  EXPECT_FALSE(booking_svc.book_seats(30, 1, {"a1"}));
  // Booking non-existent seat
  EXPECT_FALSE(booking_svc.book_seats(30, 1, {"f1"})); // f1 doesn't exist
}

// ---- Concurrency and Thread Safety Tests ----

/**
 * @brief Test atomic seat booking under race conditions
 * @details Validates that the atomic booking mechanism in the Seat class
 *          correctly handles concurrent booking attempts, ensuring only one
 *          thread can successfully book a seat even under race conditions.
 * @test Verifies atomic seat booking and race condition handling
 * @note This test uses 10 concurrent threads to stress-test the atomic mechanism
 */
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

/**
 * @brief Test concurrent booking of different seats in theater
 * @details Validates that the theater can handle concurrent booking requests
 *          for different seats without conflicts, ensuring all bookings succeed
 *          when seats don't overlap.
 * @test Verifies theater thread safety for non-conflicting concurrent operations
 * @note This test uses 20 concurrent threads booking different seats (a1-a20)
 */
TEST(TheaterTest, ConcurrentSeatBookingStressTest) {
  Theater theater(1, "Stress Test Cinema");
  Movie movie(1, "Concurrent Movie");
  theater.add_movie(movie);
  
  const int num_threads = 20;
  std::vector<std::future<bool>> futures;
  std::atomic<int> successful_bookings{0};
  
  // Create valid seat IDs for 5x4 grid layout
  std::vector<std::string> valid_seats = {
    "a1", "a2", "a3", "a4", "a5",
    "b1", "b2", "b3", "b4", "b5", 
    "c1", "c2", "c3", "c4", "c5",
    "d1", "d2", "d3", "d4", "d5"
  };
  
  // Each thread tries to book different seats (only first 20 will succeed)
  for (int i = 0; i < num_threads; ++i) {
    futures.push_back(std::async(std::launch::async, [&theater, &movie, i, &successful_bookings, &valid_seats]() {
      if (i < valid_seats.size()) {
        std::vector<std::string> seats_to_book = {valid_seats[i]};
        bool success = theater.book_seats(movie.get_id(), seats_to_book);
        if (success) successful_bookings++;
        return success;
      }
      return false;
    }));
  }
  
  // Wait for all threads
  for (auto& future : futures) {
    future.wait();
  }
  
  // Only 20 seats exist, so maximum 20 successful bookings
  EXPECT_EQ(successful_bookings.load(), std::min(num_threads, 20));
  auto available_seats = theater.get_available_seats(movie.get_id());
  EXPECT_EQ(available_seats.size(), 20 - std::min(num_threads, 20));
}

/**
 * @brief Test concurrent booking conflicts for the same seat
 * @details Validates that when multiple threads attempt to book the same seat
 *          simultaneously, only one succeeds while all others fail gracefully.
 *          This is a high-stress test with 50 concurrent threads.
 * @test Verifies theater-level thread safety under maximum contention
 * @note This test uses 50 concurrent threads all attempting to book seat "a1"
 */
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

/**
 * @brief Test concurrent operations through booking service
 * @details Validates that the booking service can handle concurrent booking
 *          requests safely, ensuring thread safety at the service layer.
 *          Tests the complete booking workflow under concurrent access.
 * @test Verifies end-to-end system thread safety through the service layer
 * @note This test uses up to 30 concurrent threads booking through the service
 */
TEST(BookingServiceTest, ConcurrentServiceOperations) {
  auto data_store = std::make_shared<CentralDataStore>();
  AdministrationService admin_svc(data_store);
  BookingService booking_svc(data_store);
  
  Movie movie(1, "Service Test Movie");
  admin_svc.add_movie(movie);
  
  auto theater = std::make_shared<Theater>(1, "Service Test Theater");
  theater->add_movie(movie);
  admin_svc.add_theater(theater);
  
  const int num_threads = 30;
  std::vector<std::future<bool>> futures;
  std::atomic<int> successful_bookings{0};
  
  // Create valid seat IDs for 5x4 grid layout
  std::vector<std::string> valid_seats = {
    "a1", "a2", "a3", "a4", "a5",
    "b1", "b2", "b3", "b4", "b5", 
    "c1", "c2", "c3", "c4", "c5",
    "d1", "d2", "d3", "d4", "d5"
  };
  
  // Multiple threads booking different seats through the service
  for (int i = 0; i < num_threads && i < valid_seats.size(); ++i) {
    futures.push_back(std::async(std::launch::async, [&booking_svc, i, &successful_bookings, &valid_seats]() {
      std::vector<std::string> seats = {valid_seats[i]};
      bool success = booking_svc.book_seats(1, 1, seats);
      if (success) successful_bookings++;
      return success;
    }));
  }
  
  for (auto& future : futures) {
    future.wait();
  }
  
  // Only 20 seats exist, so expect min(30, 20) = 20 successful bookings
  EXPECT_EQ(successful_bookings.load(), std::min(num_threads, 20));
}

