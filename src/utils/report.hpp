#include <feel/feelcore/environment.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


struct Element {
    std::string date;
    int nporc;
    double h;
    double error;
    double time_solve;
    double time_export;
};


class Report {
public:
    Report(const std::string& filePath) : filePath(filePath)
    {
        file.open(filePath, std::ios::app);

        if (!file.is_open())
            Feel::cerr << "Unable to open file: " << filePath << std::endl;

        if (file.tellp() == 0)
        {
            if (Feel::Environment::isMasterRank())
                file << "date,nporc,h,error,time_solve,time_export\n";
        }
    }

    ~Report()
    {
        file.close();
    }

    void addEntry(const Element el)
    {
        file << el.date << "," << el.nporc << "," << el.h << "," << el.error << "," << el.time_solve << "," << el.time_export  << "\n";
    }

private:
    std::string filePath;
    std::ofstream file;
};
