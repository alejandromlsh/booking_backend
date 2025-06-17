/**
 * @file IAdministrationService.h
 * @brief Abstract interface for system administration operations
 */

#pragma once
#include <memory>
#include <vector>
#include "Models/Movie.h"

// Forward declaration
class ITheater;

/**
 * @interface IAdministrationService
 * @brief Abstract interface for system administration operations
 * @details Defines the contract for all administration service implementations.
 *          Handles all administrative tasks including movie management,
 *          theater management, and system configuration.
 */
class IAdministrationService {
public:
  virtual ~IAdministrationService() = default;
  
  virtual void add_movie(const Movie& movie) = 0;
  virtual void remove_movie(int movie_id) = 0;
  virtual std::vector<Movie> get_all_movies() const = 0;
  virtual bool movie_exists(int movie_id) const = 0;
  
  virtual void add_theater(std::shared_ptr<ITheater> theater) = 0;
  virtual void remove_theater(int theater_id) = 0;
  virtual std::vector<std::shared_ptr<ITheater>> get_all_theaters() const = 0;
  virtual bool theater_exists(int theater_id) const = 0;
  
  virtual void schedule_movie_in_theater(int theater_id, const Movie& movie) = 0;
  virtual void remove_movie_from_theater(int theater_id, int movie_id) = 0;
  virtual void set_theater_capacity(int theater_id, int capacity) = 0;
};
