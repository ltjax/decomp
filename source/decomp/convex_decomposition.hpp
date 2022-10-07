#ifndef LIB_DECOMP_CONVEX_DECOMPOSITION
#define LIB_DECOMP_CONVEX_DECOMPOSITION

#include "triangulation.hpp"
#include <memory>

namespace decomp
{

struct HalfEdge
{
    std::uint16_t vertex;
    HalfEdge* partner;
    HalfEdge* next;
    bool fixed;
};

using EdgeID = std::pair<std::uint16_t, std::uint16_t>;

std::vector<std::unique_ptr<HalfEdge>> buildHalfEdgeGraph(IndexList const& triangleList, std::vector<EdgeID> const& fixedEdges);

std::vector<IndexList> hertelMehlhorn(PointList const& pointList, IndexList const& triangleList, std::vector<EdgeID> const& fixedEdges);

void edgeFlip(PointList const& pointList, std::vector<std::unique_ptr<HalfEdge>> const& edges);

/** Decompose a given simple polygon with simple holes into a list of convex polygons.
    The outer polygon's vertex order needs to be counter-clockwise, while all holes need to be clockwise.
 */
std::vector<IndexList>
decompose(PointList const& pointList, IndexList simplePolygon, std::vector<IndexList> holeList = {}, std::vector<EdgeID> const& fixedEdges = {});
}

#endif
