
#include "../triangulation.hpp"

using namespace triangulation;

int main()
{
	std::vector<Point> pointList = {
		{-2, -2}, {2, -2}, {2, 2}, {-2, 2},
		{-1, -1}, {1, -1}, {1, 1}, {-1, 1}
	};
	
	std::vector<std::uint16_t> outerPolygon = {0, 1, 2, 3};
	std::vector<std::uint16_t> innerPolygon = {7, 6, 5, 4};
	
	auto mergedPolygon = removeHoles(pointList, outerPolygon, {innerPolygon});
	
	return 0;
}