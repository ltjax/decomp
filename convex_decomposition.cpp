#include "convex_decomposition.hpp"
#include <algorithm>
#include <cassert>
#include <map>
#include <set>
#include <unordered_map>

using namespace decomp;

namespace
{

using EdgeID = std::pair<std::uint16_t, std::uint16_t>;

EdgeID getEdgeID(std::uint16_t a, std::uint16_t b)
{
    return (b < a) ? std::make_pair(b, a) : std::make_pair(a, b);
}

EdgeID getEdgeID(HalfEdge* edge)
{
    return getEdgeID(edge->vertex, edge->next->vertex);
}

class EdgePriorityQueue
{
public:
    EdgePriorityQueue() = default;

    bool empty()
    {
        return mQueue.empty();
    }

    HalfEdge* extract()
    {
        auto edge = mQueue.begin()->second;
        mQueue.erase(mQueue.begin());
        mReverse.erase(edge);
        return edge;
    }

    void insert(HalfEdge* edge, double priority)
    {
        auto inserted = mReverse.insert({ edge, mQueue.insert({ priority, edge }) });
        assert(inserted.second);
    }

    void update(HalfEdge* edge, double priority)
    {
        auto where(mReverse.find(edge));
        assert(where != mReverse.end());
        mQueue.erase(where->second);
        where->second = mQueue.insert({ priority, edge });
    }

    void erase(HalfEdge* edge)
    {
        auto where(mReverse.find(edge));
        assert(where != mReverse.end());
        mQueue.erase(where->second);
        mReverse.erase(where);
    }

    bool contains(HalfEdge* edge) const
    {
        return mReverse.find(edge) != mReverse.end();
    }

private:
    EdgePriorityQueue(EdgePriorityQueue const& rhs) = default;

