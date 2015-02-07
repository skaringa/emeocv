/*
 * RRDatabase.cpp
 *
 */

#include <rrd.h>
#include <iostream>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#include "RRDatabase.h"

RRDatabase::RRDatabase(const char* filename) {
    _filename = new char[strlen(filename) + 1];
    strcpy(_filename, filename);
}

RRDatabase::~RRDatabase() {
    delete[] _filename;
}

int RRDatabase::update(time_t time, double counter) {
    char values[256];
    snprintf(values, 255, "%ld:%.1f:%.0f", (long)time, counter/*kWh*/, counter * 3600000. /*Ws*/);

    char *updateparams[] = { "rrdupdate", _filename, values, NULL };

    rrd_clear_error();
    int res = rrd_update(3, updateparams);
    if (res) {
        log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << rrd_get_error();
    }

    return res;
}
