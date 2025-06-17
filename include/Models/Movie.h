#pragma once
#include <string>

class Movie {
public:
  Movie() = default; //it is needed by the unordered maps
  Movie(const Movie& other) = default;
  // Pass by value and then use std::move(). The name of movie is 10-50 char so bigger than Small string
  // Optimization
  Movie(int id,std::string name);
  int get_id() const;
  std::string get_name() const;
private:
  int id_;
  std::string name_;
};
