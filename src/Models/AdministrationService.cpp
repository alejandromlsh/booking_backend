#include "Models/AdministrationService.h"
#include <stdexcept>

AdministrationService::AdministrationService(std::shared_ptr<IDataStore> data_store)
  : data_store_(data_store) {
  if (!data_store_) {
    throw std::invalid_argument("DataStore cannot be null");
  }
}

void AdministrationService::add_movie(Movie&& movie) {
  data_store_->add_movie(std::move(movie));
}

void AdministrationService::remove_movie(int movie_id) {
  data_store_->remove_movie(movie_id);
}

std::vector<Movie> AdministrationService::get_all_movies() const {
  return data_store_->get_all_movies();
}

bool AdministrationService::movie_exists(int movie_id) const {
  return data_store_->movie_exists(movie_id);
}

void AdministrationService::add_theater(std::shared_ptr<ITheater> theater) {
  data_store_->add_theater(theater);
}

void AdministrationService::remove_theater(int theater_id) {
  data_store_->remove_theater(theater_id);
}

std::vector<std::shared_ptr<ITheater>> AdministrationService::get_all_theaters() const {
  return data_store_->get_all_theaters();
}

bool AdministrationService::theater_exists(int theater_id) const {
  return data_store_->theater_exists(theater_id);
}

void AdministrationService::schedule_movie_in_theater(int theater_id, Movie&& movie) {
  auto theater = data_store_->get_theater(theater_id);
  if (theater) {
    theater->add_movie(std::move(movie));
  } else {
    throw std::runtime_error("Theater not found: " + std::to_string(theater_id));
  }
}

void AdministrationService::remove_movie_from_theater(int theater_id, int movie_id) {
  throw std::runtime_error("Feature not yet implemented");
}

void AdministrationService::set_theater_capacity(int theater_id, int capacity) {
  throw std::runtime_error("Feature not yet implemented");
}
