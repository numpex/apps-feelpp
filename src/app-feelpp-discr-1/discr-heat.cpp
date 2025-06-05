#include <feel/feelcore/environment.hpp>
#include <feel/feelmodels/heat/heat.hpp>
#include <feelpp/feel/feelcore/json.hpp>

#ifndef FEELPP_DIM // Default value for IDE satisfaction, values are set in CMakeLists
#define ORDER 1
#define FEELPP_DIM 2
#endif

using metrics = std::map<std::string, double>;

int main(int argc, char** argv)
{
    using namespace Feel;
    po::options_description myoptions("my options");
    myoptions.add( toolboxes_options("heat") );

    Environment env( _argc=argc, _argv=argv,
                     _desc=myoptions,
                     _about=about(_name="update_toolbox",
                                  _author="Feel++ Consortium",
                                  _email="feelpp-devel@feelpp.org"));

    Feel::cout << fmt::format("Running app-feelpp-discr-1 (heat toolbox) in {}D, with the discretization P{}", FEELPP_DIM, ORDER) << std::endl;

    typedef FeelModels::Heat< Simplex<FEELPP_DIM, 1>,
                              Lagrange<ORDER, Scalar, Continuous, PointSetFekete> > model_type;
    std::shared_ptr<model_type> heat( new model_type("heat") );
    // init the toolbox
    heat->init();
    heat->printAndSaveInfo();

    heat->solve();
    heat->exportResults();

    const auto measures = heat->postProcessMeasures();
    auto values = measures.values();
    for (const auto& [key, val] : values)
    {
        Feel::cout << key << " : " << val << std::endl;
    }

    metrics met;
    met["output"] = values["Norm_error-evaluated_L2-error"];

    return 0;
}
