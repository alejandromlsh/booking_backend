/**
 * @file BookingService.h
 * @brief Service responsible for booking operations only
 */

#pragma once
#include "Interfaces/IBookingService.h"
#include "Interfaces/IDataStore.h"
#include "Interfaces/ITheater.h"
#include "Models/Movie.h"
#include <memory>
#include <vector>
#include <string>

/**
 * @class BookingService
 * @brief Service responsible for booking operations only
 * @details Handles seat booking, availability queries, and booking-related
 *          operations. Uses dependency injection for loose coupling.
 *          Implements the IBookingService interface.
 */
class BookingService : public IBookingService {
public:
  explicit BookingService(std::shared_ptr<IDataStore> data_store);
  
  std::vector<Movie> get_all_movies() const override;
  std::vector<std::shared_ptr<ITheater>> get_theaters_showing_movie(int movie_id) const override;
  std::vector<std::string> get_available_seats(int theater_id, int movie_id) const override;
  bool book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) override;
  bool can_book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) const override;

private:
  std::shared_ptr<IDataStore> data_store_;
};
