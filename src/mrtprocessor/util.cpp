//
// Created by Rob Ballantyne on 2023-12-16.
//

#include "util.h"
#include <sstream>
#include <string>

extern "C" {
#include <arpa/inet.h>
}


using namespace std;

const string comma_sep {"," };
const string slash_sep {"/"};

vector<uint32_t> path_to_vector(string path, char delim) {

    vector<uint32_t> res;
    stringstream spath { path };
    string s;

    while( getline(spath, s, delim) ) {
        if ( s[0] == '{') {
            s.erase(s.length()-1);
            s.erase(0,1);
        }
        res.push_back(stoul(s));
    }
    return res;
}

vector<uint32_t> path_to_vector(string path) {
    return path_to_vector(path, ' ');
}

vector<uint32_t> asnlist_to_vec(string path) {
    return path_to_vector(path, ':');
}

nlriv4 str_to_nlriv4(string nlri) {
   int slash_pos = nlri.find("/");
   if ( slash_pos == slash_sep.npos ) {
       // error
   }
   string addr_s = nlri.substr(0,slash_pos);
   string len_s = nlri.substr(slash_pos+1);

   struct in_addr addr;
   uint32_t len;

   inet_pton(AF_INET, addr_s.c_str(), &addr);
   len = stoul(len_s);
   nlriv4 res = nlriv4(addr, len);
   return res;
}

nlriv6 str_to_nlriv6(string nlri) {
    int slash_pos = nlri.find("/");
    if ( slash_pos == slash_sep.npos ) {
        // error
    }
    string addr_s = nlri.substr(0,slash_pos);
    string len_s = nlri.substr(slash_pos+1);

    struct in6_addr addr;
    uint32_t len;

    inet_pton(AF_INET6, addr_s.c_str(), &addr);
    len = stoul(len_s);
    nlriv6 res = nlriv6(addr, len);
    return res;
}

vector<nlriv4> nlriv4list_to_vec(string nlris) {
    int cur_pos = 0;
    vector<nlriv4> res;
    int first_comma = nlris.find(",");
    if ( first_comma == comma_sep.npos ) {
        res.push_back(str_to_nlriv4(nlris));
        return res;
    } else {
        do {
            res.push_back(str_to_nlriv4(nlris.substr(cur_pos,first_comma - cur_pos)));
            cur_pos = first_comma + 1;
            first_comma = nlris.find(",", cur_pos);
        } while( first_comma != comma_sep.npos );
    }
    res.push_back(str_to_nlriv4(nlris.substr(cur_pos)));
    return res;
}

vector<nlriv6> nlriv6list_to_vec(string nlris) {
    int cur_pos = 0;
    vector<nlriv6> res;
    int first_comma = nlris.find(",");
    if ( first_comma == comma_sep.npos ) {
        res.push_back(str_to_nlriv6(nlris));
        return res;
    } else {
        do {
            res.push_back(str_to_nlriv6(nlris.substr(cur_pos,first_comma - cur_pos)));
            cur_pos = first_comma + 1;
            first_comma = nlris.find(",", cur_pos);
        } while( first_comma != comma_sep.npos );
    }
    res.push_back(str_to_nlriv6(nlris.substr(cur_pos)));
    return res;
}

int edit_distance(vector<uint32_t>& p1, vector<uint32_t>& p2) {

    int p1_size = p1.size();
    int p2_size = p2.size();

    // Dynamically create the array because it may be large!
    int *matrix = new int[(p1.size()+1) * (p2.size()+1)];
    int **d = new int*[p1.size()+1];
    for ( int i = 0; i < p1_size+1; i++ ) {
        d[i] = &matrix[i*(p2_size+1)];
    }
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

    int result = d[p1_size][p2_size];

    delete [] matrix;
    delete [] d;

    return result;
}