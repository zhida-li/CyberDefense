#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <string>
#include <time.h>
#include <cmath>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "summarydata.h"
#include "BS_thread_pool.hpp"
#include "multibgpreader.h"

extern "C" {
#include <bgpdump_lib.h>
#include <netinet/in.h>
}

using namespace std;

void validate(vector<summarydata>& data) {
    for ( auto d : data ) {
        time_t box_begin = d.begin;
        for ( auto bgp : d.bgp_entries ) {
            if ( bgp->time - box_begin > 60 ) {
                cout << "BOX has entries out of time period!, begin=" << box_begin << " update time=" << bgp->time << " delay=" << bgp->time - box_begin << endl;
            }
        }
    }
}

void handle_announce_withdrawal(BGPDUMP_ENTRY *e, summarydata* box) {
    bool is_announce {false};
    bool is_widthdrawl {false};

    if (e->body.zebra_message.announce_count > 0 ) {
        is_announce = true;
        box->announced_prefixes += e->body.zebra_message.announce_count;
    }
    if (e->body.zebra_message.withdraw_count > 0) {
        is_widthdrawl = true;
        box->withdrawn_prefixes += e->body.zebra_message.withdraw_count;

    }
    if (e->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST] &&
        e->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST]->prefix_count ) {
        is_widthdrawl = true;
        box->withdrawn_prefixes += e->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST]->prefix_count;
    }
    if (e->attr->mp_info->withdraw[AFI_IP][SAFI_MULTICAST] &&
        e->attr->mp_info->withdraw[AFI_IP][SAFI_MULTICAST]->prefix_count ) {
        is_widthdrawl = true;
        box->withdrawn_prefixes += e->attr->mp_info->withdraw[AFI_IP][SAFI_MULTICAST]->prefix_count;
    }
    if (e->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST_MULTICAST] &&
        e->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST_MULTICAST]->prefix_count ) {
        is_widthdrawl = true;
        box->withdrawn_prefixes += e->attr->mp_info->withdraw[AFI_IP][SAFI_UNICAST_MULTICAST]->prefix_count;
    }
    if (e->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST] &&
        e->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST]->prefix_count ) {
        is_widthdrawl = true;
        box->withdrawn_prefixes += e->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST]->prefix_count;
    }
    if (e->attr->mp_info->withdraw[AFI_IP6][SAFI_MULTICAST] &&
        e->attr->mp_info->withdraw[AFI_IP6][SAFI_MULTICAST]->prefix_count ) {
        is_widthdrawl = true;
        box->withdrawn_prefixes += e->attr->mp_info->withdraw[AFI_IP6][SAFI_MULTICAST]->prefix_count;
    }
    if (e->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST_MULTICAST] &&
        e->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST_MULTICAST]->prefix_count ) {
        is_widthdrawl = true;
        box->withdrawn_prefixes += e->attr->mp_info->withdraw[AFI_IP6][SAFI_UNICAST_MULTICAST]->prefix_count;
    }

    if ( is_announce ) {
        box->announcements++;
    }
    if ( is_widthdrawl ) {
        box->withdrawls++;
    }
}

ostream& operator<<(ostream& s, const vector<uint32_t>& p) {
    if ( p.size() > 1 ) {
        s << p[0];
        for (int i = 1; i != p.size(); i++) {
            s << " " << p[i];
        }
    }
    return s;
}

ostream& operator<<(ostream& s, const vector<vector<uint32_t>>& p) {
    for ( auto a : p ) {
        s << a << endl;
    }
    return s;
}

ostream& operator<<(ostream&s, const set<vector<uint32_t>>& p) {
    for ( auto a : p ) {
        s << a << endl;
    }
    return s;
}

vector<uint32_t> path_to_vector(string path) {

    vector<uint32_t> res;
    stringstream spath { path };
    string s;

    while( getline(spath, s, ' ') ) {
        if ( s[0] == '{') {
            s.erase(s.length()-1);
            s.erase(0,1);
        }
        res.push_back(stoul(s));
    }
    return res;
}

