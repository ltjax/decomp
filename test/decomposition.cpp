
#include "../convex_decomposition.hpp"

using namespace decomp;

#define TEST_ASSERT(C) if (!(C)) return EXIT_FAILURE

namespace {

// Internal angle is 180deg or smaller
bool isInternallyConvex(Point const& a, Point const& b, Point const& c)
{
	auto left=c-a;
	auto right=b-a;

	// This is the same as determinant >= 0
	return right[0]*left[1]>=right[1]*left[0];

}

bool isConvex(PointList const& pointList, IndexList const& polygon)
{
	auto N=static_cast<int>(polygon.size());
	for (int i=0; i<N; ++i)
	{
		int p=(i+N-1)%N;
		int n=(i+1)%N;

		if (!isInternallyConvex(pointList[polygon[p]], pointList[polygon[i]], pointList[polygon[n]]))
			return false;
	}

	return true;
}


bool allConvex(PointList const& pointList, std::vector<IndexList> const& polygonList)
{
	for (auto&& polygon:polygonList)
	{
		if (!isConvex(pointList, polygon))
			return false;
	}

	return true;
}

}

bool testHertelMehlhorn()
{
	std::vector<Point> pointList={
		{-1, 0}, {0, 0}, {0, -1}, {1, -1}, {1, 1}, {-1, 1}
	};

	std::vector<std::uint16_t> triangleList={1, 4, 5, 1, 3, 4, 0, 1, 5, 1, 2, 3};

	auto decomposed=hertelMehlhorn(pointList, triangleList);

	if (!allConvex(pointList, decomposed))
		return false;


	return true;
}

bool testCompleteDecomposition()
{
	std::vector<Point> pointList={
		{-4, 0}, {-3, -2}, {3, -2}, {4, 0}, {3, 2}, {-3, 2}, // points on the outer polygon
		{-3, 0}, {-2, -1}, {-1, 0}, {-2, 1}, // points on the left hole
		{1, 0}, {2, -1}, {3, 0}, {2, 1} // points on the right hole
	};

	std::vector<std::uint16_t> outerPolygon={
		0, 1, 2, 3, 4, 5
	};

	std::vector<std::vector<std::uint16_t>> holeList={
		{13, 12, 11, 10},
		{9, 8, 7, 6}
	};


	auto decomposed=decompose(pointList, outerPolygon, holeList);
	if (!allConvex(pointList, decomposed))
		return false;

	return true;
}

int main()
{
	if (!testHertelMehlhorn())
		return EXIT_FAILURE;

	if (!testCompleteDecomposition())
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}