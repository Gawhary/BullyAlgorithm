#ifndef ELEPSEDTIMER_H
#define ELEPSEDTIMER_H

#include <sys\timeb.h>
#include <string>
#include <time.h>

class Time
{
public:
    Time(){this->reset();}
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
    static std::string timeStamp(){
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[9];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%X", &tstruct);

        return std::string(buf);
    }

private:
    struct timeb m_start;
};

#endif // ELEPSEDTIMER_H
