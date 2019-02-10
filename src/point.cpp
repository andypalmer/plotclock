#include "point.hpp"

Point::Point(double x, double y): _x(x), _y(y) {}
double Point::x() { return _x; }
double Point::y() { return _y; }
