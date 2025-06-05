//
// Created by Osprey on 6/4/2025.
//

#include "io.h"

#include <iostream>

const char* IO::ReadFileGLSL(const std::string& filename){
    std::ifstream::pos_type size;
    char * memblock;
    std::string text;

    // file read based on example in cplusplus.com tutorial
    std::ifstream file (filename, std::ios::in| std::ios::binary| std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char [1 + size];
        file.seekg (0, std::ios::beg);
        file.read (memblock, size);
        file.close();
        memblock[size] = '\0';
        std::cout << "file " << filename << " loaded" << std::endl;
        text.assign(memblock);
    }
    else
    {
        std::cout << "Unable to open file " << filename << std::endl;
        exit(1);
    }
    return memblock;
}
