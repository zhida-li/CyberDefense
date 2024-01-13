//
// Created by Rob Ballantyne on 2023-12-16.
//

#include "filters.h"
#include "util.h"
#include <algorithm>

bool asnfilter::filt(BGPDUMP_ENTRY* bgp) {
    std::vector<uint32_t> aspath;
    if ( bgp && bgp->attr && bgp->attr->aspath ) {
        aspath = path_to_vector(bgp->attr->aspath->str);
    } else {
        return false;
    }

    for ( auto asn :  asns ) {
        if ( std::find(aspath.begin(), aspath.end(), asn) != aspath.end() ) {
            return true;
        }
    }
    return false;
}