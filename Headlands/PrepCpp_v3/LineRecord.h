#ifndef LINERECORD_H
#define LINERECORD_H

#include "constants.h"

class LineRecord
{
public:
    LineRecord(){};
    LineRecord(const int cur_id, const int dur_secs, const int next_id);

    void setRecord(const int cur_id, const int dur_secs, const int next_id);

    int getCurId() const { return m_cur_id; };
    int getDurSecs() const { return m_dur_secs; };
    int getNextId() const { return m_next_id; };

private:
    int m_cur_id;
    int m_dur_secs;
    int m_next_id;
};

#endif
