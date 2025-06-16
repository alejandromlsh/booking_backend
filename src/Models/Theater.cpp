#include "Models/Theater.h"

Theater::Theater(int id, const std::string& name) : id_(id), name_(name) {}

void Theater::add_movie(const Movie & movie) {
  std::scoped_lock lock(mtx_);
  movies_.push_back(movie);
  initialize_seats(movie.get_id(),seat_count_);
}

void Theater::initialize_seats(int movie_id, int seat_count) {
  auto & seats = seats_per_movie_[movie_id];
  for (int i = 1; i <= seat_count;++i) {
    std::string seat_id = "a" + std::to_string(i);
    seats.emplace(seat_id,std::make_shared<Seat>(seat_id));
  }
}

std::vector<std::string> Theater::get_available_seats(int movie_id) const {
  std::scoped_lock lock(mtx_);
  std::vector<std::string> currently_available;
  auto it = seats_per_movie_.find(movie_id);
  if (it != seats_per_movie_.end()) {
    for (const auto& seat_pair : it->second) {
      if (seat_pair.second->is_available()) {
        currently_available.push_back(seat_pair.first);
      }
    }
  }
  return currently_available;
}

bool Theater::book_seats(int movie_id, const std::vector<std::string>& seat_ids){
  std::scoped_lock lock(mtx_);
  auto & seats = seats_per_movie_[movie_id];
  for (const auto& seatId : seat_ids) {
    if (seats.find(seatId) == seats.end() || !seats[seatId]->is_available())
      return false;
  }
    for (const auto& seatId : seat_ids) {
        if (!seats[seatId]->book())
            return false;
    }
    return true;
}

int Theater::get_id() const {
  return id_;
}

std::string Theater::get_name() const {
  return name_;
}


bool Theater::shows_movie(int movie_id) const {
  for (const auto & m : movies_) {
    if (m.get_id() == movie_id) {
      return true;
    }
  }
  return false;
}