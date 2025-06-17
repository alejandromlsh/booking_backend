#include "Models/Theater.h"
#include "Models/Seat.h"
#include <cmath>

Theater::Theater(int id,std::string name) : id_(id), name_(std::move(name)) {}

void Theater::add_movie(Movie&& movie) {
  std::scoped_lock lock(mtx_);
  movies_.push_back(std::move(movie));
  initialize_seats(movie.get_id(),seat_count_);
}

void Theater::initialize_seats(int movie_id, int seat_count) {
    auto& seats = seats_per_movie_[movie_id];
    
    // Calculate rows and seats per row for a squared layout
    seats_per_row = ceil(sqrt(seat_count)); //explicit conversion for clarity
    int num_rows = (seat_count + seats_per_row - 1) / seats_per_row;  // Ceiling division
    
    for (int row = 0; row < num_rows; ++row) {
        char row_letter = 'a' + row;  // a, b, c, d, e...
        
        int seats_in_this_row = std::min(seats_per_row, seat_count - (row * seats_per_row));
        for (int seat = 1; seat <= seats_in_this_row; ++seat) {
            std::string seat_id = std::string(1, row_letter) + std::to_string(seat);
            seats.emplace(seat_id, std::make_shared<Seat>(seat_id));
        }
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