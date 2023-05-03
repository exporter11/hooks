#pragma once
#include <Windows.h>
#include <string>
#include <psapi.h>
#include <processthreadsapi.h>

// this is fully made by me(romeo)
// 
// the pattern is made of 4 parts: START-> bytes; wantedValue -> blankSpace; END-> bytes after wanted value; endTAG -> " XEND"
// examplePATTERN "\x48\xC7\xC0\x37\x00\x00\x00\x48\xC7\xC0\x15\x00\x00\x00\x48\x8B\xC1\x48\x8B\x05\x08\x39\x00\x00\x48\xB8 YOU WILL GET ANYTHING THATS IN BETWEEN THOSE BYTES \x48\x8B\x05\xF7 XEND"
// if you watn 1 byte of data you take it as it if was last byte of INT(x86) or INT64a(x64)
/*
    * class T               IS A TYPE OF VARTIABLE U ARE FINDING IN MEMORY
    * typename D            IS A TYPE OF YOUR VARIABLE IN WHICH YOU WANT TO SAVE THE POINTER OF VARIABLE YOU ARE TRYING TO FIND    

*/
template <class T, typename D> 
class patternScanner
{
public:
	T m_tValue;
    UINT getPatternLength(const char* pattern);
    BOOL getMINFO(MODULEINFO* mInfo, const char* moduleName);
    patternScanner(const char* pattern, const char* moduleName, D* pointer = 0);
};