int edit_distance(vector<uint32_t>& p1, vector<uint32_t>& p2) {

    int p1_size = p1.size();
    int p2_size = p2.size();
    int d[p1_size+1][p2_size+1];
    int subst_cost = 0;

    for ( int i = 0; i < p1_size+1; i++ ) {
        d[i][0] = i;
    }

    for (int j = 1; j < p2_size+1; j++ ) {
        d[0][j] = j;
    }

    for (int j = 1; j < p2_size+1; j++ ) {
        for ( int i = 1; i < p1_size+1; i++ ) {
            if ( p1[i-1] == p2[j-1] ) {
                subst_cost = 0;
            } else {
                subst_cost = 1;
            }
            d[i][j] =  min( d[i - 1][j] + 1, min(d[i][j - 1] + 1, d[i - 1][j - 1] + subst_cost) );
        }
    }

    return d[p1_size][p2_size];
}

bool aspath_equal(struct aspath *a, struct aspath *b) {
    if ( a == NULL ) {
        return b == NULL;
    } else if ( b == NULL ) {
        return a == NULL;
    } else {
        return strcmp(a->str, b->str) == 0;
    }
}

// Compare IP addresses for equalty for selected IPv4/IPv6
bool ipequal(u_int16_t address_family, BGPDUMP_IP_ADDRESS& a, BGPDUMP_IP_ADDRESS& b) {
    switch ( address_family ) {
        case AFI_IP: {  //IPv4
            return a.v4_addr.s_addr == b.v4_addr.s_addr;
            break;
        }
        case AFI_IP6: { //IPv6
            bool result = true;
            for (int i = 0; i < 16; i++) {
                result &= (a.v6_addr.s6_addr[i] == b.v6_addr.s6_addr[i]);
            }
            return result;
            break;
        }
        default: {
            printf("Error - trying to compare address that's neither IPv4 or IPv6");
            exit(-1);
        }
    }
}

bool ipinannoucement(u_int16_t address_family, BGPDUMP_IP_ADDRESS& ip, BGPDUMP_ENTRY* bgp_entry) {
    if ( address_family != bgp_entry->body.zebra_message.address_family ) {
        return false;
    }

    for ( int i = 0 ; i < bgp_entry->body.zebra_message.announce_count; i++ ) {
        if (ipequal(address_family, ip, bgp_entry->body.zebra_message.announce[i].address))
            return true;
    }

    return false;
}

bool ipinwithdrawal(u_int16_t address_family, BGPDUMP_IP_ADDRESS& ip, BGPDUMP_ENTRY* bgp_entry) {
    if ( address_family != bgp_entry->body.zebra_message.address_family ) {
        return false;
    }

    for ( int i = 0 ; i < bgp_entry->body.zebra_message.withdraw_count; i++ ) {
        if (ipequal(address_family, ip, bgp_entry->body.zebra_message.withdraw[i].address))
            return true;
    }

    return false;
}

void finalize_box(summarydata* sd,
                  int tot_as_path_len,
                  int max_as_path_len,
                  vector<struct_BGPDUMP_ENTRY*>& bgp_entries,
                  set<vector<uint32_t>>& unique_as_paths,
                  int bgp_size_tot,
                  time_t box_begin_time,
                  int max_unique_as_path) {

    sd->avg_aspath_length = sd->count_aspaths != 0 ? (double)tot_as_path_len / (double)sd->count_aspaths : 0;
    sd->max_aspath_length = max_as_path_len;
    sd->bgp_entries = bgp_entries;
    sd->unique_as_paths = unique_as_paths;
    sd->avgsize = sd->updates != 0 ? bgp_size_tot / sd->updates : 0;
    bgp_size_tot = 0;
    process_unique_paths(sd);
    bgp_entries.clear();
    unique_as_paths.clear();
}

struct options {
    string workdir;
    string source;
    string collector;
    string wholedate;
    string output;
    bool run_parallel;
    bool filemode;
    bool posix_timestamps;
    int year;
    int month;
    int day;
} global_opts { "cyberdefense",
                "ripe",
                "rrc04",
                "2005.10.11",
                "-",
                true,
                false,
                false,
                2005,
                10,
                11};

