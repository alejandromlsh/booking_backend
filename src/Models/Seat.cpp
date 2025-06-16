#include "Models/Seat.h"

Seat::Seat(const std::string& id) : id_(id), booked_(false) {}

bool Seat::is_available() const {return !booked_.load();} // Important to notice that this operation is not atomic, so it should be used only for informative usage by the user, not for operational check

bool Seat::book() {
  bool expected = false;
  return booked_.compare_exchange_strong(expected,true); // this step is done as an atomic step and therefore is safe.
}

std::string Seat::get_id() const {
  return id_;
}