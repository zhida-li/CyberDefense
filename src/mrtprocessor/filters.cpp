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

bool nlriv4filter::search_nlri(struct in_addr addr, uint32_t len) {
    for ( auto nlri : nlris ) {
        if ( addr.s_addr == nlri.get_addr().s_addr && len == nlri.get_len() ) {
            return true;
        }
    }
    return false;
}

bool nlriv4filter::filt(BGPDUMP_ENTRY *bgp) {

    for ( int i = 0; i < bgp->body.zebra_message.announce_count; i++ ) {
        if (search_nlri(bgp->body.zebra_message.announce[i].address.v4_addr,
                        bgp->body.zebra_message.announce[i].len))
            return true;
    }
    for ( int i = 0; i < bgp->body.zebra_message.withdraw_count; i++ ) {
        if (search_nlri(bgp->body.zebra_message.withdraw[i].address.v4_addr,
                        bgp->body.zebra_message.withdraw[i].len))
            return true;
    }
    if ( ! bgp->attr ) {
        return false;
    }
    if (bgp->attr->mp_info->announce[AFI_IP][SAFI_UNICAST]) {
        for (int i = 0; i < bgp->attr->mp_info->announce[AFI_IP][SAFI_UNICAST]->prefix_count; i++) {
            if (search_nlri(bgp->attr->mp_info->announce[AFI_IP][SAFI_UNICAST]->nlri[i].address.v4_addr,
                            bgp->attr->mp_info->announce[AFI_IP][SAFI_UNICAST]->nlri[i].len))
                return true;
        }
    }
    if (bgp->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST]) {
        for ( int i = 0; i < bgp->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST]->prefix_count; i++ ) {
            if (search_nlri(bgp->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST]->nlri[i].address.v4_addr,
                            bgp->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST]->nlri[i].len) )
                return true;
        }
    }
    if (bgp->attr->mp_info->announce[AFI_IP][SAFI_MULTICAST]) {
        for (int i = 0; i < bgp->attr->mp_info->announce[AFI_IP][SAFI_MULTICAST]->prefix_count; i++) {
            if (search_nlri(bgp->attr->mp_info->announce[AFI_IP][SAFI_MULTICAST]->nlri[i].address.v4_addr,
                            bgp->attr->mp_info->announce[AFI_IP][SAFI_MULTICAST]->nlri[i].len))
                return true;
        }
    }
    if (bgp->attr->mp_info->withdraw[AFI_IP][SAFI_MULTICAST]) {
        for ( int i = 0; i < bgp->attr->mp_info->withdraw[AFI_IP][SAFI_MULTICAST]->prefix_count; i++ ) {
            if (search_nlri(bgp->attr->mp_info->withdraw[AFI_IP][SAFI_MULTICAST]->nlri[i].address.v4_addr,
                            bgp->attr->mp_info->withdraw[AFI_IP][SAFI_MULTICAST]->nlri[i].len) )
                return true;
        }
    }
    if (bgp->attr->mp_info->announce[AFI_IP][SAFI_UNICAST_MULTICAST]) {
        for (int i = 0; i < bgp->attr->mp_info->announce[AFI_IP][SAFI_UNICAST_MULTICAST]->prefix_count; i++) {
            if (search_nlri(bgp->attr->mp_info->announce[AFI_IP][SAFI_UNICAST_MULTICAST]->nlri[i].address.v4_addr,
                            bgp->attr->mp_info->announce[AFI_IP][SAFI_UNICAST_MULTICAST]->nlri[i].len))
                return true;
        }
    }
    if (bgp->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST_MULTICAST]) {
        for ( int i = 0; i < bgp->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST_MULTICAST]->prefix_count; i++ ) {
            if (search_nlri(bgp->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST_MULTICAST]->nlri[i].address.v4_addr,
                            bgp->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST_MULTICAST]->nlri[i].len) )
                return true;
        }
    }
    return false;
}

bool nlriv6filter::search_nlri(struct in6_addr addr, uint32_t len) {
    nlriv6 other(addr,len);
    for ( auto nlri : nlris ) {
        if ( nlri == other ) {
            return true;
        }
    }
    return false;
}

bool nlriv6filter::filt(BGPDUMP_ENTRY *bgp) {
    if (bgp->attr == NULL) {
        return false;
    }
    if (bgp->attr->mp_info->announce[AFI_IP6][SAFI_UNICAST]) {
        for (int i = 0; i < bgp->attr->mp_info->announce[AFI_IP6][SAFI_UNICAST]->prefix_count; i++) {
            if (search_nlri(bgp->attr->mp_info->announce[AFI_IP6][SAFI_UNICAST]->nlri[i].address.v6_addr,
                            bgp->attr->mp_info->announce[AFI_IP6][SAFI_UNICAST]->nlri[i].len))
                return true;
        }
    }
    if (bgp->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST]) {
        for ( int i = 0; i < bgp->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST]->prefix_count; i++ ) {
            if (search_nlri(bgp->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST]->nlri[i].address.v6_addr,
                            bgp->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST]->nlri[i].len) )
                return true;
        }
    }
    if (bgp->attr->mp_info->announce[AFI_IP6][SAFI_MULTICAST]) {
        for (int i = 0; i < bgp->attr->mp_info->announce[AFI_IP6][SAFI_MULTICAST]->prefix_count; i++) {
            if (search_nlri(bgp->attr->mp_info->announce[AFI_IP6][SAFI_MULTICAST]->nlri[i].address.v6_addr,
                            bgp->attr->mp_info->announce[AFI_IP6][SAFI_MULTICAST]->nlri[i].len))
                return true;
        }
    }
    if (bgp->attr->mp_info->withdraw[AFI_IP6][SAFI_MULTICAST]) {
        for ( int i = 0; i < bgp->attr->mp_info->withdraw[AFI_IP6][SAFI_MULTICAST]->prefix_count; i++ ) {
            if (search_nlri(bgp->attr->mp_info->withdraw[AFI_IP6][SAFI_MULTICAST]->nlri[i].address.v6_addr,
                            bgp->attr->mp_info->withdraw[AFI_IP6][SAFI_MULTICAST]->nlri[i].len) )
                return true;
        }
    }
    if (bgp->attr->mp_info->announce[AFI_IP6][SAFI_UNICAST_MULTICAST]) {
        for (int i = 0; i < bgp->attr->mp_info->announce[AFI_IP6][SAFI_UNICAST_MULTICAST]->prefix_count; i++) {
            if (search_nlri(bgp->attr->mp_info->announce[AFI_IP6][SAFI_UNICAST_MULTICAST]->nlri[i].address.v6_addr,
                            bgp->attr->mp_info->announce[AFI_IP6][SAFI_UNICAST_MULTICAST]->nlri[i].len))
                return true;
        }
    }
    if (bgp->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST_MULTICAST]) {
        for ( int i = 0; i < bgp->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST_MULTICAST]->prefix_count; i++ ) {
            if (search_nlri(bgp->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST_MULTICAST]->nlri[i].address.v6_addr,
                            bgp->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST_MULTICAST]->nlri[i].len) )
                return true;
        }
    }


    return false;
}