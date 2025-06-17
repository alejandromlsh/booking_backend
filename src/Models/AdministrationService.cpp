#include "Models/AdministrationService.h"
#include <stdexcept>

AdministrationService::AdministrationService(std::shared_ptr<CentralDataStore> data_store)
  : data_store_(data_store) {
  if (!data_store_) {
    throw std::invalid_argument("CentralDataStore cannot be null");
  }
}

void AdministrationService::add_movie(const Movie& movie) {
  std::scoped_lock<std::mutex> lock(admin_mutex_);
  data_store_->add_movie(movie);
}

void AdministrationService::remove_movie(int movie_id) {
  std::scoped_lock<std::mutex> lock(admin_mutex_);
  data_store_->remove_movie(movie_id);
}

std::vector<Movie> AdministrationService::get_all_movies() const {
  std::scoped_lock<std::mutex> lock(admin_mutex_);
  return data_store_->get_all_movies();
}

bool AdministrationService::movie_exists(int movie_id) const {
  std::scoped_lock<std::mutex> lock(admin_mutex_);
  return data_store_->movie_exists(movie_id);
}

void AdministrationService::add_theater(const std::shared_ptr<Theater>& theater) {
  std::scoped_lock<std::mutex> lock(admin_mutex_);
  data_store_->add_theater(theater);
}

void AdministrationService::remove_theater(int theater_id) {
  std::scoped_lock<std::mutex> lock(admin_mutex_);
  data_store_->remove_theater(theater_id);
}

std::vector<std::shared_ptr<Theater>> AdministrationService::get_all_theaters() const {
  std::scoped_lock<std::mutex> lock(admin_mutex_);
  return data_store_->get_all_theaters();
}

bool AdministrationService::theater_exists(int theater_id) const {
  std::scoped_lock<std::mutex> lock(admin_mutex_);
  return data_store_->theater_exists(theater_id);
}

void AdministrationService::schedule_movie_in_theater(int theater_id, const Movie& movie) {
  std::scoped_lock<std::mutex> lock(admin_mutex_);
  auto theater = data_store_->get_theater(theater_id);
  if (theater) {
    theater->add_movie(movie);
  } else {
    throw std::runtime_error("Theater not found: " + std::to_string(theater_id));
  }
}

void AdministrationService::remove_movie_from_theater(int theater_id, int movie_id) {
  std::scoped_lock<std::mutex> lock(admin_mutex_);
  // This would require adding a remove_movie method to Theater class, to be developed
  throw std::runtime_error("Feature not yet implemented");
}

void AdministrationService::set_theater_capacity(int theater_id, int capacity) {
  std::scoped_lock<std::mutex> lock(admin_mutex_);
  // This would require adding capacity management to Theater class, to be developed
  throw std::runtime_error("Feature not yet implemented");
}
