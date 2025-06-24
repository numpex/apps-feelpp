#include <feel/feelcore/environment.hpp>
#include <feel/feelmodels/heat/heat.hpp>
#include "../utils/report.hpp"

#ifndef FEELPP_DIM // Default value for IDE satisfaction, values are set in CMakeLists
#define ORDER 1
#define FEELPP_DIM 2
#endif


int main(int argc, char** argv)
{
    using namespace Feel;
    po::options_description myoptions("my options");
    myoptions.add( toolboxes_options("heat") );
    myoptions.add_options()
        ( "output-dir", Feel::po::value<std::string>()->default_value("") )
        ( "report", Feel::po::value<std::string>()->default_value("scalability.json") )
    ;

    // Get current date and hour
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);
    char datetime[20];
    std::strftime(datetime, sizeof(datetime), "%Y-%m-%d-%H:%M", now_tm);

    double time_solve, time_export;

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

    // solve
    tic();
    heat->solve();
    time_solve = toc("solve");
    tic();
    heat->exportResults();
    time_export = toc("export results");

    const fs::path output_dir_option = Environment::expand(soption("output-dir"));
    const fs::path outputDir = (output_dir_option == "") ? fs::current_path() : output_dir_option;

    Report report( outputDir, soption("report") );

    const auto measures = heat->postProcessMeasures();
    auto values = measures.values();
    for (const auto& [key, val] : values)
    {
        Feel::cout << key << " : " << val << std::endl;
    }

    report.setMeasure("Norm_error-evaluated_L2-error", values["Norm_error-evaluated_L2-error"]);
    report.setMeasure("time_solve", time_solve);
    report.setMeasure("time_export", time_export);

    report.exportMeasures();

    Feel::cout << "[SUCCESS]" << std::endl;

    return 0;
}
