#include "LineRecord.h"

LineRecord::LineRecord(const int cur_id, const int dur_secs, const int next_id)
    : m_cur_id{cur_id}, m_dur_secs{dur_secs}, m_next_id{next_id}
{
}

void LineRecord::setRecord(const int cur_id, const int dur_secs, const int next_id)
{
    m_cur_id = cur_id;
    m_dur_secs = dur_secs;
    m_next_id = next_id;
}
