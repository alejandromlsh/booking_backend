/**
 * @file CentralDataStore.h
 * @brief Thread-safe central repository for all system data
 */

#pragma once
#include "Interfaces/IDataStore.h"
#include "Interfaces/ITheater.h"
#include "Models/Movie.h"
#include <unordered_map>
#include <shared_mutex>

/**
 * @class CentralDataStore
 * @brief Thread-safe central repository for all system data
 * @details Provides unified access to movies and theaters with proper thread safety.
 *          Uses shared_mutex to allow multiple readers or single writer access patterns.
 *          Implements the IDataStore interface for dependency injection.
 */
class CentralDataStore : public IDataStore {
public:
  CentralDataStore() = default;
  ~CentralDataStore() = default;
  
  void add_movie(Movie&& movie) override;
  void remove_movie(int movie_id) override;
  Movie get_movie(int movie_id) const override;
  std::vector<Movie> get_all_movies() const override;
  bool movie_exists(int movie_id) const override;
  
  void add_theater(std::shared_ptr<ITheater> theater) override;
  void remove_theater(int theater_id) override;
  std::shared_ptr<ITheater> get_theater(int theater_id) const override;
  std::vector<std::shared_ptr<ITheater>> get_all_theaters() const override;
  std::vector<std::shared_ptr<ITheater>> get_theaters_showing_movie(int movie_id) const override;
  bool theater_exists(int theater_id) const override;
  
  std::vector<std::string> get_available_seats(int theater_id, int movie_id) const override;
  bool book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) override;

private:
  mutable std::shared_mutex data_mutex_;
  std::unordered_map<int, Movie> movies_;
  std::unordered_map<int, std::shared_ptr<ITheater>> theaters_;
};
