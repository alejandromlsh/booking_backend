/**
 * @file ITheater.h
 * @brief Abstract interface for theater implementations
 */

#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Models/Movie.h"

/**
 * @interface ITheater
 * @brief Abstract interface for theater implementations
 * @details Defines the contract for all theater types in the movie booking system.
 *          Manages movie scheduling, seat availability, and booking operations.
 *          Supports different theater configurations through polymorphic behavior.
 */
class ITheater {
public:
  virtual ~ITheater() = default;
  
  virtual void add_movie(const Movie& movie) = 0;
  virtual std::vector<std::string> get_available_seats(int movie_id) const = 0;
  virtual bool book_seats(int movie_id, const std::vector<std::string>& seat_ids) = 0;
  virtual int get_id() const = 0;
  virtual std::string get_name() const = 0;
  virtual bool shows_movie(int movie_id) const = 0;
};
