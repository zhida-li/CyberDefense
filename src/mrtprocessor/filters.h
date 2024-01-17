//
// Created by Rob Ballantyne on 2023-12-16.
//

#ifndef MRTPROCESSOR_FILTERS_H
#define MRTPROCESSOR_FILTERS_H

#include <vector>

extern "C" {
#include <bgpdump_lib.h>
#include <arpa/inet.h>
};

class filter {
public:
    virtual bool filt(BGPDUMP_ENTRY* bgp) = 0;
};

class asnfilter : public filter {
private:
    std::vector<uint32_t> asns;
public:
    asnfilter(std::vector<uint32_t> asns) : asns{asns} {};
    virtual bool filt(BGPDUMP_ENTRY* bgp);
};


class nlriv4 {
private:
    struct in_addr addr;
    uint32_t len;
public:
    nlriv4(struct in_addr addr, uint32_t len): addr{addr}, len{len} {};
    bool operator==(nlriv4 &other) { return addr.s_addr == other.addr.s_addr && len == other.len; };
    struct in_addr get_addr() { return addr; }
    uint32_t get_len() { return len; }
};

class nlriv4filter : public filter {
private:
    std::vector<nlriv4> nlris;
    bool search_nlri(struct in_addr addr, uint32_t len);

public:
    nlriv4filter(std::vector<nlriv4> nlris) : nlris{nlris} {};
    virtual bool filt(BGPDUMP_ENTRY *bgp);
};

#endif //MRTPROCESSOR_FILTERS_H
