#include "convex_decomposition.hpp"
#include <map>

namespace {

using EdgeID=std::pair<std::uint16_t, std::uint16_t>;

EdgeID getEdgeID(std::uint16_t a, std::uint16_t b)
{
	return (b<a) ? std::make_pair(b, a) : std::make_pair(a, b);
}

}

std::vector<decomp::HalfEdge> decomp::buildHalfEdgeGraph(std::vector<std::uint16_t> const& triangleList)
{
	if (triangleList.size()%3!=0)
	{
		throw std::runtime_error("Given triangle list does not have size divisible by 3");
	}

	using OpenEdgeMap=std::map<EdgeID, HalfEdge*>;
	std::vector<HalfEdge> halfEdgeList;
	OpenEdgeMap openEdgeList;
	halfEdgeList.resize(triangleList.size());
	int const N=triangleList.size();

	for (int i=0; (i+2)<N; i+=3)
	{
		for (int j=0; j<3; ++j)
		{
			// Indices of the current/next edge/vertex in this triangle
			auto a=i+j;
			auto b=i+(j+1)%3;

			halfEdgeList[a].vertex=triangleList[a];
			halfEdgeList[a].next=&halfEdgeList[b];

			// Check if we have an open partner for this edge
			auto edgeID=getEdgeID(triangleList[a], triangleList[b]);
			auto inserted=openEdgeList.insert({edgeID, &halfEdgeList[a]});
			if (!inserted.second)
			{
				// We do! - Join them and erase from the open edges
				inserted.first->second->partner=&halfEdgeList[a];
				halfEdgeList[a].partner=inserted.first->second;
				openEdgeList.erase(inserted.first);
			}
			else
			{
				halfEdgeList[a].partner=nullptr;
			}
		}
	}

	return halfEdgeList;
}

//std::vector<std::vector<std::uint16_t>> decomp::hertelMehlhorn(
//	std::vector<Point> const& pointList, std::vector<std::uint16_t> const& triangleList)
//{
//
//}
