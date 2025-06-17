/**
 * @file Seat.h
 * @brief Basic seat implementation for movie theaters
 */

#pragma once
#include "Interfaces/ISeat.h"
#include <string>
#include <atomic>

/**
 * @class Seat
 * @brief Basic seat implementation with atomic booking mechanism
 * @details Provides thread-safe seat booking using atomic operations.
 *          Implements the ISeat interface for polymorphic behavior.
 */
class Seat : public ISeat {
public:
  explicit Seat(const std::string& id);
  
  bool is_available() const override;
  bool book() override;
  std::string get_id() const override;

private:
  std::string id_;
  std::atomic<bool> booked_;
};
