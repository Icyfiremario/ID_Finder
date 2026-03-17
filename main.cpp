// STD
#include <iostream>
#include <cstdint>
#include <unistd.h>
#include <fstream>

// OMP
#include <omp.h>

struct IDCombo
{
    uint16_t TID;
    uint16_t SID;
};

inline bool calc_shiny(const uint16_t TID, const uint32_t SID, const uint32_t PID)
{
    const uint16_t PID_u = (PID >> 16) & 0xFFFF;
    const uint16_t PID_l = PID & 0xFFFF;

    return (TID ^ SID ^ PID_u ^ PID_l) < 8;
}

int main(const int argc, char** argv)
{
    uint32_t pid = 0xB37E9695;
    uint16_t tid = 0;

    bool userTID = false; // Is the TID user provided.

    int c;

    while ((c = getopt(argc, argv, "t: p:")) != -1)
    {
        switch (c)
        {
            case 't':
                userTID = true;
                tid = atoi(optarg);
                break;

            case 'p':
                pid = atoi(optarg);
                break;

            case ':':
            case '?':
                std::cerr << "Option -" << optopt << " requires an argument." << std::endl;
                break;

            default:
                std::cerr << "Unknown option `-" << optopt << "`." << std::endl;
                break;
        }
    }

    for (; optind < argc; optind++)
    {
        std::cerr << "Non-option argument `" << optopt << "`." << std::endl;
    }

    std::vector<IDCombo> results;

    if (userTID)
    {
        #pragma omp parallel
        {
            #pragma omp single
            {
                std::cout << "TID provided.\n";
                std::cout << "Running with " << omp_get_num_threads() << " threads.\n";
            }

            std::vector<IDCombo> localResults;

            #pragma omp for
            for (uint32_t i = 0; i <= UINT16_MAX; i++)
            {

                if (calc_shiny(tid, i, pid))
                {
                    IDCombo combo;
                    combo.TID = tid;
                    combo.SID = i;
                    std::cout << combo.TID << '/' << combo.SID << '\n';
                    localResults.push_back(combo);
                }
            }

            #pragma omp critical
            {
                results.insert(results.end(), localResults.begin(), localResults.end());
            }
        }
    }
    else
    {
        #pragma omp parallel
        {
            #pragma omp single
            {
                std::cout << "No TID given.\n";
                std::cout << "Running with " << omp_get_num_threads() << " threads.\n";
            }

            std::vector<IDCombo> localResults;

            #pragma omp for collapse(2)
            for (uint16_t i = 0; i <= UINT16_MAX; i++)
            {
                for (uint32_t j = 0; j <= UINT16_MAX; j++)
                {
                    IDCombo combo;
                    combo.TID = i;
                    combo.SID = j;

                    if (calc_shiny(i, j, pid))
                    {
                        localResults.push_back(combo);
                    }
                }
            }

            #pragma omp critical
            {
                results.insert(results.end(), localResults.begin(), localResults.end());
            }
        }
    }

    std::ofstream outFile("results.csv");
    outFile << "TID, SID\n";

    for (auto [TID, SID] : results)
    {
        outFile << TID << ", " << SID << "\n";
    }

    std::cout << "Output to file: results.csv." << std::endl;
    return 0;
}