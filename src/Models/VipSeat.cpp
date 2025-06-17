#include "Models/VipSeat.h"

VipSeat::VipSeat(std::string id, double premium_multiplier)
    : id_(std::move(id)), booked_(false), premium_multiplier_(premium_multiplier) {}

bool VipSeat::is_available() const {
    return !booked_.load();
}

bool VipSeat::book() {
    bool expected = false;
    return booked_.compare_exchange_strong(expected, true);
}

std::string VipSeat::get_id() const {
    return id_;
}

double VipSeat::get_premium_price() const {
    return base_price_ * premium_multiplier_;
}

double VipSeat::get_premium_multiplier() const {
    return premium_multiplier_;
}
