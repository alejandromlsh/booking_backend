/**
 * @file IAdministrationService.h
 * @brief Abstract interface for system administration operations
 */

#pragma once

#include <vector>
#include <memory>
#include "Models/Movie.h"

// Forward declaration
class ITheater;

/**
 * @interface IAdministrationService
 * @brief Abstract interface for system administration operations
 * @details Defines the contract for all administration service implementations.
 * Handles all administrative tasks including movie management,
 * theater management, and system configuration.
 */
class IAdministrationService {
public:
  virtual ~IAdministrationService() = default;

  /**
   * @brief Add a new movie to the system
   * @param movie Movie object to add (moved for efficiency)
   */
  virtual void add_movie(Movie&& movie) = 0;

  /**
   * @brief Remove a movie from the system
   * @param movie_id Unique identifier of the movie to remove
   */
  virtual void remove_movie(int movie_id) = 0;

  /**
   * @brief Retrieve all movies in the system
   * @return Vector containing all movies in the system
   */
  virtual std::vector<Movie> get_all_movies() const = 0;

  /**
   * @brief Check if a movie exists in the system
   * @param movie_id Unique identifier of the movie to check
   * @return true if movie exists, false otherwise
   */
  virtual bool movie_exists(int movie_id) const = 0;

  /**
   * @brief Add a new theater to the system
   * @param theater Shared pointer to theater to add
   */
  virtual void add_theater(std::shared_ptr<ITheater> theater) = 0;

  /**
   * @brief Remove a theater from the system
   * @param theater_id Unique identifier of the theater to remove
   */
  virtual void remove_theater(int theater_id) = 0;

  /**
   * @brief Retrieve all theaters in the system
   * @return Vector of shared pointers to all theaters
   */
  virtual std::vector<std::shared_ptr<ITheater>> get_all_theaters() const = 0;

  /**
   * @brief Check if a theater exists in the system
   * @param theater_id Unique identifier of the theater to check
   * @return true if theater exists, false otherwise
   */
  virtual bool theater_exists(int theater_id) const = 0;

  /**
   * @brief Schedule a movie to be shown in a specific theater
   * @param theater_id Unique identifier of the theater
   * @param movie Movie object to schedule (moved for efficiency)
   */
  virtual void schedule_movie_in_theater(int theater_id, Movie&& movie) = 0;

  /**
   * @brief Remove a movie from a theater's schedule
   * @param theater_id Unique identifier of the theater
   * @param movie_id Unique identifier of the movie to remove
   */
  virtual void remove_movie_from_theater(int theater_id, int movie_id) = 0;

  /**
   * @brief Set the seating capacity for a theater
   * @param theater_id Unique identifier of the theater
   * @param capacity New seating capacity
   */
  virtual void set_theater_capacity(int theater_id, int capacity) = 0;
};
