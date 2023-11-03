#ifndef CHAIN_H
#define CHAIN_H

#include <string>
#include <unordered_map>
#include <vector>
#include "Job.h"
#include "LineRecord.h"

class Chain
{
public:
    Chain(){};
    Chain(Job *init_job);

    int getDurSecs() const { return m_dur_secs; };
    int getInitId() const { return m_init_id; };
    int getLastId() const { return m_last_id; };
    int getEndId() const { return m_end_id; };
    int getNumJobs() const { return m_num_jobs; };
    int getAveDurSecs() const { return m_dur_secs / getNumJobs(); };
    bool isChainEnded() const { return m_end_id > 0; };

    bool isDurRangeValid() const { return m_dur_secs <= SECONDS_IN_A_DAY; };
    void formatDurationString(const int dur_secs, std::string &dur_str) const;

private:
    int m_init_id;
    int m_dur_secs;
    int m_num_jobs;
    int m_last_id;
    int m_end_id;

    void intToStringAndAppend(std::string &output_str, const int val) const;
};

struct CmpChainPtrs
{
    bool operator()(const Chain *lhs, const Chain *rhs) const
    {
        return (lhs->getDurSecs() < rhs->getDurSecs());
    }
};

#endif
