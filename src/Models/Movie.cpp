#include "Models/Movie.h"

Movie::Movie(int id,std::string name) : id_(id), name_(std::move(name)) {}

int Movie::get_id() const { return id_; }

std::string Movie::get_name() const { return name_; }