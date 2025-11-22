#pragma once
#include <iostream>
#include <string>
#include <vector>

#ifdef __3DS__
#include <3ds.h>
#else
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
#endif

namespace Pica {
void InstallErrorCallback(void (*ErrorHandler)(const char* top,
                                               const char* message));
std::vector<u8> AssembleCode(const std::string& vertex);
std::vector<u8> AssembleFile(const std::string& file);
}  // namespace Pica