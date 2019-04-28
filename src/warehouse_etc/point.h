#pragma once

namespace amazoom {
class Point {
public:
	Point(const int x, const int y, const int z);
	Point();
	~Point();

	const int X;
	const int Y;
	const int Z;
};
}