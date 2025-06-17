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
 * Enables polymorphic behavior for different seat categories (regular, VIP, etc.).
 * Provides atomic booking operations to ensure thread safety in concurrent environments.
 * All implementations must guarantee thread-safe booking operations.
 */
class ISeat {
public:
  /**
   * @brief Virtual destructor for proper polymorphic destruction
   */
  virtual ~ISeat() = default;

  /**
   * @brief Check if the seat is currently available for booking
   * @return true if seat is available, false if already booked
   * @note This operation may not be atomic and should be used for informational purposes only
   */
  virtual bool is_available() const = 0;

  /**
   * @brief Attempt to book the seat atomically
   * @return true if booking was successful, false if seat was already booked
   * @details This operation must be implemented as an atomic operation to prevent
   * race conditions in concurrent booking scenarios. Uses compare-and-swap semantics.
   */
  virtual bool book() = 0;

  /**
   * @brief Get the seat's unique identifier
   * @return Seat ID as string (e.g., "a1", "b2", "c3")
   */
  virtual std::string get_id() const = 0;
};
