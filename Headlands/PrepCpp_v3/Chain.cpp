#include "Chain.h"

Chain::Chain(Job *init_job)
    : m_dur_secs{0}, m_num_jobs{0}, m_last_id{0}, m_end_id{0}
{
    Job *job_ptr = init_job;
    m_init_id = init_job->getId();
    while (true)
    {
        m_dur_secs += job_ptr->getDurSecs();
        m_num_jobs++;
        if (job_ptr->getNextJob() == nullptr)
        {
            m_last_id = job_ptr->getId();
            if (job_ptr->getNextId() == 0)
            {
                m_end_id = m_last_id;
            }
            break;
        }
        else
        {
            job_ptr = job_ptr->getNextJob();
        }
    }
}

void Chain::formatDurationString(const int dur_secs, std::string &dur_str) const
{
    dur_str = "";
    int hrs = dur_secs / 3600;
    int mins = (dur_secs % 3600) / 60;
    int secs = (dur_secs % 60);
    int hr_min_sec[] = {hrs, mins, secs};

    for (auto it : hr_min_sec)
    {
        intToStringAndAppend(dur_str, it);
    }
}

void Chain::intToStringAndAppend(std::string &output_str, const int val) const
{
    std::string val_str = std::to_string(val);
    if (val_str.length() == 1)
    {
        val_str = "0" + val_str;
    }
    if (!output_str.empty())
    {
        val_str = ":" + val_str;
    }

    output_str.append(val_str);
}
