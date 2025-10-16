#include <ql/dummy/dummy.h>
#include <gtest/gtest.h>

TEST(Dummt, Basic) {
    ASSERT_EQ(ql::Sum(1, 2), 3);
}
