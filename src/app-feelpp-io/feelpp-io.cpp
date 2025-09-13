/**
 * @file feelpp-io.cpp
 * @brief Implementation file for Feel++ IO application.
 *
 * @author Thomas Saigre
 * @date 2025-09-08
 */

#include <iostream>
#include <feel/feelcore/environment.hpp>
#include <feel/feelfilters/loadmesh.hpp>
#include <feel/feeldiscr/pch.hpp>
#include <feel/feelfilters/exporter.hpp>


template <int DIM, int ORDER>
int run()
{
    using mesh_t = Feel::Mesh<Feel::Simplex<DIM, 1>>;
    using namespace Feel;

    double time_loadMesh = 0,
           time_createFunctionSapce = 0,
           time_export = 0;
    const int nRun = ioption( "nrun" );

    for (int i=0; i<nRun; ++i)
    {
        tic();
        auto mesh = loadMesh( _mesh=new mesh_t);
        time_loadMesh += toc("loadMesh");

        tic();
        auto Xh = Pch<ORDER>(mesh);
        time_createFunctionSapce += toc("create function space");

        auto u = Xh->element(Px() * Px() + 4 * Py() + cos(Pz()));

        auto e = exporter( _mesh = mesh,
                           _name = fmt::format("Export_{}", i)
        );
        e->add("u", u);

        tic();
        e->save();
        time_export += toc("export time");
    }

    Feel::cout << "Time load mesh " << time_loadMesh / nRun << std::endl;
    Feel::cout << "Time load create function space " << time_createFunctionSapce / nRun << std::endl;
    Feel::cout << "Time to export " << time_export / nRun << std::endl;

    nl::json time_measures = {{
        "time", {}
    }};
    time_measures["time"]["loadMesh"] = time_loadMesh / nRun;
    time_measures["time"]["functionSpace"] = time_createFunctionSapce / nRun;
    time_measures["time"]["export"] = time_export / nRun;

    if ( Environment::isMasterRank() )
    {
        std::ofstream ofs("time_measures.json");
        ofs << time_measures.dump(2);
        ofs.close();
    }

    return 0;
}


int main(int argc, char** argv)
{
    using namespace Feel;

    po::options_description myoptions("my options");
    myoptions.add_options()
        ( "nrun", Feel::po::value<int>()->default_value(10) )
        ( "dimension", Feel::po::value<int>()->default_value(3) )
        ( "discretization", Feel::po::value<std::string>()->default_value("P1") )
    ;

    Environment env( _argc=argc, _argv=argv,
                     _desc=myoptions,
                     _about=about(_name="Feelpp I/O",
                                  _author="Feel++ Consortium",
                                  _email="feelpp-devel@feelpp.org"));

    int dimension = ioption(_name="dimension");
    std::string discretization = soption(_name="discretization");
    int status = 0;

    auto dimt = hana::make_tuple(hana::int_c<2>,hana::int_c<3>);
    auto discretizationt = hana::make_tuple( hana::make_tuple("P1", hana::int_c<1> ),
                                             hana::make_tuple("P2", hana::int_c<2> ),
                                             hana::make_tuple("P3", hana::int_c<3> ),
                                             hana::make_tuple("P4", hana::int_c<4> ),
                                             hana::make_tuple("P5", hana::int_c<5> ),
                                             hana::make_tuple("P6", hana::int_c<6> )
    );

    hana::for_each( hana::cartesian_product(hana::make_tuple(dimt, discretizationt)), [&discretization, &dimension, &status]( auto const& d )
    {
        constexpr int _dimension = std::decay_t<decltype(hana::at_c<0>(d))>::value;
        std::string const& _discretization = hana::at_c<0>( hana::at_c<1>(d) );
        constexpr int _order = std::decay_t<decltype(hana::at_c<1>(hana::at_c<1>(d)))>::value;

        if (dimension == _dimension && discretization == _discretization)
        {
            status = run<_dimension, _order>();
        }
    } );

    return status;
}
