#ifndef UTILS_H
#define UTILS_H

#include <cctype>
#include <iostream>
#include <queue>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Chain.h"
#include "Job.h"
#include "LineRecord.h"

bool headerIsValid(const std::string &header);

bool lineIsValid(const std::string &log_line, LineRecord &line_record);

bool processLineRecordToJob(
    const LineRecord &line_record,
    std::unordered_map<int, Job *> &map_id_to_job_ptr);

bool isAChainValid(
    Job *init_job_ptr,
    std::unordered_set<int> &set_job_ids);

bool hasJobCycles(
    const std::unordered_map<int, Job *> &map_id_to_job_ptr,
    std::vector<Job *> &chain_head_ptrs);

int genChains(
    const std::unordered_map<int, Job *> &map_id_to_job_ptr,
    std::vector<Chain *> &chain_ptrs);

void printSummaryReport(const std::vector<Chain *> &chain_ptrs);

int raiseError(const bool print_ln, const int ln);

#endif