    using MapType = std::multimap<double, HalfEdge*>;
    MapType mQueue;
    std::unordered_map<HalfEdge*, MapType::iterator> mReverse;
};

// Internal angle is 180deg or smaller
bool isInternallyConvex(Point const& a, Point const& b, Point const& c)
{
    auto left = c - a;
    auto right = b - a;

    // This is the same as determinant >= 0
    return right[0] * left[1] >= right[1] * left[0];
}

bool isEdgeRemoveable(PointList const& pointList, decomp::HalfEdge* edge)
{
    if (!edge->partner)
        return false;

    // This is the same as determinant >= 0
    return isInternallyConvex(pointList[edge->vertex], pointList[edge->partner->next->next->vertex],
                              pointList[edge->next->next->vertex]) &&
           isInternallyConvex(pointList[edge->partner->vertex], pointList[edge->next->next->vertex],
                              pointList[edge->partner->next->next->vertex]);
}

template <class T> inline bool contains(std::set<T> const& c, T const& e)
{
    return c.find(e) != c.end();
}

inline HalfEdge* representative(HalfEdge* e)
{
    if (e->partner && e->vertex > e->partner->vertex)
        return e->partner;
    else
        return e;
}

void flip(HalfEdge* e)
{
    auto f = e->partner;

    auto a = e->next;
    auto b = a->next;
    auto c = f->next;
    auto d = c->next;

    e->next = b;
    b->next = c;
    c->next = e;
    e->vertex = d->vertex;

    f->next = d;
    d->next = a;
    a->next = f;
    f->vertex = b->vertex;
}

bool flipImprovesAngle(PointList const& pointList, HalfEdge* edge)
{
    auto a = pointList[edge->next->vertex];
    auto b = pointList[edge->next->next->vertex];
    auto c = pointList[edge->vertex];
    auto d = pointList[edge->partner->next->next->vertex];

    auto oldAngle = std::max(minimumInteriorAngle(a, b, c), minimumInteriorAngle(a, c, d));
    auto newAngle = std::max(minimumInteriorAngle(a, b, d), minimumInteriorAngle(b, c, d));

    return oldAngle > newAngle;
}

HalfEdge* getUndeletedLeft(std::set<EdgeID> const& deletedEdgeSet, HalfEdge* edge)
{
    auto edge_right = edge;
    auto edge_left = edge->next->next;

    while (contains(deletedEdgeSet, getEdgeID(edge_left->vertex, edge_right->vertex)))
    {
        assert(edge_left->partner != nullptr);
        edge_right = edge_left->partner;
        edge_left = edge_right->next->next;
    }

    return edge_left;
}

HalfEdge* getUndeletedRight(std::set<EdgeID> const& deletedEdgeSet, HalfEdge* edge)
{
    do
    {
        edge = edge->partner->next;

    } while (contains(deletedEdgeSet, getEdgeID(edge->vertex, edge->next->vertex)));

    return edge;
}

double getSmallestAdjacentAngleOnHalfEdge(Point centerPoint, Point forwardPoint, Point leftPoint, Point rightPoint)
{
    auto forwardDirection = normalize(forwardPoint - centerPoint);

    return std::max(dot(normalize(leftPoint - centerPoint), forwardDirection),
                    dot(normalize(rightPoint - centerPoint), forwardDirection));
}

double getSmallestAdjacentAngleOnHalfEdge(HalfEdge* edge,
                                          std::set<EdgeID> const& deletedEdgeSet,
                                          std::vector<Point> const& pointList)
{
    auto leftEdge = getUndeletedLeft(deletedEdgeSet, edge);
    auto rightEdge = getUndeletedRight(deletedEdgeSet, edge);

    auto centerPoint = pointList[edge->vertex];
    auto forwardPoint = pointList[edge->next->vertex];
    auto leftPoint = pointList[leftEdge->vertex];
    auto rightPoint = pointList[rightEdge->next->vertex];

    return getSmallestAdjacentAngleOnHalfEdge(centerPoint, forwardPoint, leftPoint, rightPoint);
}

double getSmallestAdjacentAngleOnEdge(HalfEdge* edge,
                                      std::set<EdgeID> const& deletedEdgeSet,
                                      std::vector<Point> const& pointList)
{
    return std::max(getSmallestAdjacentAngleOnHalfEdge(edge, deletedEdgeSet, pointList),
                    getSmallestAdjacentAngleOnHalfEdge(edge->partner, deletedEdgeSet, pointList));
}

void updateEdge(HalfEdge* edgeToRemove,
                EdgePriorityQueue& priorityQueue,
                std::set<EdgeID> const& deletedEdgeSet,
                std::vector<Point> const& pointList)
{
    auto left = getUndeletedLeft(deletedEdgeSet, edgeToRemove);
    auto right = getUndeletedRight(deletedEdgeSet, edgeToRemove);

    if (priorityQueue.contains(representative(left)))
    {
        // Check if this is still removable
        auto leftOfLeft = getUndeletedLeft(deletedEdgeSet, left->partner);

        if (leftOfLeft->partner == right || leftOfLeft->partner == edgeToRemove ||
            !isInternallyConvex(pointList[edgeToRemove->vertex], pointList[right->next->vertex],
                                pointList[leftOfLeft->vertex]))
        {
            priorityQueue.erase(representative(left));
        }
        else
        {
            // Need to update the priority
            priorityQueue.update(representative(left), getSmallestAdjacentAngleOnEdge(left, deletedEdgeSet, pointList));
        }
    }

    if (priorityQueue.contains(representative(right)))
    {
        auto rightOfRight = getUndeletedRight(deletedEdgeSet, right);
        if (rightOfRight->partner == left || rightOfRight == edgeToRemove ||
            !isInternallyConvex(pointList[edgeToRemove->vertex], pointList[rightOfRight->next->vertex],
                                pointList[left->vertex]))
        {
            priorityQueue.erase(representative(right));
        }
        else
        {
            priorityQueue.update(representative(right),
                                 getSmallestAdjacentAngleOnEdge(right, deletedEdgeSet, pointList));
        }
    }
}

void getRemovableEdgeQueue(EdgePriorityQueue& priorityQueue,
                           std::vector<Point> const& pointList,
                           std::vector<std::unique_ptr<HalfEdge>> const& graph)
{
    for (auto&& edge : graph)
    {
        if (edge->vertex > edge->next->vertex)
            continue;

        if (isEdgeRemoveable(pointList, edge.get()))
        {
            priorityQueue.insert(edge.get(), getSmallestAdjacentAngleOnEdge(edge.get(), {}, pointList));
        }
    }
}

std::vector<std::vector<std::uint16_t>> extractPolygonList(std::vector<std::unique_ptr<HalfEdge>> const& graph,
                                                           std::set<EdgeID> const& deletedEdgeSet)
{
    std::vector<std::vector<std::uint16_t>> resultList;
    std::set<HalfEdge*> visited;

    for (auto&& edge : graph)
    {
        // Don't extract twice
        if (contains(visited, edge.get()))
            continue;

        // Don't extract deleted
        if (contains(deletedEdgeSet, getEdgeID(edge.get())))
            continue;

        std::vector<std::uint16_t> polygon;
        auto current = edge.get();
        do
        {
            visited.insert(current);
            polygon.push_back(current->vertex);

            current = current->next;
            while (contains(deletedEdgeSet, getEdgeID(current)))
                current = current->partner->next;

        } while (current != edge.get());

        resultList.push_back(std::move(polygon));
    }

    return resultList;
}

std::set<EdgeID> deleteEdges(EdgePriorityQueue& priorityQueue, std::vector<Point> const& pointList)
{
    std::set<EdgeID> deletedEdgeSet;

    while (!priorityQueue.empty())
    {
        auto edgeToRemove = priorityQueue.extract();

        deletedEdgeSet.insert(getEdgeID(edgeToRemove->vertex, edgeToRemove->next->vertex));

        updateEdge(edgeToRemove, priorityQueue, deletedEdgeSet, pointList);
        updateEdge(edgeToRemove->partner, priorityQueue, deletedEdgeSet, pointList);
    }

    return deletedEdgeSet;
}
}

