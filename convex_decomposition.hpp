#ifndef LIB_DECOMP_CONVEX_DECOMPOSITION
#define LIB_DECOMP_CONVEX_DECOMPOSITION

#include "triangulation.hpp"
#include <memory>

namespace decomp {

struct HalfEdge
{
	std::uint16_t	vertex;
	HalfEdge*		partner;
	HalfEdge*		next;
};


std::vector<std::unique_ptr<HalfEdge>> buildHalfEdgeGraph(IndexList const& triangleList);

std::vector<IndexList> hertelMehlhorn(PointList const& pointList,
									  IndexList const& triangleList);

std::vector<IndexList> decompose(PointList const& pointList,
								 IndexList simplePolygon,
								 std::vector<IndexList> holeList={});

}

#endif
