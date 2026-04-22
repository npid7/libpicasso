#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Pica {
using u8 = uint8_t;
void InstallErrorCallback(void (*ErrorHandler)(const char *top,
                                               const char *message));
std::vector<u8> AssembleCode(const std::string &vertex);
std::vector<u8> AssembleFile(const std::string &file);
} // namespace Pica