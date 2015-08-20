#include "convex_decomposition.hpp"
#include <map>
#include <set>
#include <cassert>

using namespace decomp;

namespace {

using EdgeID=std::pair<std::uint16_t, std::uint16_t>;

EdgeID getEdgeID(std::uint16_t a, std::uint16_t b)
{
	return (b<a) ? std::make_pair(b, a) : std::make_pair(a, b);
}

EdgeID getEdgeID(HalfEdge* edge)
{
	return getEdgeID(edge->vertex, edge->next->vertex);
}

// Internal angle is 180deg or smaller
bool isInternallyConvex(Point const& a, Point const& b, Point const& c)
{
	auto left=c-a;
	auto right=b-a;

	// This is the same as determinant >= 0
	return right[0]*left[1]>=right[1]*left[0];

}

bool isEdgeRemoveable(std::vector<Point> const& pointList, decomp::HalfEdge* edge)
{
	if (!edge->partner)
		return false;
	
	// This is the same as determinant >= 0
	return isInternallyConvex(pointList[edge->vertex], pointList[edge->partner->next->next->vertex], pointList[edge->next->next->vertex]);
}

template <class T>
inline bool contains(std::set<T> const& c, T const& e)
{
	return c.find(e)!=c.end();
}

inline HalfEdge* representative(HalfEdge* e)
{
	if (e->partner && e->vertex>e->partner->vertex)
		return e->partner;
	else
		return e;
}

HalfEdge* getUndeletedLeft(std::set<EdgeID> const& deletedEdgeSet, HalfEdge* edge)
{
	auto edge_right=edge;
	auto edge_left=edge->next->next;

	while (contains(deletedEdgeSet, getEdgeID(edge_left->vertex, edge_right->vertex)))
	{
		assert(edge_left->partner!=nullptr);
		edge_right=edge_left->partner;
		edge_left=edge_right->next->next;
	}

	return edge_left;
}

HalfEdge* getUndeletedRight(std::set<EdgeID> const& deletedEdgeSet, HalfEdge* edge)
{
	do
	{
		edge=edge->partner->next;

	} while (contains(deletedEdgeSet, getEdgeID(edge->vertex, edge->next->vertex)));

	return edge;

}

HalfEdge* pickEdgeToRemove(std::set<HalfEdge*>& removeableEdgeSet)
{
	auto edgeToRemove=*removeableEdgeSet.begin();
	removeableEdgeSet.erase(removeableEdgeSet.begin());

	return edgeToRemove;
}

void updateEdge(HalfEdge* edgeToRemove,
				std::set<HalfEdge*>& removableEdgeSet,
				std::set<EdgeID> const& deletedEdgeSet,
				std::vector<Point> const& pointList)
{
	auto left=getUndeletedLeft(deletedEdgeSet, edgeToRemove);
	auto right=getUndeletedRight(deletedEdgeSet, edgeToRemove);

	if (contains(removableEdgeSet, representative(left)))
	{
		// Check if this is still removable
		auto leftOfLeft=getUndeletedLeft(deletedEdgeSet, left->partner);

		if (leftOfLeft->partner==right||leftOfLeft->partner==edgeToRemove||
			!isInternallyConvex(pointList[edgeToRemove->vertex], pointList[right->next->vertex], pointList[leftOfLeft->vertex]))
		{
			removableEdgeSet.erase(representative(left));
		}
	}

	if (contains(removableEdgeSet, representative(right)))
	{
		auto rightOfRight=getUndeletedRight(deletedEdgeSet, right);
		if (rightOfRight->partner==left||rightOfRight==edgeToRemove||
			!isInternallyConvex(pointList[edgeToRemove->vertex], pointList[rightOfRight->next->vertex], pointList[left->vertex]))
		{
			removableEdgeSet.erase(representative(right));
		}
	}
}
std::set<HalfEdge*> getRemovableEdgeSet(std::vector<Point> const& pointList,
										std::vector<std::unique_ptr<HalfEdge>> const& graph)
{
	std::set<HalfEdge*> removableEdgeSet;

	for (auto&& edge:graph)
	{
		if (edge->vertex>edge->next->vertex)
			continue;

		if (isEdgeRemoveable(pointList, edge.get()))
		{
			removableEdgeSet.insert(edge.get());
		}
	}

	return removableEdgeSet;
}

std::vector<std::vector<std::uint16_t>> extractPolygonList(
	std::vector<std::unique_ptr<HalfEdge>> const& graph,
	std::set<EdgeID> const& deletedEdgeSet)
{
	std::vector<std::vector<std::uint16_t>> resultList;
	std::set<HalfEdge*> visited;

	for (auto&& edge:graph)
	{
		// Don't extract twice
		if (contains(visited, edge.get()))
			continue;

		// Don't extract deleted
		if (contains(deletedEdgeSet, getEdgeID(edge.get())))
			continue;

		std::vector<std::uint16_t> polygon;
		auto current=edge.get();
		do
		{
			visited.insert(current);
			polygon.push_back(current->vertex);

			current=current->next;
			while (contains(deletedEdgeSet, getEdgeID(current)))
				current=current->partner->next;

		} while (current!=edge.get());

		resultList.push_back(std::move(polygon));
	}

	return resultList;
}

std::set<EdgeID> deleteEdges(std::set<HalfEdge*> &removableEdgeSet, std::vector<Point> const& pointList)
{
	std::set<EdgeID> deletedEdgeSet;

	while (!removableEdgeSet.empty())
	{
		auto edgeToRemove=pickEdgeToRemove(removableEdgeSet);

		updateEdge(edgeToRemove, removableEdgeSet, deletedEdgeSet, pointList);
		updateEdge(edgeToRemove->partner, removableEdgeSet, deletedEdgeSet, pointList);

		deletedEdgeSet.insert(getEdgeID(edgeToRemove->vertex, edgeToRemove->next->vertex));
	}
	
	return deletedEdgeSet;
}

}

