
#include "triangulation.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <set>
#include <stdexcept>

using namespace decomp;

namespace
{

std::uint16_t findRightmostPoint(PointList const& pointList, IndexList const& polygon)
{
    assert(!polygon.empty());

    return std::max_element(
               polygon.begin(), polygon.end(),
               [&](std::uint16_t lhs, std::uint16_t rhs) { return pointList[lhs].x() < pointList[rhs].x(); }) -
           polygon.begin();
}

inline double determinant(Point const& lhs, Point const& rhs)
{
    return lhs[0] * rhs[1] - lhs[1] * rhs[0];
}

bool isCounterClockwise(Point const& a, Point const& b, Point const& c)
{
    return determinant(b - a, c - a) > 0.0;
}

bool isClockwise(Point const& a, Point const& b, Point const& c)
{
    return determinant(b - a, c - a) < 0.0;
}

bool inCone(Point const& a, Point const& b, Point const& c, Point const& p)
{
    if (isCounterClockwise(a, b, c))
        return isCounterClockwise(a, b, p) && isCounterClockwise(b, c, p);
    else
        return isCounterClockwise(a, b, p) || isCounterClockwise(b, c, p);
}

bool triangleContains(Point const& a, Point const& b, Point const& c, Point const& tested)
{
    return !isClockwise(a, b, tested) && !isClockwise(b, c, tested) && !isClockwise(c, a, tested);
}

bool segmentsIntersect(Point const& a, Point const& b, Point const& c, Point const& d)
{
    auto directionA = b - a;
    auto directionB = c - d; // direction purposefully reversed

    auto delta = c - a;

    auto denominator = determinant(directionA, directionB);

    if (denominator == 0.0)
        return false;

    auto lambda = (directionB[1] * delta[0] - directionB[0] * delta[1]) / denominator;
    auto mu = (directionA[0] * delta[1] - directionA[1] * delta[0]) / denominator;

    if (lambda < 0.0 || lambda > 1.0)
        return false;

    if (mu < 0.0 || mu > 1.0)
        return false;

    return true;
}

bool pointVisibleFrom(PointList const& pointList, IndexList const& indexList, int relativeIndex, Point const& from)
{
    int N = indexList.size();
    auto target = pointList[indexList[relativeIndex]];

    for (int i = 0; i < N; ++i)
    {
        auto next = (i + 1) % N;

        // Dont test edges connected to the relativ index
        if (i == relativeIndex || next == relativeIndex)
            continue;

        // Do we have an actual blocker?
        if (segmentsIntersect(from, target, pointList[indexList[i]], pointList[indexList[next]]))
            return false;
    }

    return true;
}

struct VertexNode;

struct EarLess
{
    bool operator()(VertexNode* lhs, VertexNode* rhs);
};

// Using the order and iterator constancy of the set as
// simple addressable priority queue
using EarPriorityQueue = std::multiset<VertexNode*, EarLess>;

struct VertexNode
{
    std::uint16_t index;
    VertexNode* next;
    VertexNode* prev;
    bool isConvex;
    bool isReflex;
    bool isEar = false;
    double minimumInteriorAngle;
    EarPriorityQueue::iterator queueNode;
};

bool EarLess::operator()(VertexNode* lhs, VertexNode* rhs)
{
    return lhs->minimumInteriorAngle < rhs->minimumInteriorAngle;
}

void updateNodeType(VertexNode* node, PointList const& pointList)
{
    node->isConvex =
        isCounterClockwise(pointList[node->prev->index], pointList[node->index], pointList[node->next->index]);
    node->isReflex = isClockwise(pointList[node->prev->index], pointList[node->index], pointList[node->next->index]);
}

void updateEarState(VertexNode* node, PointList const& pointList, EarPriorityQueue& queue)
{
    // Start by erasing this node's entry in the priority queue
    // If the node is still an ear, we will reinsert it later
    if (node->isEar)
        queue.erase(node->queueNode);

    // A vertex is an ear iff it's convex and no vertices are inside the attached ear
    // It is sufficient to test only for reflex vertices, as any vertex in the ear implies
    // that a reflex vertex is also there because the polygon is simple
    if (!node->isConvex)
    {
        node->isEar = false;
        return;
    }

    auto const& a(pointList[node->prev->index]);
    auto const& b(pointList[node->index]);
    auto const& c(pointList[node->next->index]);

    for (auto current = node->next->next; current != node->prev; current = current->next)
    {
        if (!current->isReflex)
            continue;

        if (triangleContains(a, b, c, pointList[current->index]))
        {
            node->isEar = false;
            return;
        }
    }

    auto x = normalize(b - a);
    auto y = normalize(c - b);
    auto z = normalize(a - c);

    auto alpha = -dot(z, x);
    auto beta = -dot(x, y);
    auto gamma = -dot(y, z);

    node->minimumInteriorAngle = std::max({ alpha, beta, gamma });
    node->isEar = true;
    node->queueNode = queue.insert(node);
}

VertexNode* findEar(EarPriorityQueue& queue)
{
    if (queue.empty())
        return nullptr;

    // Get top and pop
    auto result = *queue.begin();
    queue.erase(queue.begin());
    return result;
}

VertexNode* clipEar(IndexList& resultList, VertexNode* ear, PointList const& pointList, EarPriorityQueue& queue)
{
    resultList.insert(resultList.end(), { ear->prev->index, ear->index, ear->next->index });
    ear->prev->next = ear->next;
    ear->next->prev = ear->prev;
    updateNodeType(ear->prev, pointList);
    updateNodeType(ear->next, pointList);
    updateEarState(ear->prev, pointList, queue);
    updateEarState(ear->next, pointList, queue);
    return ear->next;
}

void extractRightmostHole(PointList const& pointList,
                          std::vector<IndexList>& holeList,
                          IndexList& hole,
                          int& rightmostPoint)
{
    // Find the rightmost hole point
    auto rightmostHole = holeList.end();

    for (auto i = holeList.begin(); i != holeList.end(); ++i)
    {
        int localRightmost = findRightmostPoint(pointList, *i);

        if (rightmostHole == holeList.end() ||
            pointList[(*rightmostHole)[rightmostPoint]].x() < pointList[(*i)[localRightmost]].x())
        {
            rightmostHole = i;
            rightmostPoint = localRightmost;
        }
    }

    hole = std::move(*rightmostHole);
    holeList.erase(rightmostHole);
}

int findVisiblePoint(PointList const& pointList,
                     IndexList const& indexList,
                     Point const& from,
                     Point const& idealDirection)
{
    auto getPoint = [&](int i) { return pointList[indexList[i]]; };

    int bestPoint = -1;
    int N = indexList.size();
    for (int i = 0; i < N; ++i)
    {
        // Look for points to the right side of our 'from'
        if (pointList[indexList[i]].x() <= from.x())
            continue;

        int const next = (i + 1) % N;
        int const previous = (i + N - 1) % N;

        auto const& rimPoint = getPoint(i);

        // Simple visibility test - can give false positives, hence the more detailed test later
        if (!inCone(getPoint(previous), rimPoint, getPoint(next), from))
        {
            continue;
        }

        // Try from smallest angle with the idealDirection
        if (bestPoint >= 0 && dot(normalize(rimPoint - from), idealDirection) <
                                  dot(normalize(getPoint(bestPoint) - from), idealDirection))
        {
            continue;
        }

        // Make sure the point is not occluded by other edges
        // This is the most expensive test, hence performed last
        if (!pointVisibleFrom(pointList, indexList, i, from))
        {
            continue;
        }

        bestPoint = i;
    }

    assert(bestPoint != -1);
    return bestPoint;
}

void removeHole(PointList const& pointList, IndexList& indexList, std::vector<IndexList>& holeList)
{
    // Find the rightmost hole point
    int rightmostPoint = 0;
    IndexList hole;
    extractRightmostHole(pointList, holeList, hole, rightmostPoint);

    Point holePoint = pointList[hole[rightmostPoint]];
    Point bestDirection =
        normalize(normalize(holePoint - pointList[hole[(rightmostPoint + 1) % hole.size()]]) +
                  normalize(holePoint - pointList[hole[(rightmostPoint + hole.size() - 1) % hole.size()]]));

    // Find a point to connect that to
    int bestPoint = findVisiblePoint(pointList, indexList, holePoint, bestDirection);

    // Splice the hole vertex into the outer polygon by first rotating
    // the insertion point to the end and then adding the hole vertices plus
    // the additional two edges.
    // Note that the clockwise ordering of the hole is correctly turned into
    // counter-clockwise here, since the polygon is semantically inverted.
    std::rotate(indexList.begin(), indexList.begin() + bestPoint, indexList.end());
    indexList.push_back(indexList.front());
    indexList.insert(indexList.end(), hole.begin() + rightmostPoint, hole.end());
    indexList.insert(indexList.end(), hole.begin(), hole.begin() + rightmostPoint + 1);
}
}

