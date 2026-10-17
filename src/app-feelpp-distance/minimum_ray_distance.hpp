#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>

namespace feelpp_apps
{
// Results are grouped by origin, with one entry per sampled ray. Missing
// intersections must neither contribute zero nor reuse another origin's hits.
template <typename RayResults>
std::optional<double> minimum_ray_distance( RayResults const& results,
                                           std::size_t first, std::size_t count )
{
    if ( first > results.size() || count > results.size() - first )
        throw std::out_of_range( "Incomplete BVH ray results" );
    double nearest = std::numeric_limits<double>::infinity();
    for ( std::size_t i = first; i < first + count; ++i )
        if ( !results[i].empty() )
            nearest = std::min( nearest, results[i].front().distance() );
    if ( !std::isfinite( nearest ) )
        return std::nullopt;
    return nearest;
}
}
