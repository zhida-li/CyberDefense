//
// Created by Rob Ballantyne on 2023-12-01.
//

#include <iostream>
#include <vector>
#include <string>

#include "multibgpreader.h"

multibgpreader::multibgpreader(std::vector<std::string> files) :
       files{files}, current_file{NULL} {
    if ( files.size() == 0 ) {
        std::cout << "No files to process" << std::endl;
        exit(-1);
    }
}

BGPDUMP_ENTRY *multibgpreader::get_next_record() {
    if ( current_file == NULL ) {
        current_file = bgpdump_open_dump(files[0].c_str());
        if ( current_file == NULL ) {
            std::cout << "Cannot read file: " << files[0] << std::endl;
            exit(-1);
        }
    }
    BGPDUMP_ENTRY* result;
    while ((result = bgpdump_read_next(current_file)) == NULL &&
           files.size() > 0) {
        bgpdump_close_dump(current_file);
        files.erase(files.begin());
        if (files.empty()) {
            return NULL;
        }
        current_file = bgpdump_open_dump(files[0].c_str());
        if (current_file == NULL) {
            std::cout << "Cannot read file: " << files[0] << std::endl;
            exit(-1);
        }
    }
    return result;
}