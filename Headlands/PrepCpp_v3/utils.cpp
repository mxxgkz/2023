#include "utils.h"

bool headerIsValid(const std::string &header)
{
    if (header != HEADER)
        return false;
    else
        return true;
}

bool lineIsValid(const std::string &log_line, LineRecord &line_record)
{
    int cnt = 0;
    int vals[3] = {0};
    std::string nstr;
    std::istringstream iss{log_line};
    while (!iss.eof())
    {
        if (cnt < 2)
            std::getline(iss, nstr, ',');
        else if (cnt == 2)
            std::getline(iss, nstr);
        else // More than 3 values got
            return false;

        unsigned long len = nstr.length();
        if (
            len == 0 ||
            (cnt == 0 && nstr == "0") || // Current job id should not be 0
            (len > 1 && nstr[0] == '0')  // Multi-digits number should not start with 0
        )
            return false;

        for (auto ch : nstr)
        {
            if (!std::isdigit(ch))
                return false;
        }

        vals[cnt++] = std::stoi(nstr);
    }

    if (vals[1] > SECONDS_IN_A_DAY)
        return false;

    line_record.setRecord(vals[0], vals[1], vals[2]);

    return true;
}

bool processLineRecordToJob(
    const LineRecord &line_record,
    std::unordered_map<int, Job *> &map_id_to_job_ptr)
{
    int next_id = line_record.getNextId();

    if (
        (map_id_to_job_ptr.find(next_id) != map_id_to_job_ptr.end()) &&
        (map_id_to_job_ptr[next_id]->getPreJob() != nullptr))
    {
        // The next job already had the dependency, error.
        if (PRINT_DETAILS)
        {
            std::cout << "The next job already had the dependency, error." << next_id << std::endl;
        }
        return false;
    }

    int cur_id = line_record.getCurId();
    Job *cur_job_ptr = nullptr;
    if (map_id_to_job_ptr.find(cur_id) == map_id_to_job_ptr.end())
    {
        // New job instance
        cur_job_ptr = new Job(line_record, map_id_to_job_ptr);
    }
    else
    {
        cur_job_ptr = map_id_to_job_ptr[cur_id];
        if ((cur_job_ptr->getNextJob() != nullptr) || cur_job_ptr->isTheLastJob())
        {
            // The job previously started another job or it was the last job of a chain, error.
            if (PRINT_DETAILS)
            {
                std::cout << "The job previously started another job or it was the last job of a chain, error." << cur_id << std::endl;
            }
            return false;
        }

        cur_job_ptr->setNextJob(line_record, map_id_to_job_ptr);
    }

    return true;
}

bool isAChainValid(
    Job *init_job_ptr,
    std::unordered_set<int> &set_job_ids)
{
    Job *job_ptr = init_job_ptr;
    while (job_ptr)
    {
        if (set_job_ids.find(job_ptr->getId()) != set_job_ids.end())
        {
            // The job_id was visited before so that it is not a chain.
            if (PRINT_DETAILS)
            {
                std::cout << "The job_id was visited before so that it is not a chain."
                          << job_ptr->getId() << std::endl;
            }
            return false;
        }

        set_job_ids.insert(job_ptr->getId());
        job_ptr = job_ptr->getNextJob();
    }

    return true;
}

bool hasJobCycles(
    const std::unordered_map<int, Job *> &map_id_to_job_ptr,
    std::vector<Job *> &chain_head_ptrs)
{
    std::unordered_set<int> set_job_ids;
    for (const auto &pair : map_id_to_job_ptr)
    {
        Job *job_ptr = pair.second;
        if (job_ptr->getPreJob() == nullptr)
        {
            // No dependency. The beginning of a chain.
            if (isAChainValid(job_ptr, set_job_ids))
            {
                chain_head_ptrs.push_back(job_ptr);
            }
            else
            {
                return true;
            }
        }
    }

    if (set_job_ids.size() != map_id_to_job_ptr.size())
    {
        // There are some cycles.
        if (PRINT_DETAILS)
        {
            std::cout << "There are some cycles." << std::endl;
            for (const auto &pair : map_id_to_job_ptr)
            {
                if (set_job_ids.find(pair.first) == set_job_ids.end())
                {
                    std::cout << (pair.second)->getId() << std::endl;
                }
            }
        }
        return true;
    }

    return false;
}

int genChains(
    const std::unordered_map<int, Job *> &map_id_to_job_ptr,
    std::vector<Chain *> &chain_ptrs)
{
    std::vector<Job *> chain_head_ptrs;
    if (hasJobCycles(map_id_to_job_ptr, chain_head_ptrs))
    {
        if (PRINT_DETAILS)
        {
            std::cout << "The parsed log has job cycles." << std::endl;
        }
        return raiseError(PRINT_LN, -1);
    }

    for (auto chain_head_ptr : chain_head_ptrs)
    {
        Chain *chain_ptr = new Chain(chain_head_ptr);
        if (!(chain_ptr->isDurRangeValid()))
        {
            if (PRINT_DETAILS)
            {
                std::cout << "The parsed chain has too long duration." << std::endl;
            }
            return raiseError(PRINT_LN, -2);
        }
        chain_ptrs.push_back(chain_ptr);
    }

    return 0;
}

void printSummaryReport(const std::vector<Chain *> &chain_ptrs)
{
    // https://stackoverflow.com/a/24002756

    // Sort chains based on chain duration.
    // https://stackoverflow.com/a/10761286
    std::priority_queue<Chain *, std::vector<Chain *>, CmpChainPtrs> pqchains(
        chain_ptrs.begin(),
        chain_ptrs.end());

    std::string fmt_runtime;
    while (!pqchains.empty())
    {
        const Chain *pchain = pqchains.top();
        pqchains.pop();
        std::cout << '-' << '\n';
        std::cout << "start_job: " << pchain->getInitId() << '\n';
        std::cout << "last_job: " << pchain->getLastId() << '\n';
        std::cout << "number_of_jobs: " << pchain->getNumJobs() << '\n';
        pchain->formatDurationString(pchain->getDurSecs(), fmt_runtime);
        std::cout << "job_chain_runtime: " << fmt_runtime << '\n';
        pchain->formatDurationString(pchain->getAveDurSecs(), fmt_runtime);
        std::cout << "average_job_time: " << fmt_runtime << '\n';
    }

    std::cout << '-' << '\n';
}

int raiseError(const bool print_ln, const int ln)
{
    if (print_ln)
    {
        std::cout << ERROR_MESSAGE << ": " << ln << std::endl;
    }
    else
    {
        std::cout << ERROR_MESSAGE << std::endl;
    }

    return 1;
}
