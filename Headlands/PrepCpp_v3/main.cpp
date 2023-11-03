#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "Chain.h"
#include "Job.h"
#include "LineRecord.h"
#include "utils.h"

int main([[maybe_unused]] int argc, char *argv[])
{
    // std::ifstream std::cin{"input_files/input_default_valid.csv"};
    // std::ifstream std::cin{"input_files/input_garbage.csv"};
    // std::string fpath = argv[1];
    // std::cout << "Enterred input file path: " << fpath << std::endl;

    // std::string fpath{"input_files/input_default_valid.csv"};

    // std::ifstream std::cin{fpath};

    std::unordered_map<int, Job *> map_id_to_job_ptr;

    int ln = 0; // line number
    if (!std::cin)
        return raiseError(PRINT_LN, ln);
    else
        ln++;

    std::string log_line;

    // Get header
    if (std::getline(std::cin, log_line))
    {
        if (!headerIsValid(log_line))
        {
            return raiseError(PRINT_LN, ln);
        }
        ln++;
    }
    else
    {
        return raiseError(PRINT_LN, ln);
    }

    // Process logging line-by-line
    LineRecord line_record{};
    while (std::getline(std::cin, log_line))
    {
        if (log_line.empty())
            break;

        if (!lineIsValid(log_line, line_record))
        {
            return raiseError(PRINT_LN, ln);
        }

        if (!processLineRecordToJob(line_record, map_id_to_job_ptr))
        {
            // if (ln == 4)
            // {
            //     std::cout << log_line << std::endl;
            // }
            if (PRINT_DETAILS)
            {
                std::cout << ln << ':' << log_line << std::endl;
            }
            return raiseError(PRINT_LN, ln);
        }

        ln++;
    }

    std::vector<Chain *> chain_ptrs;
    int gen_chain_status = genChains(map_id_to_job_ptr, chain_ptrs);
    if (gen_chain_status != 0)
    {
        // Raised error.
        return gen_chain_status;
    }

    printSummaryReport(chain_ptrs);

    return 0;
}
