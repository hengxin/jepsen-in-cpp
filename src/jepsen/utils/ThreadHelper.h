#ifndef THREAD_HELPER_H
#define THREAD_HELPER_H

// refer to https://stackoverflow.com/questions/10121560/stdthread-naming-your-thread
#ifdef _WIN32
#include <windows.h>
#include <thread>
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
    DWORD dwType;      // Must be 0x1000.
    LPCSTR szName;     // Pointer to name (in user addr space).
    DWORD dwThreadID;  // Thread ID (-1=caller thread).
    DWORD dwFlags;     // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)


void SetThreadName(uint32_t dwThreadID, const char* threadName);
void SetThreadName(const char* threadName);
void SetThreadName(std::thread* thread, const char* threadName);

#elif defined(__linux__)
#include <sys/prctl.h>
void SetThreadName(const char* threadName);

#else
void SetThreadName(std::thread* thread, const char* threadName);
#endif

#endif