IndexList decomp::removeHoles(PointList const& pointList, IndexList indexList, std::vector<IndexList> holeList)
{
    while (!holeList.empty())
    {
        removeHole(pointList, indexList, holeList);
    }

    return indexList;
}

IndexList decomp::earClipping(PointList const& pointList, IndexList const& indexList)
{
    std::vector<VertexNode> nodeList(indexList.size());
    IndexList resultList;

    int N = indexList.size();
    if (N < 3)
        throw std::invalid_argument("Polygon needs at least 3 vertices");

    // Simple polygons with N vertices are decomposed
    // into N-2 triangles of 3 indices each
    resultList.reserve((N - 2) * 3);

    // Setup an initial circular linked list of all vertices
    // for constant-time ear removal later
    for (int i = 0; i < N; ++i)
    {
        int j = (i + 1) % N;

        auto& node0(nodeList[i]);
        auto& node1(nodeList[j]);

        node0.next = &node1;
        node1.prev = &node0;
        node0.index = indexList[i];
    }

    // Figure out which nodes are initially reflex and convex
    for (auto& node : nodeList)
        updateNodeType(&node, pointList);

    EarPriorityQueue queue;

    // Check which are ears - note that this
    // needs reflex and convex flags set up correctly
    for (auto& node : nodeList)
        updateEarState(&node, pointList, queue);

    // Clip off ears while the polygon still has any
    auto current = &nodeList.front();
    while (N >= 3)
    {
        current = findEar(queue);
        if (current == nullptr)
            throw std::invalid_argument("Polygon is not simple");

        current = clipEar(resultList, current, pointList, queue);
        --N;
    }

    // Only a line segment left now
    assert(current->next->next == current);

    return resultList;
}

decomp::Winding decomp::computeWinding(PointList const& pointList, IndexList const& polygon)
{
    // Compute the signed area using the shoelace algorithm
    auto N = static_cast<int>(polygon.size());
    double signedArea = 0.0;

    for (int i = 0; i < N; ++i)
    {
        int j = (i + 1) % N;
        signedArea += determinant(pointList[polygon[i]], pointList[polygon[j]]);
    }

    if (signedArea < 0.0)
        return Winding::Clockwise;
    else // (signedArea>0.0)
        return Winding::CounterClockwise;
}
