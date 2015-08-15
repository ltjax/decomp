#ifndef LIB_DECOMP_CONVEX_DECOMPOSITION
#define LIB_DECOMP_CONVEX_DECOMPOSITION

#include "triangulation.hpp"

namespace decomp {

struct HalfEdge
{
	std::uint16_t	vertex;
	HalfEdge*		partner;
	HalfEdge*		next;
};
std::vector<HalfEdge> buildHalfEdgeGraph(std::vector<std::uint16_t> const& triangleList);

//std::vector<std::vector<std::uint16_t>> hertelMehlhorn(std::vector<Point> const& pointList,
//										std::vector<std::uint16_t> const& triangleList);
}

#endif
