#include "output.hpp"
#include <limits>
#include <algorithm>

using namespace decomp;

namespace
{
void writeSvgHeader(std::ostream& out, Point const& min, Point const& max)
{
    out << R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg
  xmlns="http://www.w3.org/2000/svg"
  version="1.1"
)";
    out << "  width=\"" << std::ceil(max[0]-min[0]) << "\" height=\"" << std::ceil(max[1]-min[1]) << "\">\n";
    out << "  <rect width=\"100%\" height=\"100%\" fill=\"#fff\"/>\n";
    out << "  <g transform=\"translate(" << -min[0] << " " << -min[1] << ")\">\n";
}

void writeSvgFooter(std::ostream& out)
{
    out << R"(
  </g>
</svg>
)";
    out << std::endl;
}

void writeSvgPolygon(std::ostream& svg, PointList const& points, IndexList const& indices, char const* color)
{
    svg << "    <polygon points=\"";

    for (auto const& each : indices)
    {
        auto point = points[each];
        svg << point[0] << "," << point[1] << " ";
    }

    svg << "\"\nstyle=\"fill:none;stroke:" << color << ";stroke-width:3\" />\n";

}

void writeSvgPointIndices(std::ostream& svg, PointList const& points)
{
    svg << R"(
    <style>
      .small { font: italic 9px sans-serif; fill: #22a }
    </style>
)";

    for (std::size_t i = 0; i < points.size(); ++i)
    {
        auto point = points[i];
        svg << "    <text x=\""<<point[0]<<"\" y=\""<<point[1]<<R"(" class="small">)"<<i<<"</text>\n";
    }
}
} // namespace

void svg::writePolygon(std::ostream& svg, PointList const& points, IndexList const& indices,
                       std::vector<IndexList> const& holes)
{
    Point min{ std::numeric_limits<float>::max() };
    Point max{ -std::numeric_limits<float>::max() };

    for (auto& each : points)
    {
        for (int i = 0; i < 2; ++i)
        {
            min[i] = std::min(min[i], each[i]);
            max[i] = std::max(max[i], each[i]);
        }
    }
    writeSvgHeader(svg, min, max);

    writeSvgPolygon(svg, points, indices, "#000");
    for (auto const& hole : holes)
    {
        writeSvgPolygon(svg, points, hole, "#a00");
    }

    writeSvgPointIndices(svg, points);

    writeSvgFooter(svg);
}

void svg::writeTriangles(std::ostream& svg, PointList const& points, IndexList const& indices)
{
    Point min{ std::numeric_limits<float>::max() };
    Point max{ -std::numeric_limits<float>::max() };

    for (auto& each : points)
    {
        for (int i = 0; i < 2; ++i)
        {
            min[i] = std::min(min[i], each[i]);
            max[i] = std::max(max[i], each[i]);
        }
    }
    writeSvgHeader(svg, min, max);

    for (std::size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        svg << "    <polygon style=\"fill:none;stroke:black;stroke-width:3\" points=\"";

        for (std::size_t j = 0; j < 3; ++j)
        {
            auto point = points[indices[i + j]];
            svg << point[0] << "," << point[1] << " ";
        }

        svg << "\"/>\n";
    }

    writeSvgFooter(svg);
}

void decomp::writePoints(std::ostream& out, PointList const& points)
{
    out << "{";
    for (auto const& point : points)
        out << "{" << point.x() << "," << point.y() << "},";
    out << "}";
}

void decomp::writeIndices(std::ostream& out, std::vector<IndexList> const& indices)
{
    out << "{";
    for (auto const& list : indices)
    {
        writeIndices(out, list);
        out << ",";
    }
    out << "}";
}

void decomp::writeIndices(std::ostream& out, IndexList const& indices)
{
    out << "{";
    for (auto const& index : indices)
        out << index << ",";
    out << "}";
}

namespace
{
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
}

std::ostream & json::dump(
    std::ostream &out,
    PointList const& pointList,
    IndexList const& outerPolygon,
    std::vector<IndexList> const& holeList,
    std::vector<IndexList> const& convexPolygonList)
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