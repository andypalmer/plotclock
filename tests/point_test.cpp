#include "CppUTest/TestHarness.h"
#include "point.hpp"

TEST_GROUP(Point) {
};

TEST(Point, has_coordinates) {
  Point point = Point(3,4);
  LONGS_EQUAL(point.x(), 3);
  LONGS_EQUAL(point.y(), 4);
}
