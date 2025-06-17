/**
 * @file IBookingService.h
 * @brief Abstract interface for booking service operations
 */

#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Models/Movie.h"

// Forward declaration
class ITheater;

/**
 * @interface IBookingService
 * @brief Abstract interface for booking service operations
 * @details Defines the contract for all booking service implementations.
 *          Handles seat booking, availability queries, and booking-related
 *          operations following the Single Responsibility Principle.
 */
class IBookingService {
public:
  virtual ~IBookingService() = default;
  
  virtual std::vector<Movie> get_all_movies() const = 0;
  virtual std::vector<std::shared_ptr<ITheater>> get_theaters_showing_movie(int movie_id) const = 0;
  virtual std::vector<std::string> get_available_seats(int theater_id, int movie_id) const = 0;
  virtual bool book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) = 0;
  virtual bool can_book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) const = 0;
};
