#include "asettosharedmemoryreader.h"

#include <memoryapi.h>

#include <stdexcept>

#define INVALID_HANDLE_VALUE -1

AsettoSharedMemoryReader::AsettoSharedMemoryReader()
{
    /*
    std::wstring name     = "Local\\acpmf_physics";
    static auto  hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, false, name.c_str());
    if (!hMapFile || hMapFile == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("OpenFileMappingA(write) fail! Error");
    }
    */
    /*
    static auto hMapFile = OpenFileMappingA(FILE_MAP_WRITE, FALSE, "Local\\acpmf_graphics");
    if (!hMapFile || hMapFile == INVALID_HANDLE_VALUE)
    {
        printf("OpenFileMappingA(write) fail! Error: %u\n", GetLastError());
        return 0;
    }
    static auto hMapFile = OpenFileMappingA(FILE_MAP_WRITE, FALSE, "Local\\acpmf_static");
    if (!hMapFile || hMapFile == INVALID_HANDLE_VALUE)
    {
        printf("OpenFileMappingA(write) fail! Error: %u\n", GetLastError());
        return 0;
    }
    */
}
