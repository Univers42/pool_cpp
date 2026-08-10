#pragma once
#include <iostream>
#include <string>
extern const char* reset;
extern const char* bold ;
extern const char* red   ;
extern const char* green ;
extern const char* yellow;
extern const char* blue  ;
extern const char* purple;
extern const char* cyan  ;

void banner(const std::string& title,
            const char* color = cyan);