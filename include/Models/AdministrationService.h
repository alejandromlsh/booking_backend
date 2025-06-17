/**
 * @file AdministrationService.h
 * @brief Service responsible for system administration operations
 */

#pragma once
#include "Interfaces/IAdministrationService.h"
#include "Interfaces/IDataStore.h"
#include "Interfaces/ITheater.h"
#include "Models/Movie.h"
#include <memory>
#include <vector>

/**
 * @class AdministrationService
 * @brief Service responsible for system administration operations
 * @details Handles all administrative tasks including movie management,
 *          theater management, and system configuration. Uses dependency
 *          injection for loose coupling. Implements the IAdministrationService interface.
 */
class AdministrationService : public IAdministrationService {
public:
  explicit AdministrationService(std::shared_ptr<IDataStore> data_store);
  
  void add_movie(const Movie& movie) override;
  void remove_movie(int movie_id) override;
  std::vector<Movie> get_all_movies() const override;
  bool movie_exists(int movie_id) const override;
  
  void add_theater(std::shared_ptr<ITheater> theater) override;
  void remove_theater(int theater_id) override;
  std::vector<std::shared_ptr<ITheater>> get_all_theaters() const override;
  bool theater_exists(int theater_id) const override;
  
  void schedule_movie_in_theater(int theater_id, const Movie& movie) override;
  void remove_movie_from_theater(int theater_id, int movie_id) override;
  void set_theater_capacity(int theater_id, int capacity) override;

private:
  std::shared_ptr<IDataStore> data_store_;
};
