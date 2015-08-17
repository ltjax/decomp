
#include "../convex_decomposition.hpp"

using namespace decomp;

#define TEST_ASSERT(C) if (!(C)) return EXIT_FAILURE

int main()
{
	std::vector<Point> pointList={
		{-1, 0}, {0, 0}, {0, -1}, {1, -1}, {1, 1}, {-1, 1}
	};

	std::vector<std::uint16_t> triangleList={1, 4, 5, 1, 3, 4, 0, 1, 5, 1, 2, 3};

	auto decomposed = hertelMehlhorn(pointList, triangleList);

	return EXIT_SUCCESS;
}