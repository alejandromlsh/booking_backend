/**
 * @file SeatFactory.h
 * @brief Type-safe seat factory with perfect forwarding and compile-time validation
 */

#pragma once
#include "Interfaces/ISeat.h"
#include "Utils/SeatTypeTraits.h"
#include <memory>
#include <utility>
#include <type_traits>

/**
 * @class SeatFactory
 * @brief Factory class for creating type-safe seat objects with perfect forwarding
 * @details Provides compile-time type safety through SFINAE and type traits.
 *          Uses perfect forwarding to efficiently pass constructor arguments.
 *          Ensures only ISeat-derived types can be created.
 */
class SeatFactory {
public:
  /**
   * @brief Create a seat object with perfect forwarding and type safety
   * @tparam SeatType Type of seat to create (must derive from ISeat)
   * @tparam Args Types of constructor arguments
   * @param args Constructor arguments forwarded perfectly
   * @return Shared pointer to created seat as ISeat interface
   * @note This function is only available if SeatType derives from ISeat
   *       and is constructible with the provided arguments
   */
  template<typename SeatType, typename... Args>
  static auto create(Args&&... args) 
      -> std::enable_if_t<
          SeatTraits::is_constructible_seat_v<SeatType, Args...>,
          std::shared_ptr<ISeat>
      > {
      static_assert(SeatTraits::is_seat_type_v<SeatType>, 
                    "SeatType must derive from ISeat interface");
      static_assert(std::is_constructible_v<SeatType, Args...>,
                    "SeatType must be constructible with provided arguments");
      
      return std::make_shared<SeatType>(std::forward<Args>(args)...);
  }
  
  /**
   * @brief Create a seat with explicit type specification for clarity
   * @tparam SeatType Type of seat to create
   * @tparam Args Types of constructor arguments  
   * @param args Constructor arguments
   * @return Shared pointer to created seat
   */
  template<typename SeatType, typename... Args>
  static auto make_seat(Args&&... args)
      -> std::enable_if_t<
          SeatTraits::is_seat_type_v<SeatType>,
          std::shared_ptr<SeatType>
      > {
      return std::make_shared<SeatType>(std::forward<Args>(args)...);
  }
    
private:
  // Private constructor to prevent instantiation
  SeatFactory() = delete;
  ~SeatFactory() = delete;
  SeatFactory(const SeatFactory&) = delete;
  SeatFactory& operator=(const SeatFactory&) = delete;
};
