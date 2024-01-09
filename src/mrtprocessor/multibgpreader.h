//
// Created by Rob Ballantyne on 2023-12-01.
//

#ifndef MRTPROCESSOR_MULTIBGPREADER_H
#define MRTPROCESSOR_MULTIBGPREADER_H

#include <vector>
#include <string>

#include "filters.h"

extern "C" {
#include <bgpdump_lib.h>
}

class multibgpreader {

private:
    std::vector<std::string> files;
    bool filtering;
    bool runfilter(BGPDUMP_ENTRY* e);
    std::vector<filter *> filts;
    BGPDUMP*    current_file;

public:
    multibgpreader(std::vector<std::string> files);
    multibgpreader() = delete;

    void setfilters(std::vector<filter *> fs) {
        filts = fs;
        filtering = true;
    }

    BGPDUMP_ENTRY* get_next_record();
};


#endif //MRTPROCESSOR_MULTIBGPREADER_H
