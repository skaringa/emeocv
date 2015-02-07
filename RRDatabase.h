/*
 * RRDatabase.h
 *
 */

#ifndef RRDATABASE_H_
#define RRDATABASE_H_

#include <string>

class RRDatabase {
public:
    RRDatabase(const char* filename);
    virtual ~RRDatabase();
    int update(time_t time, double value);

private:
    char* _filename;
};

#endif /* RRDATABASE_H_ */
