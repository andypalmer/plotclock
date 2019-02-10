#include "CppUTest/TestHarness.h"
#include "point.hpp"

TEST_GROUP(Point) {
};

TEST(Point, has_coordinates) {
  Point point = Point(3,4);
  LONGS_EQUAL(point.x(), 3);
  LONGS_EQUAL(point.y(), 4);
}

TEST(Point, knows_distance_from_another_point) {
  Point origin = Point(0,0);
  Point target = Point(3,4);
  LONGS_EQUAL(origin.distance_to(target), 5);
}
