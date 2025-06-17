#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <shared_mutex>
#include <string>

#include "Movie.h"
#include "Theater.h"

/**
 * @class CentralDataStore
 * @brief Thread-safe central repository for all system data
 * @details Provides unified access to movies and theaters with proper thread safety.
 *          Uses shared_mutex to allow multiple readers or single writer access patterns.
 *          This class serves as the single source of truth for all system data.
 */
class CentralDataStore {
public:
  CentralDataStore() = default;
  ~CentralDataStore() = default;

  // Movie operations
  void add_movie(const Movie& movie);
  void remove_movie(int movie_id);
  Movie get_movie(int movie_id) const;
  std::vector<Movie> get_all_movies() const;
  bool movie_exists(int movie_id) const;

  // Theater operations
  void add_theater(const std::shared_ptr<Theater>& theater);
  void remove_theater(int theater_id);
  std::shared_ptr<Theater> get_theater(int theater_id) const;
  std::vector<std::shared_ptr<Theater>> get_all_theaters() const;
  std::vector<std::shared_ptr<Theater>> get_theaters_showing_movie(int movie_id) const;
  bool theater_exists(int theater_id) const;

  // Booking operations (delegated to theaters)
  std::vector<std::string> get_available_seats(int theater_id, int movie_id) const;
  bool book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids);

private:
  mutable std::shared_mutex data_mutex_;
  std::unordered_map<int, Movie> movies_;
  std::unordered_map<int, std::shared_ptr<Theater>> theaters_;
};
