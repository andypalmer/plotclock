#pragma once

class Point {
  double _x,_y;

  public:
  Point(double, double);
  double x();
  double y();
  double distance_to(Point);
};
