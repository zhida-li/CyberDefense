//
// Created by Rob Ballantyne on 2023-12-16.
//

#include "util.h"
#include <sstream>


using namespace std;

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