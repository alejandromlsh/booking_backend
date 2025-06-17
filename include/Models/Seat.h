#pragma once
#include <string>
#include <atomic>

class ISeat {
public:
  virtual ~ISeat() = default;
  
  virtual bool is_available() const = 0;
  virtual bool book() = 0;
  virtual std::string get_id() const = 0;
  virtual double get_price() const = 0;
  virtual std::string get_type() const = 0;
    
};

class Seat :ISeat {
public:
  Seat(const std::string& id);
  bool is_available() const;
  bool book();
  std::string get_id() const;


private:
 std::string id_;
 std::atomic<bool> booked_;
};
