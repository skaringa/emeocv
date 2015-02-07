/*
 * Plausi.h
 *
 */

#ifndef PLAUSI_H_
#define PLAUSI_H_

#include <string>
#include <deque>
#include <utility>
#include <ctime>

class Plausi {
public:
    Plausi(double maxPower = 50. /*kW*/, int window = 13);
    bool check(const std::string & value, time_t time);
    double getCheckedValue();
    time_t getCheckedTime();
private:
    std::string queueAsString();
    double _maxPower;
    int _window;
    std::deque<std::pair<time_t, double> > _queue;
    time_t _time;
    double _value;
};

#endif /* PLAUSI_H_ */
