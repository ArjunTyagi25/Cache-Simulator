#include <iostream>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <cstring>
#include <cmath>
#include "../include/memory_subsystem.hpp"

using namespace std;

vector<vector<string>> read_trace_file(string trace_file_path)
{
    ifstream trace_file(trace_file_path);

    if (!trace_file)
    {
        cerr << "Unable to open trace file!" << trace_file_path << endl;
    }

    vector<vector<string>> trace;
    string line;

    while(getline(trace_file, line))
    {
        istringstream iss(line);
        vector<string> tokens;
        string token;

        while (iss >> token)
        {
            tokens.push_back(token);
        }

        trace.push_back(tokens);
    }

    return trace;
}

int main(int argc, char** argv)
{
    if (argc < 21)
    {
        cerr << "Missing arguments. Usage: " << argv[0] << " --memory_size <memory size in kB> --page_size <page size in kB> --cache_size <cache size in kB> --line_size <line size in kB> --assoc <associativity> --replacement_policy <specify the replacement policy for the cache> --trace_file <path to trace file> --write_policy <write_back/write_through> --write_allocate<true/false> --verbose true" << endl;
        return -1;
    }
    size_t memory_size = 8192;
    size_t page_size = 4096;
    size_t cache_size = 64;
    size_t line_size = 8;
    size_t assoc = 4;
    string replacement_policy = "first_line";
    string write_policy = "write_back";
    bool write_allocate = false;
    string trace_file_path = "../test/trace.txt";
    bool verbose = false;

    for (int i = 1; i < 20; i += 2)
    {
        if (!strcmp(argv[i], "--memory_size"))
            memory_size = strtoull(argv[i+1], NULL, 10);
        else if (!strcmp(argv[i], "--page_size"))
            page_size = strtoull(argv[i+1], NULL, 10); 
        else if (!strcmp(argv[i], "--cache_size"))
            cache_size = strtoull(argv[i+1], NULL, 10);
        else if (!strcmp(argv[i], "--line_size"))
            line_size = strtoull(argv[i+1], NULL, 10); 
        else if (!strcmp(argv[i], "--assoc"))
            assoc = strtoull(argv[i+1], NULL, 10); 
        else if (!strcmp(argv[i], "--replacement_policy"))
            replacement_policy = argv[i+1];
        else if (!strcmp(argv[i], "--trace_file"))
            trace_file_path = argv[i+1];
        else if (!strcmp(argv[i], "--verbose"))
            verbose = (!strcmp(argv[i+1], "true"));
        else if (!strcmp(argv[i], "--write_policy"))
            write_policy= argv[i+1];
        else if (!strcmp(argv[i], "--write_allocate"))
            write_allocate = (!strcmp(argv[i+1], "true")); 
        else
        {
            cerr << "Undefined flag. Usage: " << argv[0] << " --memory_size <memory size in kB> --page_size <page size in kB> --cache_size <cache size in kB> --line_size <line size in kB> --assoc <associativity> --replacement_policy <specify the replacement policy for the cache> --trace_file <path to trace file>" << endl;
            return -1;
        }        
    }

    size_t line_offset_bits = log2(line_size);
    size_t page_offset_bits = log2(page_size);
    size_t index_bits = log2(cache_size/(line_size*assoc));
    size_t index_mask = (1u << (index_bits+line_offset_bits)) - 1;
    size_t line_offset_mask = (1u << line_offset_bits) - 1;
    size_t page_offset_mask = (1u << page_offset_bits) - 1;

    cout << "========================SIMULATION PARAMETERS========================" << endl;
    cout << "Memory size (kB): " << memory_size << endl;
    cout << "Page size (kB): " << page_size << endl;
    cout << "Cache size (kB): " << cache_size << endl;
    cout << "Line size (kB): " << line_size << endl;
    cout << "Associativity: " << assoc << endl;
    cout << "Replacement Policy: " << replacement_policy << endl;
    cout << "Trace Path: " << trace_file_path << endl;
    cout << "Verbose Mode: " << verbose << endl;
    cout << "Number of page offset bits: " << page_offset_bits << endl;
    cout << "Number of line offset bits: " << line_offset_bits << endl;
    cout << "Number of index bits: " << index_bits << endl;
    cout << "=====================================================================" << endl;

    ifstream trace_file(trace_file_path);
    if (!trace_file)
    {
        cerr << "Unable to open trace file!" << trace_file_path << endl;
        return -1;
    }

    trace_file.close();

    vector<vector<string>> trace = read_trace_file(trace_file_path);

    memory_subsystem* mem_subsys = new memory_subsystem(memory_size, page_size, cache_size, line_size, assoc, replacement_policy, write_policy, write_allocate, verbose);
    
    for (size_t lines = 0; lines < trace.size(); lines++)
    {
        string operation = trace[lines][0];
        size_t address = static_cast<size_t>(stoul(trace[lines][1], nullptr, 16));
        size_t data;
        if (operation == "W")
            data = static_cast<size_t>(stoul(trace[lines][2], nullptr, 16));

        if (operation == "R")
        {
            mem_subsys->read(address);
        }
        else if (operation == "W")
        {
            mem_subsys->write(address, data);
        }
    }

    mem_subsys->report_stats();
    
    return 1;
}
