#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include "Movie.h"
#include "Theater.h"
#include "CentralDataStore.h"

/**
 * @class BookingService
 * @brief Service responsible for booking operations only
 * @details Handles seat booking, availability queries, and booking-related
 *          operations. Administrative functions have been moved to
 *          AdministrationService following Single Responsibility Principle.
 */
class BookingService {
public:
  explicit BookingService(std::shared_ptr<CentralDataStore> data_store);

  // Read-only queries for booking interface
  std::vector<Movie> get_all_movies() const;
  std::vector<std::shared_ptr<Theater>> get_theaters_showing_movie(int movie_id) const;
  std::vector<std::string> get_available_seats(int theater_id, int movie_id) const;

  // Booking operations
  bool book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids);

  // Booking validation
  bool can_book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) const;

private:
  std::shared_ptr<CentralDataStore> data_store_;
  mutable std::mutex booking_mutex_;
};
