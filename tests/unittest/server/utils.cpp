#include "gtest/gtest.h"
#include "utils.h"

using namespace zipfiles;
TEST(Utils, add) {
  EXPECT_EQ(3, add(1, 2));
  EXPECT_EQ(5, add(2, 3));
  EXPECT_EQ(7, add(3, 4));
}