/**
 * @file VipSeat.h
 * @brief VIP seat implementation with premium features
 */

#pragma once
#include "Interfaces/ISeat.h"
#include <string>
#include <atomic>

/**
 * @class VipSeat
 * @brief Premium seat implementation with enhanced features
 * @details Extends basic seat functionality with premium pricing
 *          and additional amenities for VIP customers.
 */
class VipSeat : public ISeat {
public:
    explicit VipSeat(std::string id, double premium_multiplier = 2.5);
    
    bool is_available() const override;
    bool book() override;
    std::string get_id() const override;
    
    double get_premium_price() const;
    double get_premium_multiplier() const;

private:
    std::string id_;
    std::atomic<bool> booked_;
    double premium_multiplier_;
    static constexpr double base_price_ = 10.0;
};
