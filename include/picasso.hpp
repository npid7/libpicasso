#pragma once
#include <iostream>
#include <string>

namespace Pica
{
    void InstallErrorCallback(void(*ErrorHandler)(const char* top, const char* message));
    char* AssembleCode(const char* vertex, int &res_size);
    char* AssembleFile(const char* file, int &res_size);
}