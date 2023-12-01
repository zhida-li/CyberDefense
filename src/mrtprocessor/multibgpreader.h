//
// Created by Rob Ballantyne on 2023-12-01.
//

#ifndef MRTPROCESSOR_MULTIBGPREADER_H
#define MRTPROCESSOR_MULTIBGPREADER_H

#include <vector>
#include <string>

extern "C" {
#include <bgpdump_lib.h>
}

class multibgpreader {

private:
    std::vector<std::string> files;
    BGPDUMP*    current_file;

public:
    multibgpreader(std::vector<std::string> files);
    multibgpreader() = delete;

    BGPDUMP_ENTRY* get_next_record();
};


#endif //MRTPROCESSOR_MULTIBGPREADER_H
