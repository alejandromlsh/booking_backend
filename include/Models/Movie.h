/**
 * @file Movie.h
 * @brief Movie entity representing a film in the booking system
 */

#pragma once

#include <string>

/**
 * @class Movie
 * @brief Represents a movie entity in the booking system
 * @details Simple data class containing movie information.
 * Uses move semantics for efficient string handling.
 */
class Movie {
public:
  /**
   * @brief Default constructor required by unordered_map containers
   */
  Movie() = default;

  /**
   * @brief Default copy constructor
   */
  Movie(const Movie& other) = default;

  /**
   * @brief Construct movie with ID and name
   * @param id Unique identifier for the movie
   * @param name Movie title (passed by value for move optimization)
   */
  Movie(int id, std::string name);

  /**
   * @brief Get the movie's unique identifier
   * @return Movie ID
   */
  int get_id() const;

  /**
   * @brief Get the movie's title
   * @return Movie name
   */
  std::string get_name() const;

private:
  int id_;        ///< Unique movie identifier
  std::string name_;  ///< Movie title
};
