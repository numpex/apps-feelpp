#include <feel/feelcore/environment.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;


class Report {
public:
    Report(const std::string& outputDir, const std::string& filePath) : M_outputDir(outputDir), M_path(filePath) {}

    ~Report() {}

    void exportMeasures()
    {
        if ( Feel::Environment::isMasterRank() )
        {
            if (!fs::exists(M_outputDir))
                fs::create_directories(M_outputDir);

            std::ofstream outfile(M_outputDir/M_path);
            if ( outfile.is_open() )
            {
                outfile << "{\n";
                size_t count = 0;
                size_t total = M_data.size();
                for (auto [name, value] : M_data)
                {
                    outfile << fmt::format("  \"{}\": {}", name, value);
                    if (++count < total)
                        outfile << ",\n";
                    else
                        outfile << "\n";
                }
                outfile << "}\n";
            }
            else
                Feel::cerr << fmt::format( "[ERROR] Could not open file {}", M_path ) << std::endl;
        }
    }

    void setMeasure(std::string const& name, double value)
    {
        M_data[name] = value;
    }

    double getMeasure(std::string const& name)
    {
        return M_data.at(name);
    }

private:
    fs::path M_outputDir;
    fs::path M_path;
    std::map<std::string, double> M_data;
};

