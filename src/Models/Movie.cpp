#include "Models/Movie.h"

Movie::Movie(int id, const std::string& name) : id_(id), name_(name) {}

int Movie::get_id() const { return id_; }

std::string Movie::get_name() const { return name_; }