#ifndef ELEPSEDTIMER_H
#define ELEPSEDTIMER_H

#include <sys\timeb.h>

class ElepsedTimer
{
public:
    ElepsedTimer(){this->reset();}
    void reset(){
        ftime(&m_start);
    }
    int elepsedMiliSec(){
        timeb end;
        int diff;
        ftime(&end);
        diff = (int) (1000.0 * (end.time - m_start.time)
            + (end.millitm - m_start.millitm));
        return diff;
    }

private:
    struct timeb m_start;
};

#endif // ELEPSEDTIMER_H
