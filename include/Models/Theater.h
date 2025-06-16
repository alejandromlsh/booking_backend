#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "Seat.h"
#include "Movie.h"

class Theater {
public:
  Theater(int id, const std::string & name);

  void add_movie(const Movie & movie);
  void initialize_seats(int movie_id,int seat_count = 20);

  std::vector<std::string> get_available_seats(int movie_id) const;
  bool book_seats(int movie_id,const std::vector<std::string>& seat_ids);
  int get_id() const;
  std::string get_name() const;
  bool shows_movie(int movie_id) const;

private:
  int id_;
  int seat_count_ = 20;
  std::string name_;
  std::vector<Movie> movies_;
  std::unordered_map<int, std::unordered_map<std::string, std::shared_ptr<Seat>>> seats_per_movie_;
  mutable std::mutex mtx_;


};