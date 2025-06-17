#include "Models/CentralDataStore.h"
#include <algorithm>
#include <mutex>

void CentralDataStore::add_movie(Movie&& movie) {
  std::unique_lock<std::shared_mutex> lock(data_mutex_);
  movies_.insert_or_assign(movie.get_id(), std::move(movie));
}

void CentralDataStore::remove_movie(int movie_id) {
  std::unique_lock<std::shared_mutex> lock(data_mutex_);
  movies_.erase(movie_id);
}

Movie CentralDataStore::get_movie(int movie_id) const {
  std::shared_lock<std::shared_mutex> lock(data_mutex_);
  auto it = movies_.find(movie_id);
  if (it != movies_.end()) {
    return it->second;
  }
  throw std::runtime_error("Movie not found: " + std::to_string(movie_id));
}

std::vector<Movie> CentralDataStore::get_all_movies() const {
  std::shared_lock<std::shared_mutex> lock(data_mutex_);
  std::vector<Movie> result;
  result.reserve(movies_.size());
  std::vector<std::pair<int, Movie>> sorted_movies;
  for (const auto& pair : movies_) {
    sorted_movies.push_back(pair);
  }
  std::sort(sorted_movies.begin(), sorted_movies.end(),
    [](const auto& a, const auto& b) { return a.first < b.first; });
  for (const auto& pair : sorted_movies) {
    result.push_back(pair.second);
  }
  return result;
}

bool CentralDataStore::movie_exists(int movie_id) const {
  std::shared_lock<std::shared_mutex> lock(data_mutex_);
  return movies_.find(movie_id) != movies_.end();
}

void CentralDataStore::add_theater(std::shared_ptr<ITheater> theater) {
  std::unique_lock<std::shared_mutex> lock(data_mutex_);
  theaters_[theater->get_id()] = theater;
}

void CentralDataStore::remove_theater(int theater_id) {
  std::unique_lock<std::shared_mutex> lock(data_mutex_);
  theaters_.erase(theater_id);
}

std::shared_ptr<ITheater> CentralDataStore::get_theater(int theater_id) const {
  std::shared_lock<std::shared_mutex> lock(data_mutex_);
  auto it = theaters_.find(theater_id);
  if (it != theaters_.end()) {
    return it->second;
  }
  return nullptr;
}

std::vector<std::shared_ptr<ITheater>> CentralDataStore::get_all_theaters() const {
  std::shared_lock<std::shared_mutex> lock(data_mutex_);
  std::vector<std::shared_ptr<ITheater>> result;
  result.reserve(theaters_.size());
  for (const auto& pair : theaters_) {
    result.push_back(pair.second);
  }
  return result;
}

std::vector<std::shared_ptr<ITheater>> CentralDataStore::get_theaters_showing_movie(int movie_id) const {
  std::shared_lock<std::shared_mutex> lock(data_mutex_);
  std::vector<std::shared_ptr<ITheater>> result;
  for (const auto& pair : theaters_) {
    if (pair.second->shows_movie(movie_id)) {
      result.push_back(pair.second);
    }
  }
  return result;
}

bool CentralDataStore::theater_exists(int theater_id) const {
  std::shared_lock<std::shared_mutex> lock(data_mutex_);
  return theaters_.find(theater_id) != theaters_.end();
}

std::vector<std::string> CentralDataStore::get_available_seats(int theater_id, int movie_id) const {
  std::shared_lock<std::shared_mutex> lock(data_mutex_);
  auto theater = get_theater(theater_id);
  if (theater) {
    return theater->get_available_seats(movie_id);
  }
  return {};
}

bool CentralDataStore::book_seats(int theater_id, int movie_id, const std::vector<std::string>& seat_ids) {
  std::shared_lock<std::shared_mutex> lock(data_mutex_);
  auto theater = get_theater(theater_id);
  if (theater) {
    return theater->book_seats(movie_id, seat_ids);
  }
  return false;
}
