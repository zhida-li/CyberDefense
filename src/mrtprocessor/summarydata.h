//
// Created by Rob Ballantyne on 2023-11-05.
//

#ifndef MRTPROCESSOR_SUMMARYDATA_H
#define MRTPROCESSOR_SUMMARYDATA_H


#include <ctime>
#include <vector>
#include <set>

extern "C" {
#include <bgpdump_lib.h>
}



class summarydata {

public:
    time_t begin;
    std::vector<BGPDUMP_ENTRY*> bgp_entries;
    std::set<std::vector<uint32_t>> unique_as_paths;
    int count;
    int count_aspaths;
    int count_unique_as;
    int announcements;
    int withdrawls;
    int announced_prefixes;
    int withdrawn_prefixes;
    double avg_aspath_length;
    double max_aspath_length;
    double avg_unique_aspath;
    double max_unique_aspath;
    int duplicate_announcements;
    int duplicate_withdrawls;
    int nada;
    int implicit_withdrawls;
    int max_path_edit_distance;
    int avg_path_edit_distance;
    int min_path_edit_distance;
    int igps;
    int egps;
    int incompletes;
    int opens;
    int keepalives;
    int updates;
    int notifications;
    int avgsize;

public:
    summarydata();
    void reset();
};

void process_unique_paths(summarydata* sd);


#endif //MRTPROCESSOR_SUMMARYDATA_H
