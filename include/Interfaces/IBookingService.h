/**
 * @file IBookingService.h
 * @brief Abstract interface for booking service operations
 */

#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Models/Movie.h"

// Forward declaration
class ITheater;

/**
 * @interface IBookingService
 * @brief Abstract interface for booking service operations
 * @details Defines the contract for all booking service implementations.
 * Handles seat booking, availability queries, and booking-related
 * operations following the Single Responsibility Principle.
 */
class IBookingService {
public:
  virtual ~IBookingService() = default;

  /**
   * @brief Retrieve all available movies in the system
   * @return Vector containing all movies currently available for booking
   */
  virtual std::vector<Movie> get_all_movies() const = 0;

  /**
   * @brief Get all theaters showing a specific movie
   * @param movie_id Unique identifier of the movie
   * @return Vector of shared pointers to theaters showing the specified movie
   */
  virtual std::vector<std::shared_ptr<ITheater>> get_theaters_showing_movie(int movie_id) const = 0;

  /**
   * @brief Get available seats for a specific movie in a theater
   * @param theater_id Unique identifier of the theater
   * @param movie_id Unique identifier of the movie
   * @return Vector of seat IDs that are currently available for booking
   */
  virtual std::vector<std::string> get_available_seats(int theater_id, int movie_id) const = 0;

  /**
   * @brief Attempt to book specified seats for a movie showing
   * @param theater_id Unique identifier of the theater
   * @param movie_id Unique identifier of the movie
   * @param seat_ids Vector of seat IDs to book
   * @return true if all seats were successfully booked, false otherwise
   */
  virtual bool book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) = 0;

  /**
   * @brief Check if specified seats can be booked without actually booking them
   * @param theater_id Unique identifier of the theater
   * @param movie_id Unique identifier of the movie
   * @param seat_ids Vector of seat IDs to check
   * @return true if all seats are available for booking, false otherwise
   */
  virtual bool can_book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) const = 0;
};
