
#include "triangulation.hpp"
#include <cassert>
#include <algorithm>
#include <cmath>

using namespace triangulation;

namespace {

std::uint16_t findRightmostPoint(std::vector<Point> const& pointList, std::vector<std::uint16_t> const& polygon)
{
	assert(!polygon.empty());
	
	return std::max_element(polygon.begin(), polygon.end(), [&](std::uint16_t lhs, std::uint16_t rhs)
	{
		return pointList[lhs].x() < pointList[rhs].x();
	})-polygon.begin();
}

inline double determinant(Point const& lhs, Point const& rhs)
{
	return lhs[0]*rhs[1]-lhs[1]*rhs[0];
}

bool isCounterClockwise(Point const& a, Point const& b, Point const& c)
{
	return determinant(b-a, c-a)>0.0;
}

bool inCone(Point const& a, Point const& b, Point const& c, Point const& p)
{
	if (isCounterClockwise(a, b, c))
		return isCounterClockwise(a, b, p) && isCounterClockwise(b, c, p);
	else
		return isCounterClockwise(a, b, p) || isCounterClockwise(b, c, p);
}

inline double squared(Point const& p)
{
	return p[0]*p[0]+p[1]*p[1];
}

Point normalize(Point const& p)
{
	double length = std::sqrt(squared(p));
	return {p.x()/length, p.y()/length};
}

bool segmentsIntersect(Point const& a, Point const& b, Point const& c, Point const& d)
{
	auto directionA = b-a;
	auto directionB = c-d; // direction purposefully reversed
	
	auto delta = c-a;
	
	auto denominator = determinant(directionA, directionB);
	
	if (denominator==0.0)
		return false;
	
	auto lambda = (directionB[1]*delta[0]-directionB[0]*delta[1])/denominator;
	auto mu = (directionA[0]*delta[1]-directionA[1]*delta[0])/denominator;
	
	if (lambda < 0.0 || lambda > 1.0)
		return false;
	
	if (mu < 0.0 || mu > 1.0)
		return false;
	
	return true;
}

bool pointVisibleFrom(std::vector<Point> const& pointList, std::vector<std::uint16_t> const& indexList, int relativeIndex, Point const& from)
{
	int N=indexList.size();
	auto target=pointList[indexList[relativeIndex]];
	
	for (int i=0; i< N; ++i)
	{
		auto next=(i+1)%N;
		
		// Dont test edges connected to the relativ index
		if (i==relativeIndex||next==relativeIndex)
			continue;
		
		// Do we have an actual blocker?
		if (segmentsIntersect(from, target, pointList[indexList[i]], pointList[indexList[next]]))
			return false;
		
	}
	
	return true;
}

double dot(Point const& lhs, Point const& rhs)
{
	return lhs[0]*rhs[0]+lhs[1]*rhs[1];
}

}

std::vector<std::uint16_t> triangulation::removeHoles(std::vector<Point> const& pointList,
	std::vector<std::uint16_t> indexList, std::vector<std::vector<std::uint16_t>> holeList)
{
	while (!holeList.empty())
	{
		// Find the rightmost hole point
		auto rightmostHole = holeList.end();
		std::uint16_t rightmostPoint =0;
		
		for (auto i=holeList.begin(); i!=holeList.end(); ++i)
		{
			int localRightmost = findRightmostPoint(pointList, *i);
			
			if (rightmostHole==holeList.end()||
				pointList[(*rightmostHole)[rightmostPoint]].x() < pointList[(*i)[localRightmost]].x())
			{
				rightmostHole=i;
				rightmostPoint=localRightmost;
			}
		}
		
		std::vector<std::uint16_t> hole = std::move(*rightmostHole);
		holeList.erase(rightmostHole);
		Point holePoint = pointList[hole[rightmostPoint]];
		Point bestDirection = normalize(normalize(holePoint-pointList[hole[(rightmostPoint+1)%hole.size()]])+
			normalize(holePoint-pointList[hole[(rightmostPoint+hole.size()-1)%hole.size()]]));
		
		// Find a point to connect that to
		auto getPoint=[&](int i)
		{
			return pointList[indexList[i]];
		};
		
		int bestPoint=-1;
		int N=indexList.size();
		for (int i=0; i<N; ++i)
		{
			if (pointList[indexList[i]].x() <= holePoint.x())
				continue;
			
			int const next=(i+1)%N;
			int const previous=(i+N-1)%N;
			
			auto const& rimPoint=getPoint(i);
			
			if (!inCone(getPoint(previous), rimPoint, getPoint(next), holePoint))
			{
				continue;
			}
			
			if (bestPoint >= 0 && dot(normalize(rimPoint-holePoint), bestDirection) <
								  dot(normalize(getPoint(bestPoint)-holePoint), bestDirection))
			{
				continue;
			}
			
			if (!pointVisibleFrom(pointList, indexList, i, holePoint))
			{
				continue;
			}
			
			bestPoint=i;
		}
		
		assert(bestPoint!=-1);
		
		std::rotate(indexList.begin(), indexList.begin()+bestPoint, indexList.end());
		indexList.push_back(indexList.front());
		indexList.insert(indexList.end(), hole.begin()+rightmostPoint, hole.end());
		indexList.insert(indexList.end(), hole.begin(), hole.begin()+rightmostPoint+1);
	}
	
	return indexList;
}