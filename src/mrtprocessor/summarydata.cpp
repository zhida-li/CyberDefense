//
// Created by Rob Ballantyne on 2023-11-05.
//

#include "summarydata.h"
#include <string>

using namespace std;

void summarydata::reset() {
    begin = 0;
    count = 0;
    count_aspaths = 0;
    count_unique_as = 0;
    announcements = 0;
    withdrawls = 0;
    announced_prefixes = 0;
    withdrawn_prefixes = 0;
    avg_aspath_length = 0.0;
    max_aspath_length = 0.0;
    avg_unique_aspath = 0.0;
    max_unique_aspath = 0.0;
    duplicate_announcements = 0;
    duplicate_withdrawls = 0;
    nada = 0;
    implicit_withdrawls = 0;
    max_path_edit_distance = 0;
    avg_path_edit_distance = 0;
    min_path_edit_distance = 0;
    igps = 0;
    egps = 0;
    incompletes = 0;
    opens = 0;
    keepalives = 0;
    updates = 0;
    notifications = 0;
    avgsize = 0;
    bgp_entries.clear();
    unique_as_paths.clear();
}


void process_unique_paths(summarydata &sd) {
    int max = 0;
    int tot = 0;
    int count = 0;
    for ( const vector<uint32_t> p : sd.unique_as_paths ) {
        int cursize = p.size();
        if ( cursize > max ) {
            max = cursize;
        }
        tot += cursize;
        count++;
    }
    sd.max_unique_aspath = max;
    sd.avg_unique_aspath = sd.unique_as_paths.size() != 0 ? ((double) tot / (double) sd.unique_as_paths.size()) : 0;
}