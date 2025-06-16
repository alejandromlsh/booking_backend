#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include "Movie.h"
#include "Theater.h"


// Right nwt is a Singleton but better to modify it to dependency injection
class BookingService{
  public:
  BookingService() = default;
  void add_theater(const std::shared_ptr<Theater>& theater);
  void add_movie(const Movie& movie);

  std::vector<Movie> get_all_movies() const;
  std::vector<std::shared_ptr<Theater>> get_theaters_showing_movie(int movie_id) const;
  std::vector<std::string> get_available_seats(int theater_id, int movie_id) const;
  bool book_seats(int theater_id, int movie_id, const std::vector<std::string>&seats_ids);
private:
  mutable std::mutex mtx_;
  std::vector<std::shared_ptr<Theater>> theaters_;
  std::vector<Movie> movies_;
};