void usage() {
    cout << "Usage:  mrtprocessor -d WORKDIR -s DATA-SRC -c DATA-COLLECTOR YYYYMMDD -o OUTPUT\n\n"
         <<  "       mrtprocessor -o OUTPUT -f file1 file2 file3\n\n"
         <<  "           WORKDIR: directory cyberdefense is using (default cyberdefense)\n"
         <<  "           DATA-SRC: ripe or routeviews (default ripe)\n"
         <<  "           DATA-COOLECTOR: specific collector (default rrc04)\n"
         <<  "           OUTPUT: path to output file (default: stdout)\n"
         <<  "           [-np]: run non-parallel\n"
         <<  "            -f: file mode, process the files list directly, in order\n"
         <<  "            -T: output posix timestamps in feature data"
         << endl;
    exit(-1);
 }

 void process_dups_edit_dists(summarydata* box) {
     for ( int i = 0; i < box->bgp_entries.size(); i++ ) {
         for ( int j = 0; j < box->bgp_entries[i]->body.zebra_message.announce_count; j++ ) {
             for ( int k = i+1; k < box->bgp_entries.size(); k++ ) {
                 if ( ipinannoucement(box->bgp_entries[i]->body.zebra_message.address_family,
                                      box->bgp_entries[i]->body.zebra_message.announce[j].address,
                                      box->bgp_entries[k]) ) {
                     if (aspath_equal(box->bgp_entries[i]->attr->aspath, box->bgp_entries[k]->attr->aspath) ) {
                         box->duplicate_announcements++;
                     } else {
                         box->implicit_withdrawls++;
                     }
                 }
             }
         }
         for ( int j = 0; j < box->bgp_entries[i]->body.zebra_message.withdraw_count; j++ ) {
             for ( int k = i+1; k < box->bgp_entries.size(); k++ ) {
                 if ( ipinwithdrawal(box->bgp_entries[i]->body.zebra_message.address_family,
                                     box->bgp_entries[i]->body.zebra_message.withdraw[j].address,
                                     box->bgp_entries[k]) ) {
                     if (aspath_equal(box->bgp_entries[i]->attr->aspath, box->bgp_entries[k]->attr->aspath) ) {
                         box->duplicate_withdrawls++;
                     }
                 }
             }
         }
     }
     vector<vector<uint32_t>> unique_paths_v(box->unique_as_paths.begin(), box->unique_as_paths.end());
     int32_t unique_paths = unique_paths_v.size();
     int max_edit_distance = 0;
     int64_t tot_edit_distance = 0;
     int64_t count = 0;
     for ( int i = 0; i < unique_paths; i++ ) {
         for ( int j = i+1; j < unique_paths; j++ ) {
             count++;
             int dist = edit_distance(unique_paths_v[i],unique_paths_v[j]);
             tot_edit_distance += dist;
             if (dist  > max_edit_distance ) {
                 max_edit_distance = dist;
             }
         }
     }
     box->max_path_edit_distance = max_edit_distance;
     box->avg_path_edit_distance = count != 0.0 ? ceil((double)tot_edit_distance / (double)count) : 0.0;
     int entries_size = box->bgp_entries.size();
     for ( int i = 0; i < entries_size; i++ ) {
         bgpdump_free_mem(box->bgp_entries[i]);
     }
     box->bgp_entries.clear();
}


