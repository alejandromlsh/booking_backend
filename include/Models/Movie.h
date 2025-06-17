#pragma once
#include <string>

class Movie {
public:
  Movie() = default; //it is needed by the unordered maps
  Movie(int id,const std::string& name);
  int get_id() const;
  std::string get_name() const;
private:
  int id_;
  std::string name_;
};
