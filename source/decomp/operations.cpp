#include "operations.hpp"

using namespace decomp;

IndexList Remapper::apply(IndexList const& indices)
{
    IndexList result;
    result.reserve(indices.size());
    for (auto each : indices)
    {
        result.push_back(mMapping.insert(std::make_pair(each, mMapping.size())).first->second);
    }
    return result;
}

PointList Remapper::mapped(PointList const& points)
{
    PointList result;
    result.resize(mMapping.size());
    for (auto const& each : mMapping)
    {
        result[each.second] = points[each.first];
    }
    return result;
}
