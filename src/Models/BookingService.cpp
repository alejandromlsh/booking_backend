#include "Models/BookingService.h"



void BookingService::add_theater(const std::shared_ptr<Theater>&theater) {
  std::scoped_lock(mtx_);
  theaters_.push_back(theater);
}

void BookingService::add_movie(const Movie& movie) {
  std::scoped_lock(mtx_);
  movies_.push_back(movie);
}


std::vector<Movie> BookingService::get_all_movies() const {
  std::scoped_lock(mtx_);
  return movies_;
}

std::vector<std::shared_ptr<Theater>> BookingService::get_theaters_showing_movie(int movie_id) const {
  std::scoped_lock(mtx_);
  std::vector<std::shared_ptr<Theater>> result;
  for (const auto & theater :theaters_) {
    if (theater->shows_movie(movie_id)) {
      result.push_back(theater);
    }
  }
  return result;
}

std::vector<std::string> BookingService::get_available_seats(int theaterId, int movieId) const {
     std::scoped_lock(mtx_);
    for (const auto& theater : theaters_) {
        if (theater->get_id() == theaterId)
            return theater->get_available_seats(movieId);
    }
    return {};
}

bool BookingService::book_seats(int theaterId, int movieId, const std::vector<std::string>& seatIds) {
     std::scoped_lock(mtx_);
    for (const auto& theater : theaters_) {
        if (theater->get_id() == theaterId)
            return theater->book_seats(movieId, seatIds);
    }
    return false;
}
