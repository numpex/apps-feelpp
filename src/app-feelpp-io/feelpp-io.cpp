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

#ifndef FEELPP_DIM // Default value for IDE satisfaction, values are set in CMakeLists
#define FEELPP_DIM 2
#endif
#ifndef ORDER
#define ORDER 2
#endif


int main(int argc, char** argv)
{
    using namespace Feel;
    po::options_description myoptions("my options");

    using mesh_t = Feel::Mesh<Feel::Simplex<FEELPP_DIM, 1>>;
    // using exporter_t = Exporter<mesh_t, ORDER>;
    // using Function_t =

    myoptions.add_options()
        ( "nrun", Feel::po::value<int>()->default_value(10))
    ;

    Environment env( _argc=argc, _argv=argv,
                     _desc=myoptions,
                     _about=about(_name="Feelpp I/O",
                                  _author="Feel++ Consortium",
                                  _email="feelpp-devel@feelpp.org"));

    std::cout << "Hello world" << std::endl;

    double time_loadMesh = 0,
           time_createFunctionSapce = 0;
    const int nRun = ioption( "nrun" );

    for (int i=0; i<nRun; ++i)
    {
        tic();
        auto mesh = loadMesh( _mesh=new mesh_t);
        time_loadMesh += toc("loadMesh");

        tic();
        auto Xh = Pch<ORDER>(mesh);
        time_createFunctionSapce += toc("create function space");

        #if FEELPP_DIM == 2
            auto u = Xh->element(Px() * Px() + 4 * Py());
        #else
            auto u = Xh->element(Px() * Px() + 4 * Py() + cos(Pz()));
        #endif

        auto e = exporter( _mesh = mesh,
                           _name = fmt::format("Export_{}", i)
        );
        e->add("u", u);

        e->save();
    }

    Feel::cout << "Time load mesh " << time_loadMesh / nRun << std::endl;
    Feel::cout << "Time load create function space " << time_createFunctionSapce / nRun << std::endl;
}