std::vector<std::unique_ptr<HalfEdge>> decomp::buildHalfEdgeGraph(std::vector<std::uint16_t> const& triangleList)
{
	if (triangleList.size()%3!=0)
	{
		throw std::runtime_error("Given triangle list does not have size divisible by 3");
	}

	using OpenEdgeMap=std::map<EdgeID, HalfEdge*>;
	std::vector<std::unique_ptr<decomp::HalfEdge>> halfEdgeList;
	OpenEdgeMap openEdgeList;
	halfEdgeList.resize(triangleList.size());

	int const N=triangleList.size();

	for (int i=0; i<N; ++i)
		halfEdgeList[i].reset(new HalfEdge);

	for (int i=0; (i+2)<N; i+=3)
	{
		for (int j=0; j<3; ++j)
		{
			// Indices of the current/next edge/vertex in this triangle
			auto a=i+j;
			auto b=i+(j+1)%3;

			halfEdgeList[a]->vertex=triangleList[a];
			halfEdgeList[a]->next=halfEdgeList[b].get();

			// Check if we have an open partner for this edge
			auto edgeID=getEdgeID(triangleList[a], triangleList[b]);
			auto inserted=openEdgeList.insert({edgeID, halfEdgeList[a].get()});
			if (!inserted.second)
			{
				// We do! - Join them and erase from the open edges
				inserted.first->second->partner=halfEdgeList[a].get();
				halfEdgeList[a]->partner=inserted.first->second;
				openEdgeList.erase(inserted.first);
			}
			else
			{
				halfEdgeList[a]->partner=nullptr;
			}
		}
	}

	return halfEdgeList;
}

std::vector<std::vector<std::uint16_t>> decomp::hertelMehlhorn(
	std::vector<Point> const& pointList, std::vector<std::uint16_t> const& triangleList)
{
	// Extract connectivity information
	auto graph=buildHalfEdgeGraph(triangleList);
	
	// Find out which edges are removable in general, i.e. which can be removed
	// without creating non-convex corners in a first step.
	auto removableEdgeSet=getRemovableEdgeSet(pointList, graph);

	// Figure out which edges to actually remove
	auto deletedEdgeSet=deleteEdges(removableEdgeSet, pointList);

	// Extract a list of polygons an return it
	return extractPolygonList(graph, deletedEdgeSet);
}
