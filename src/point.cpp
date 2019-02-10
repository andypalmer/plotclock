#include "point.hpp"
#include <cmath>

Point::Point(double x, double y): _x(x), _y(y) {}
double Point::x() { return _x; }
double Point::y() { return _y; }
double Point::distance_to(Point target) { return sqrt((target._x - _x) * (target._x - _x) + (target._y - _y) * (target._y - _y)); }
