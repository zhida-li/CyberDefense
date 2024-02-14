//
// Created by Rob Ballantyne on 2023-12-16.
//

#ifndef MRTPROCESSOR_UTIL_H
#define MRTPROCESSOR_UTIL_H

#include <string>
#include <vector>

#include "filters.h"

std::vector<uint32_t> path_to_vector(std::string path, char delim);
std::vector<uint32_t> path_to_vector(std::string path);
std::vector<uint32_t> asnlist_to_vec(std::string path);
std::vector<nlriv4> nlriv4list_to_vec(std::string nlris);

nlriv4 str_to_nlriv4(std::string nlri);
std::vector<nlriv4> nlriv4list_to_vec(std::string nlris);
std::vector<nlriv6> nlriv6list_to_vec(std::string nlris);

int edit_distance(std::vector<uint32_t>& p1, std::vector<uint32_t>& p2);
#endif //MRTPROCESSOR_UTIL_H
