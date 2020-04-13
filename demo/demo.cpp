// This program illustrates the use of decomp library
// It creates the convex decomposition of a polygon with
// two holes and displays to standard output a python script
// that displays the input polygon and patches of its convex
// polygon.

#include <vector>
#include <fstream>
#include <iostream>

#include <decomp/convex_decomposition.hpp>
#include <decomp/triangulation.hpp>
#include <decomp/operations.hpp>
#include <decomp/output.hpp>

using namespace decomp;

template <class Container, class F>
void writeJsonArray(std::ostream& out, Container const& container, F f)
{
  out << "[";
  auto i = container.begin();
  if (i != container.end())
    f(out, *i++);
  for (;i != container.end();++i)
  {
    out << ",";
    f(out, *i);
  }
  out << "]";
}

std::ostream & dumpJson(
    std::ostream &out,
    const PointList& pointList,
    const IndexList& outerPolygon,
    const std::vector<IndexList>& holeList,
    const std::vector<IndexList>& convexPolygonList)
{
  out << "{" << std::endl;
  out << "\"vertices\":" << std::endl;
  writeJsonArray(out, pointList, [](std::ostream& os, const Point& p) {os << "[" << p[0] << "," << p[1] << "]";});
  out << "," << std::endl;
  out << "\"input\": {" << std::endl;
  out << "\"outer\": " << std::endl;
  writeJsonArray(out, outerPolygon, [](std::ostream& os, const std::uint16_t& p) {os << p;});
  out << "," << std::endl;
  out << "\"holes\": " << std::endl;
  writeJsonArray(out, holeList, [](std::ostream& os, IndexList const& hole)
  {
    writeJsonArray(os, hole, [](std::ostream& os, std::uint16_t index){os << index;});
  });
  out << std::endl << "}," << std::endl;
  out << "\"output\":" << std::endl << "[";
  writeJsonArray(out, convexPolygonList, [](std::ostream& os, IndexList const& hole)
  {
    writeJsonArray(os, hole, [](std::ostream& os, std::uint16_t index){os << index;});
  });
  out << "}" << std::endl;
  return out;
}

int main()
{
  const PointList pointList={
    // points on the outer polygon
    {-4, 0}, {-3, -2}, {3, -2}, {4, 0}, {3, 2}, {-3, 2},
    // points on the left hole
    {-3, 0}, {-2, -1}, {-1, 0}, {-2, 1},
    // points on the right hole
    {1, 0}, {2, -1}, {3, 0}, {2, 1}
  };

  const IndexList outerPolygon = {
    0, 1, 2, 3, 4, 5
  };

  const std::vector<IndexList> holeList = {
    {13, 12, 11, 10},
    {9, 8, 7, 6}
  };

  const std::vector<IndexList> convexPolygonList = decompose(
    pointList, outerPolygon, holeList);

  dumpJson(
    std::cout, pointList, outerPolygon, holeList, convexPolygonList);

  return 0;
}