int main(int argc, char *argv[]) {

    // Process the command line and get the options into the fields

    vector<string> args(argv + 1, argv + argc);
    for ( auto it = args.begin(); it != args.end(); ) {
        if (*it == "-d") {
            it = args.erase(it);
            global_opts.workdir = *it;
            it = args.erase(it);
        } else if (*it == "-s") {
            it = args.erase(it);
            global_opts.source = *it;
            it = args.erase(it);
        } else if (*it == "-c") {
            it = args.erase(it);
            global_opts.collector = *it;
            it = args.erase(it);
        } else if (*it == "-o") {
            it = args.erase(it);
            global_opts.output = *it;
            it = args.erase(it);
        } else if (*it == "-np") {
            it = args.erase(it);
            global_opts.run_parallel = false;
        } else if (*it == "-f") {
            it = args.erase(it);
            global_opts.filemode = true;
        } else if (*it == "-T") {
            it = args.erase(it);
            global_opts.posix_timestamps = true;
        } else if ((*it).substr(0, 1) == "-") {
            cout << "Illegal option! \n\n" << endl;
            usage();
        } else {
            ++it;
        }
    }
    if (! global_opts.filemode ) {
        global_opts.wholedate = args[0];
        global_opts.year = stoi(global_opts.wholedate.substr(0, 4));
        global_opts.month = stoi(global_opts.wholedate.substr(4, 2));
        global_opts.day = stoi(global_opts.wholedate.substr(6, 2));
        global_opts.wholedate = args[0].insert(4, ".").insert(7, ".");
        if (global_opts.year == 0 || global_opts.month == 0 || global_opts.day == 0) {
            cout << "It appears your date \'" << argv[0] << "\'is not formatted YYYYMMDD (example 20101102)\n" << endl;
            usage();
        }
    }

    // If an output file is specified, make sure we output to it, otherwise it goes to stdout.
    ofstream fileout;
    if ( global_opts.output != "-" ) {
        fileout.open(global_opts.output);
    }
    ostream& output { global_opts.output == "-" ? cout : fileout };

    // Get the files to be processed from the directory and sort them into lexicographic (chronological) order.
    vector<string> files_to_process;
    if ( ! global_opts.filemode ) {
        string dirpath = std::filesystem::path(global_opts.workdir + "/" +
                                                   global_opts.source + "/" +
                                                   global_opts.collector + "/" +
                                                   global_opts.wholedate);
        if ( ! std::filesystem::is_directory(dirpath) ) {
            cout << "No directory of data at \'" << "\'." << endl;
            usage();
        }
        for (auto dirent: std::filesystem::directory_iterator(dirpath)) {
            files_to_process.push_back(dirent.path().string());
        }
        sort(files_to_process.begin(), files_to_process.end());
    } else {
        files_to_process = args;
    }

    BS::thread_pool pool;
    BGPDUMP *bgpdumphandle;
    BGPDUMP_ENTRY *e;

    vector<BGPDUMP_ENTRY*> bgp_entries;
    set<vector<uint32_t>> unique_as_paths;
    vector<summarydata*> data;
    bool initial_startup = true;
    time_t box_begin_time;
    summarydata* box;

    vector<uint32_t> as_path;
    int tot_as_path_len;
    int max_as_path_len;
    int tot_unique_as_path;
    int max_unique_as_path;
    int bgp_size_tot;

    multibgpreader bgp_reader(files_to_process);

    while ((e = bgp_reader.get_next_record()) != NULL) {

        if (e->type == BGPDUMP_TYPE_ZEBRA_BGP &&
            (e->subtype == BGPDUMP_SUBTYPE_ZEBRA_BGP_MESSAGE ||
             e->subtype == BGPDUMP_SUBTYPE_ZEBRA_BGP_MESSAGE_AS4)) {

            if (initial_startup) {

                struct tm *firsttime;
                firsttime = gmtime(&(e->time));
                firsttime->tm_sec = 0;
                box_begin_time = timegm(firsttime);

                box = new summarydata();
                tot_as_path_len = 0;
                max_as_path_len = 0;
                tot_unique_as_path = 0;
                max_unique_as_path = 0;
                bgp_size_tot = 0;
                as_path.clear();
                unique_as_paths.clear();

                box->begin = box_begin_time;

                initial_startup = false;
            }

            while (e->time >= box_begin_time + 60) {
                finalize_box(box, tot_as_path_len, max_as_path_len, bgp_entries, unique_as_paths, bgp_size_tot,
                             box_begin_time, max_unique_as_path);

                int index = data.size();
                data.push_back(box);

                if (global_opts.run_parallel) {
                    pool.push_task(process_dups_edit_dists, box);
                } else {
                    process_dups_edit_dists(box);
                }

                box_begin_time += 60;

                box = new summarydata();
                tot_as_path_len = 0;
                max_as_path_len = 0;
                tot_unique_as_path = 0;
                max_unique_as_path = 0;
                bgp_size_tot = 0;
                as_path.clear();
                unique_as_paths.clear();

                box->begin = box_begin_time;
            }

            if (e->body.zebra_message.type == BGP_MSG_UPDATE) {
                bgp_entries.push_back(e);
                box->count++;
                string as_path_str(e->attr->aspath != NULL ? e->attr->aspath->str : "");
                box->updates++;
                bgp_size_tot += e->length;

                handle_announce_withdrawal(e, box);

                if (e->attr->aspath) {
                    box->count_aspaths++;
                    as_path = path_to_vector(e->attr->aspath->str);
                    tot_as_path_len += e->attr->aspath->count;
                    unique_as_paths.insert(path_to_vector(e->attr->aspath->str));
                    if (as_path.size() > max_as_path_len) {
                        max_as_path_len = as_path.size();
                    }
                }

                switch (e->attr->origin) {
                    case 0:
                        box->igps++;
                        break;
                    case 1:
                        box->egps++;
                        break;
                    case 2:
                        box->incompletes++;
                        break;
//                        default:
//                            cout << "ERROR - unknown ORIGIN value (0=IGP, 1=EGP, 2=INCOMPLETE) was "
//                                 << e->attr->origin << endl;
                }
            }

            if (e->body.zebra_message.type == BGP_MSG_KEEPALIVE) {
                box->keepalives++;
            }

            if (e->body.zebra_message.type == BGP_MSG_NOTIFY) {
                box->notifications++;
            }

            if (e->body.zebra_message.type == BGP_MSG_OPEN) {
                box->opens++;
            }
        }
    }

    finalize_box(box, tot_as_path_len, max_as_path_len, bgp_entries, unique_as_paths, bgp_size_tot, box_begin_time,
                 max_unique_as_path);
    process_dups_edit_dists(box);
    data.push_back(box);

    time_t basetime = data[0]->begin;

//    validate(data);

    if ( global_opts.run_parallel ) {
        pool.wait_for_tasks();
    }

    for ( summarydata* d : data) {
        time_t t = d->begin - basetime;

        const int bufsz = 128;
        char buf[bufsz];
        snprintf(buf, bufsz, "%.1f", (double) (d->updates / 60.0) );

        struct tm *firsttime;
        firsttime = gmtime(&(d->begin));

        int hour = firsttime->tm_hour;
        int min = firsttime->tm_min;
        int sec = firsttime->tm_sec;

        auto filchar = cout.fill();
        auto width = cout.width();

        if ( global_opts.posix_timestamps ) {
            output << d->begin << " ";
        }
        output << setw(2) << setfill('0') << hour
               << setw(2) << setfill('0') << min << " "
               << setw(2) << setfill('0') << hour << " "
               << setw(2) << setfill('0') << min << " "
               << setw(2) << setfill('0') << sec << " ";
        output << setw(width) << setfill(filchar)
               << d->announcements << " "             // column 5
               << d->withdrawls << " "                // 6
               << d->announced_prefixes << " "        // 7
               << d->withdrawn_prefixes << " "        // 8
               << ceil(d->avg_aspath_length) << " "   // 9
               << d->max_aspath_length << " "         // 10
               << ceil(d->avg_unique_aspath) << " "         // 11 ***
               << d->duplicate_announcements << " "   // 12
               << d->implicit_withdrawls << " "       // 13
               << d->duplicate_withdrawls << " "      // 14
               << d->max_path_edit_distance << " "    // 15 ***
               << buf << " "                         // 16
               << d->avg_path_edit_distance << " ";   // 17 ***

        for ( int i = 11; i < 21; i++ ) {
            output << ( (d->max_aspath_length == i) ? "1" : "0" ) << " ";
        }

        for ( int i = 7; i < 17; i++ ) {
            output << ( (d->max_path_edit_distance == i) ? "1" : "0" ) << " ";
        }

        output << d->igps << " "                      // 38
               << d->egps << " "                      // 39
               << d->incompletes << " "               // 40
               << d->avgsize << endl;                 // 41
    }

    return 0;
}
