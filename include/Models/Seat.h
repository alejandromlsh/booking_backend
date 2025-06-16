#pragma once
#include <string>
#include <atomic>

class Seat {
public:
  Seat(const std::string& id);
  bool is_available() const;
  bool book();
  std::string get_id() const;


private:
 std::string id_;
 std::atomic<bool> booked_;
};
