/**
 * @file Theater.h
 * @brief Standard theater implementation for movie showings
 */

#pragma once
#include "Interfaces/ITheater.h"
#include "Interfaces/ISeat.h"
#include "Models/Movie.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <map>

/**
 * @class Theater
 * @brief Standard theater implementation with seat management
 * @details Manages movie scheduling and seat booking for a theater.
 *          Provides thread-safe operations for concurrent booking requests.
 *          Implements the ITheater interface for polymorphic behavior.
 */
class Theater : public ITheater {
public:
  // Name of theater could be bigger than SSO, so pass-by-balue and move is preferred.
  Theater(int id,std::string name);
  
  void add_movie(Movie&& movie) override;
  std::vector<std::string> get_available_seats(int movie_id) const override;
  bool book_seats(int movie_id, const std::vector<std::string>& seat_ids) override;
  int get_id() const override;
  std::string get_name() const override;
  bool shows_movie(int movie_id) const override;
  
  void initialize_seats(int movie_id, int seat_count = 20);

private:
  int id_;
  int seats_per_row;
  int seat_count_ = 20;
  std::string name_;
  std::vector<Movie> movies_;
  std::unordered_map<int, std::map<std::string, std::shared_ptr<ISeat>>> seats_per_movie_;
  //std::unordered_map<int, std::unordered_map<std::string, std::shared_ptr<Seat>>> seats_per_movie_;

  mutable std::mutex mtx_;
};











