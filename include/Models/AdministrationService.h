#pragma once

#include <memory>
#include <mutex>
#include "Movie.h"
#include "Theater.h"
#include "CentralDataStore.h"

/**
 * @class AdministrationService
 * @brief Service responsible for system administration operations
 * @details Handles all administrative tasks including movie management,
 *          theater management, and system configuration. Separated from
 *          booking operations to follow Single Responsibility Principle.
 */
class AdministrationService {
public:
  explicit AdministrationService(std::shared_ptr<CentralDataStore> data_store);

  // Movie management
  void add_movie(const Movie& movie);
  void remove_movie(int movie_id);
  std::vector<Movie> get_all_movies() const;
  bool movie_exists(int movie_id) const;

  // Theater management
  void add_theater(const std::shared_ptr<Theater>& theater);
  void remove_theater(int theater_id);
  std::vector<std::shared_ptr<Theater>> get_all_theaters() const;
  bool theater_exists(int theater_id) const;

  // Movie-Theater associations
  void schedule_movie_in_theater(int theater_id, const Movie& movie);
  void remove_movie_from_theater(int theater_id, int movie_id);

  // System configuration
  void set_theater_capacity(int theater_id, int capacity);

private:
  std::shared_ptr<CentralDataStore> data_store_;
  mutable std::mutex admin_mutex_;
};
