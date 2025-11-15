#pragma once

namespace ql {

enum class Freq {
    kAnnualy,      // Once a year
    kSemiAnnualy,  // Twice a year
    kQuarterly,    // Three times a year
    kMonthly,      // Every month
    kDaily,        // Every day
};

enum class Adjustment {
    kFollowing,          // Следующий рабочий день
    kPreceding,          // Предыдущий рабочий день
    kModifiedFollowing,  // Модифицированный следующий рабочий день
    kUnadjusted          // Без подстройки (используется для генератора)
};

}  // namespace ql