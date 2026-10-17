#include "minimum_ray_distance.hpp"
#include <iostream>
#include <vector>

struct Hit
{
    double value;
    double distance() const { return value; }
};

int main()
{
    using Results = std::vector<std::vector<Hit>>;
    auto require = []( bool condition, char const* message ) {
        if ( !condition )
            throw std::runtime_error( message );
    };
    try
    {
        Results rays = { {{0.2}}, {}, {{0.4}}, {}, {{0.8}}, {{1.0}}, {}, {}, {} };
        require( feelpp_apps::minimum_ray_distance(rays, 0, 3) == 0.2,
                 "A missed ray must not make the distance zero" );
        require( feelpp_apps::minimum_ray_distance(rays, 3, 3) == 0.8,
                 "An origin must not reuse a previous origin's smaller distance" );
        require( !feelpp_apps::minimum_ray_distance(rays, 6, 3),
                 "All-miss batches must not manufacture an intersection" );
        require( !feelpp_apps::minimum_ray_distance(Results{}, 0, 0),
                 "Empty local ray batches must be supported" );
        bool rejected = false;
        try { feelpp_apps::minimum_ray_distance(rays, 8, 3); }
        catch ( std::out_of_range const& ) { rejected = true; }
        require( rejected, "Incomplete results must be rejected before indexing" );
    }
    catch ( std::exception const& error )
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
    std::cout << "BVH missed-ray and independent-origin regression checks passed\n";
}
