//
// Created by Rob Ballantyne on 2023-12-16.
//

#ifndef MRTPROCESSOR_FILTERS_H
#define MRTPROCESSOR_FILTERS_H

#include <vector>

#include <bgpdump_lib.h>

class filter {
public:
    virtual bool filt(BGPDUMP_ENTRY* bgp) = 0;
};

class asnfilter : public filter {
private:
    std::vector<uint32_t> asns;
public:
    asnfilter(std::vector<uint32_t> asns) : asns(asns) {};
    virtual bool filt(BGPDUMP_ENTRY* bgp);
};

#endif //MRTPROCESSOR_FILTERS_H
