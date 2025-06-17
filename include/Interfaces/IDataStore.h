/**
 * @file IDataStore.h
 * @brief Abstract interface for centralized data storage operations
 */

#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Models/Movie.h"

// Forward declaration to avoid circular dependency
class ITheater;

/**
 * @interface IDataStore
 * @brief Abstract interface for centralized data storage operations
 * @details Defines the contract for all data storage implementations in the movie booking system.
 *          Provides unified access to movies and theaters with proper abstraction layer.
 *          This interface enables dependency injection and supports multiple storage backends.
 */
class IDataStore {
public:
  virtual ~IDataStore() = default;
  
  virtual void add_movie(Movie&& movie) = 0;
  virtual void remove_movie(int movie_id) = 0;
  virtual Movie get_movie(int movie_id) const = 0;
  virtual std::vector<Movie> get_all_movies() const = 0;
  virtual bool movie_exists(int movie_id) const = 0;
  
  virtual void add_theater(std::shared_ptr<ITheater> theater) = 0;
  virtual void remove_theater(int theater_id) = 0;
  virtual std::shared_ptr<ITheater> get_theater(int theater_id) const = 0;
  virtual std::vector<std::shared_ptr<ITheater>> get_all_theaters() const = 0;
  virtual std::vector<std::shared_ptr<ITheater>> get_theaters_showing_movie(int movie_id) const = 0;
  virtual bool theater_exists(int theater_id) const = 0;
  
  virtual std::vector<std::string> get_available_seats(int theater_id, int movie_id) const = 0;
  virtual bool book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) = 0;
};
