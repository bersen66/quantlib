#include <ql/pricing/curve.h>
#include "ql/calendar/holiday_storage.h"

namespace ql {

Curve::Curve(ql::HolidayStorage& hs) 
    : hs_(hs) 
{}

f64 Curve::InterpolatedRate(const std::string& jur, const DateType& date) const {
    
}

} // namespace ql