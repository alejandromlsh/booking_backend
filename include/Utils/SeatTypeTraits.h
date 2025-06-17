/**
 * @file SeatTypeTraits.h
 * @brief Type traits and compile-time validation for seat types
 */

#pragma once
#include "Interfaces/ISeat.h"
#include <type_traits>
#include <memory>

namespace SeatTraits {
  /**
   * @brief Type trait to check if a type is derived from ISeat
   * @tparam T Type to check
   */
  template<typename T>
  struct is_seat_type : std::is_base_of<ISeat, T> {};
  
  /**
   * @brief Helper variable template for is_seat_type
   * @tparam T Type to check
   */
  template<typename T>
  inline constexpr bool is_seat_type_v = is_seat_type<T>::value;
  
  /**
   * @brief Type trait to check if a type has a valid seat constructor
   * @tparam T Type to check
   * @tparam Args Constructor argument types
   */
  template<typename T, typename... Args>
  struct is_constructible_seat : std::conjunction<
      is_seat_type<T>,
      std::is_constructible<T, Args...>
  > {};
  
  /**
   * @brief Helper variable template for is_constructible_seat
   * @tparam T Type to check
   * @tparam Args Constructor argument types
   */
  template<typename T, typename... Args>
  inline constexpr bool is_constructible_seat_v = is_constructible_seat<T, Args...>::value;
  
  /**
   * @brief SFINAE helper for enabling seat creation
   * @tparam T Type to check
   * @tparam Args Constructor argument types
   */
  template<typename T, typename... Args>
  using enable_if_seat_t = std::enable_if_t<is_constructible_seat_v<T, Args...>>;
}
