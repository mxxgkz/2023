#include "Job.h"

Job::Job(
    const LineRecord &line_record,
    std::unordered_map<int, Job *> &map_id_to_job_ptr)
{
    m_id = line_record.getCurId();
    map_id_to_job_ptr[m_id] = this;
    setNextJob(line_record, map_id_to_job_ptr);
}

void Job::setNextJob(
    const LineRecord &line_record,
    std::unordered_map<int, Job *> &map_id_to_job_ptr)
{
    m_dur_secs = line_record.getDurSecs();
    m_next_id = line_record.getNextId();
    if (m_next_id > 0)
    {
        if (map_id_to_job_ptr.find(m_next_id) == map_id_to_job_ptr.end())
        {
            m_next_job = new Job(m_next_id);
            map_id_to_job_ptr[m_next_id] = m_next_job;
        }
        else
        {
            m_next_job = map_id_to_job_ptr.at(m_next_id);
        }
        m_next_job->setPreJob(this);
    }
}