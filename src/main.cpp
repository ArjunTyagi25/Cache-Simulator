#include<iostream>
#include<vector>
#include<stdlib.h>
#include<fstream>
#include<sstream>
#include<cstdint>
#include<cstring>
#include<cmath>
#include<cache.hpp>
#include<memory.hpp>

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
    if (argc < 17)
    {
        cerr << "Missing arguments. Usage: " << argv[0] << " --memory_size <memory size in kB> --page_size <page size in kB> --cache_size <cache size in kB> --line_size <line size in kB> --assoc <associativity> --replacement_policy <specify the replacement policy for the cache> --trace_file <path to trace file> --verbose true" << endl;
        return -1;
    }
    size_t memory_size = 8192;
    size_t page_size = 4096;
    size_t cache_size = 64;
    size_t line_size = 8;
    size_t assoc = 4;
    string replacement_policy = "first_line";
    string trace_file_path;
    bool verbose = false;

    for (int i = 1; i < 16; i += 2)
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
    cout << "Trace path: " << trace_file_path << endl;
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

    memory* M = new memory(memory_size, page_size, line_size, "random");
    cache* C = new cache(cache_size, line_size, assoc, replacement_policy); 

    cout << "--------------------Initial State of Memory--------------------" << endl;
    M->print_memory_data();
    cout << "--------------------Initial State of Cache--------------------" << endl;
    C->print_cache_data();
    
    for (size_t lines = 0; lines < trace.size(); lines++)
    {
        string operation = trace[lines][0];
        size_t address = static_cast<size_t>(stoul(trace[lines][1], nullptr, 16));
        size_t data;
        if (operation == "W")
            data = static_cast<size_t>(stoul(trace[lines][2], nullptr, 16));

        if (operation == "R")
        {
            string status;
            bool evicted = false;
            optional<u_int8_t> result = C->find_byte(address);
            if (result.has_value())
            {
                status = "HIT";
            }
            else
            {
                line* l = M->get_line(address);
                pair<line*, size_t> evicted_line = C->place_line(l, address);
                if (evicted_line.first != nullptr)
                {
                    M->write_line(evicted_line.first, evicted_line.second);
                    evicted = true;
                }
                status = "MISS";
            }

            if (verbose)
            {
                cout << "-----------------------------------------------------------" << endl; 
                cout << "\tTrace: " << operation << " " << address << endl;
                cout << "\tTag: " << hex << (address >> (index_bits + line_offset_bits)) << endl;
                cout << "\tIndex: " << hex << ((address & index_mask) >> line_offset_bits) << endl;
                cout << "\tLine Offset: " << hex << (address & line_offset_mask) << endl;
                cout << "\tPage Number: " << hex << (address >> page_offset_bits) << endl;
                cout << "\tPage Offset: " << hex << (address & page_offset_mask) << endl;
                cout << "\tStatus: " << status << endl; 
                cout << "\tEvicted: " << evicted << endl;
                if (status == "HIT")
                    cout << "\tRead Value: " << static_cast<size_t>(*result) << endl; 
                C->print_cache_data();
                cout << "-----------------------------------------------------------" << endl;
            }
        }
        else if (operation == "W")
        {
            string status;
            bool evicted = false;
            bool write_successful = C->write_byte(address, data);
            if (write_successful)
            {
                status = "HIT";
            }
            else
            {
                line* requested_line = M->get_line(address);
                requested_line->write_byte(data, (address >> (index_bits + line_offset_bits)), ((address & index_mask) >> line_offset_bits));
                pair<line*, size_t> evicted_line = C->place_line(requested_line, address);
                if (evicted_line.first != nullptr)
                {
                    M->write_line(evicted_line.first, evicted_line.second);
                    evicted = true;
                }
                status = "MISS";
            }

            if (verbose)
            {
                cout << "-----------------------------------------------------------" << endl; 
                cout << "\tTrace: " << operation << " " << address << " " << data << endl;
                cout << "\tTag: " << hex << (address >> (index_bits + line_offset_bits)) << endl;
                cout << "\tIndex: " << hex << ((address & index_mask) >> line_offset_bits) << endl;
                cout << "\tLine Offset: " << hex << (address & line_offset_mask) << endl;
                cout << "\tPage Number: " << hex << (address >> page_offset_bits) << endl;
                cout << "\tPage Offset: " << hex << (address & page_offset_mask) << endl;
                cout << "\tStatus: " << status << endl; 
                cout << "\tEvicted: " << evicted << endl;
                C->print_cache_data();
                cout << "-----------------------------------------------------------" << endl;
            }
        }
    }

    cout << "------------------------FINAL STATS------------------------" << endl;
    cout << "Total Accesses: " << dec << C->total_accesses << endl;
    cout << "Total Hits: " << dec << C->total_hits << endl;
    cout << "Total Misses: " << dec << C->total_misses << endl;
    cout << "-----------------------------------------------------------" << endl;
    
    return 1;
}