void decomp::edgeFlip(PointList const& pointList, std::vector<std::unique_ptr<HalfEdge>> const& edges)
{
    std::vector<HalfEdge*> eligible;
    for (auto const& edge : edges)
    {
        if (!edge->partner)
            continue;

        if (edge.get() != representative(edge.get()))
            continue;
        eligible.push_back(edge.get());
    }

    for (int i = 0; i < edges.size(); ++i)
    {
        bool flipped = false;
        for (auto edge : eligible)
        {
            if (!isEdgeRemoveable(pointList, edge))
                continue;

            if (!flipImprovesAngle(pointList, edge))
                continue;

            flip(edge);
            flipped = true;
        }
        if (!flipped)
            break;
    }
}

std::vector<std::unique_ptr<HalfEdge>> decomp::buildHalfEdgeGraph(IndexList const& triangleList)
{
    if (triangleList.size() % 3 != 0)
    {
        throw std::runtime_error("Given triangle list does not have size divisible by 3");
    }

    using OpenEdgeMap = std::map<EdgeID, HalfEdge*>;
    std::vector<std::unique_ptr<decomp::HalfEdge>> halfEdgeList;
    OpenEdgeMap openEdgeList;
    halfEdgeList.resize(triangleList.size());

    int const N = triangleList.size();

    for (int i = 0; i < N; ++i)
        halfEdgeList[i].reset(new HalfEdge);

    for (int i = 0; (i + 2) < N; i += 3)
    {
        for (int j = 0; j < 3; ++j)
        {
            // Indices of the current/next edge/vertex in this triangle
            auto a = i + j;
            auto b = i + (j + 1) % 3;

            halfEdgeList[a]->vertex = triangleList[a];
            halfEdgeList[a]->next = halfEdgeList[b].get();

            // Check if we have an open partner for this edge
            auto edgeID = getEdgeID(triangleList[a], triangleList[b]);
            auto inserted = openEdgeList.insert({ edgeID, halfEdgeList[a].get() });
            if (!inserted.second)
            {
                // We do! - Join them and erase from the open edges
                inserted.first->second->partner = halfEdgeList[a].get();
                halfEdgeList[a]->partner = inserted.first->second;
                openEdgeList.erase(inserted.first);
            }
            else
            {
                halfEdgeList[a]->partner = nullptr;
            }
        }
    }

    return halfEdgeList;
}

std::vector<IndexList> decomp::hertelMehlhorn(PointList const& pointList, IndexList const& triangleList)
{
    // Extract connectivity information
    auto graph = buildHalfEdgeGraph(triangleList);

    // Refine the triangulation by flipping edges to increase the minimum interior angle
    edgeFlip(pointList, graph);

    // Find out which edges are removable in general, i.e. which can be removed
    // without creating non-convex corners in a first step.
    EdgePriorityQueue priorityQueue;
    getRemovableEdgeQueue(priorityQueue, pointList, graph);

    // Figure out which edges to actually remove
    auto deletedEdgeSet = deleteEdges(priorityQueue, pointList);

    // Extract a list of polygons an return it
    return extractPolygonList(graph, deletedEdgeSet);
}

std::vector<IndexList>
decomp::decompose(PointList const& pointList, IndexList simplePolygon, std::vector<IndexList> holeList)
{
    auto simpleWithoutHoles = removeHoles(pointList, simplePolygon, holeList);

    auto triangleList = earClipping(pointList, simpleWithoutHoles);

    return hertelMehlhorn(pointList, triangleList);
}
