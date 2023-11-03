#ifndef JOB_H
#define JOB_H

#include <unordered_map>
#include "LineRecord.h"

class Job
{
public:
    Job(){};
    Job(const int id) : m_id{id} {};
    Job(
        const LineRecord &line_record,
        std::unordered_map<int, Job *> &map_id_to_job_ptr);

    void setPreJob(Job *pjob) { m_pre_job = pjob; };
    void setNextJob(
        const LineRecord &line_record,
        std::unordered_map<int, Job *> &map_id_to_job_ptr);

    int getId() const { return m_id; };
    int getNextId() const { return m_next_id; };
    int getDurSecs() const { return m_dur_secs; };
    Job *getPreJob() const { return m_pre_job; };
    Job *getNextJob() const { return m_next_job; };

    bool isTheLastJob() const { return ((m_dur_secs >= 0) && (m_next_job == nullptr)); };

private:
    int m_id;
    int m_next_id;
    int m_dur_secs = -1;
    Job *m_pre_job = nullptr;
    Job *m_next_job = nullptr;
};

#endif
