/**
 * @file ITheater.h
 * @brief Abstract interface for theater implementations
 */

#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Models/Movie.h"

/**
 * @interface ITheater
 * @brief Abstract interface for theater implementations
 * @details Defines the contract for all theater types in the movie booking system.
 * Manages movie scheduling, seat availability, and booking operations.
 * Supports different theater configurations through polymorphic behavior.
 */
class ITheater {
public:
  virtual ~ITheater() = default;

  /**
   * @brief Add a movie to the theater's schedule
   * @param movie Movie object to add (moved for efficiency)
   */
  virtual void add_movie(Movie&& movie) = 0;

  /**
   * @brief Get available seats for a specific movie
   * @param movie_id Unique identifier of the movie
   * @return Vector of available seat IDs
   */
  virtual std::vector<std::string> get_available_seats(int movie_id) const = 0;

  /**
   * @brief Book specified seats for a movie
   * @param movie_id Unique identifier of the movie
   * @param seat_ids Vector of seat IDs to book
   * @return true if all seats were successfully booked, false otherwise
   */
  virtual bool book_seats(int movie_id, const std::vector<std::string>& seat_ids) = 0;

  /**
   * @brief Get the theater's unique identifier
   * @return Theater ID
   */
  virtual int get_id() const = 0;

  /**
   * @brief Get the theater's name
   * @return Theater name
   */
  virtual std::string get_name() const = 0;

  /**
   * @brief Check if theater shows a specific movie
   * @param movie_id Unique identifier of the movie
   * @return true if theater shows the movie, false otherwise
   */
  virtual bool shows_movie(int movie_id) const = 0;
};
