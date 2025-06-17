/**
 * @file IDataStore.h
 * @brief Abstract interface for centralized data storage operations
 */

#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Models/Movie.h"

// Forward declaration to avoid circular dependency
class ITheater;

/**
 * @interface IDataStore
 * @brief Abstract interface for centralized data storage operations
 * @details Defines the contract for all data storage implementations in the movie booking system.
 * Provides unified access to movies and theaters with proper abstraction layer.
 * This interface enables dependency injection and supports multiple storage backends.
 */
class IDataStore {
public:
  virtual ~IDataStore() = default;

  /**
   * @brief Add a movie to the data store
   * @param movie Movie object to add (moved for efficiency)
   */
  virtual void add_movie(Movie&& movie) = 0;

  /**
   * @brief Remove a movie from the data store
   * @param movie_id Unique identifier of the movie to remove
   */
  virtual void remove_movie(int movie_id) = 0;

  /**
   * @brief Retrieve a specific movie by ID
   * @param movie_id Unique identifier of the movie
   * @return Movie object if found
   * @throws std::runtime_error if movie not found
   */
  virtual Movie get_movie(int movie_id) const = 0;

  /**
   * @brief Retrieve all movies from the data store
   * @return Vector containing all movies
   */
  virtual std::vector<Movie> get_all_movies() const = 0;

  /**
   * @brief Check if a movie exists in the data store
   * @param movie_id Unique identifier of the movie
   * @return true if movie exists, false otherwise
   */
  virtual bool movie_exists(int movie_id) const = 0;

  /**
   * @brief Add a theater to the data store
   * @param theater Shared pointer to theater to add
   */
  virtual void add_theater(std::shared_ptr<ITheater> theater) = 0;

  /**
   * @brief Remove a theater from the data store
   * @param theater_id Unique identifier of the theater to remove
   */
  virtual void remove_theater(int theater_id) = 0;

  /**
   * @brief Retrieve a specific theater by ID
   * @param theater_id Unique identifier of the theater
   * @return Shared pointer to theater if found, nullptr otherwise
   */
  virtual std::shared_ptr<ITheater> get_theater(int theater_id) const = 0;

  /**
   * @brief Retrieve all theaters from the data store
   * @return Vector of shared pointers to all theaters
   */
  virtual std::vector<std::shared_ptr<ITheater>> get_all_theaters() const = 0;

  /**
   * @brief Get theaters showing a specific movie
   * @param movie_id Unique identifier of the movie
   * @return Vector of shared pointers to theaters showing the movie
   */
  virtual std::vector<std::shared_ptr<ITheater>> get_theaters_showing_movie(int movie_id) const = 0;

  /**
   * @brief Check if a theater exists in the data store
   * @param theater_id Unique identifier of the theater
   * @return true if theater exists, false otherwise
   */
  virtual bool theater_exists(int theater_id) const = 0;

  /**
   * @brief Get available seats for a movie in a theater
   * @param theater_id Unique identifier of the theater
   * @param movie_id Unique identifier of the movie
   * @return Vector of available seat IDs
   */
  virtual std::vector<std::string> get_available_seats(int theater_id, int movie_id) const = 0;

  /**
   * @brief Book seats for a movie in a theater
   * @param theater_id Unique identifier of the theater
   * @param movie_id Unique identifier of the movie
   * @param seat_ids Vector of seat IDs to book
   * @return true if booking successful, false otherwise
   */
  virtual bool book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) = 0;
};
