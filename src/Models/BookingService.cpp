#include "Models/BookingService.h"
#include <stdexcept>
#include <algorithm>

BookingService::BookingService(std::shared_ptr<IDataStore> data_store)
  : data_store_(data_store) {
  if (!data_store_) {
    throw std::invalid_argument("DataStore cannot be null");
  }
}

std::vector<Movie> BookingService::get_all_movies() const {
  return data_store_->get_all_movies();
}

std::vector<std::shared_ptr<ITheater>> BookingService::get_theaters_showing_movie(int movie_id) const {
  return data_store_->get_theaters_showing_movie(movie_id);
}

std::vector<std::string> BookingService::get_available_seats(int theater_id, int movie_id) const {
  return data_store_->get_available_seats(theater_id, movie_id);
}

bool BookingService::book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) {
  return data_store_->book_seats(theater_id, movie_id, seat_ids);
}

bool BookingService::can_book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) const {
  auto available_seats = data_store_->get_available_seats(theater_id, movie_id);
  for (const auto& seat_id : seat_ids) {
    if (std::find(available_seats.begin(), available_seats.end(), seat_id) == available_seats.end()) {
      return false;
    }
  }
  return true;
}
