/**
 * @file ISeat.h
 * @brief Abstract interface for seat implementations in theaters
 */

#pragma once
#include <string>

/**
 * @interface ISeat
 * @brief Abstract interface for seat implementations in theaters
 * @details Defines the contract for all seat types in the movie booking system.
 *          Enables polymorphic behavior for different seat categories.
 *          Provides atomic booking operations and pricing information.
 */
class ISeat {
public:
  virtual ~ISeat() = default;
  
  virtual bool is_available() const = 0;
  virtual bool book() = 0;
  virtual std::string get_id() const = 0;
};
