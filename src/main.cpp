#include<iostream>
#include<vector>
#include<stdlib.h>
#include<fstream>
#include<sstream>
#include<cstdint>
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
    if (argc < 15)
    {
        cerr << "Missing arguments. Usage: " << argv[0] << " --memory_size <memory size in kB> --page_size <page size in kB> --cache_size <cache size in kB> --line_size <line size in kB> --assoc <associativity> --replacement_policy <specify the replacement policy for the cache> --trace_file <path to trace file>" << endl;
        return -1;
    }
    size_t memory_size = 8192;
    size_t page_size = 4096;
    size_t cache_size = 64;
    size_t line_size = 8;
    size_t assoc = 4;
    string replacement_policy = "first_line";
    string trace_file_path;

    for (int i = 1; i < 14; i += 2)
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
        else
        {
            cerr << "Undefined flag. Usage: " << argv[0] << " --memory_size <memory size in kB> --page_size <page size in kB> --cache_size <cache size in kB> --line_size <line size in kB> --assoc <associativity> --replacement_policy <specify the replacement policy for the cache> --trace_file <path to trace file>" << endl;
            return -1;
        }        
    }

    size_t line_offset_bits = log2(line_size);
    size_t page_offset_bits = log2(page_size);
    size_t index_bits = log2(cache_size/(line_size*assoc));
    int accesses = 0, hits = 0, misses = 0;


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
        accesses++;
        string operation = trace[lines][0];
        size_t address = static_cast<size_t>(stoul(trace[lines][1], nullptr, 16));
        size_t data;
        if (operation == "W")
            data = static_cast<size_t>(stoul(trace[lines][2], nullptr, 16));

        if (operation == "R")
        {
            cout << "-----------------------------------------------------------" << endl; 
            cout << "Trace: " << operation << " " << address << endl;
            optional<u_int8_t> result = C->find_byte(address);
            if (result.has_value())
            {
                size_t byte = static_cast<size_t>(*result);
                cout << "Read Hit! Found the data " << byte << " at address " << address << endl; 
                hits++;
            }
            else
            {
                cout << "Read Miss! Line containing address " << address << " was not found in the cache. Requesting from memory..." << endl;
                line* l = M->get_line(address);
                pair<line*, size_t> evicted_line = C->replace_line(l, address);
                if (evicted_line.first != nullptr)
                    M->write_line(evicted_line.first, evicted_line.second);
                misses++;
            }
        }
        else if (operation == "W")
        {
            cout << "-----------------------------------------------------------" << endl; 
            cout << "Trace: " << operation << " " << address << " " << data << endl;
            bool write_successful = C->write_byte(address, data);
            if (write_successful)
            {
                hits++;
                cout << "Write Hit! Found the line containing address " << address << " in the cache so writing data " << data << " at that address was successful." << endl;
            }
            else
            {
                misses++;
                cout << "Write Miss! Line containing address " << address << " was not found in the cache. Requesting from memory..." << endl;
                line* requested_line = M->get_line(address);
                pair<line*, size_t> evicted_line = C->replace_line(requested_line, address);
                if (evicted_line.first != nullptr)
                    M->write_line(evicted_line.first, evicted_line.second);

                bool write_successful = C->write_byte(address, data);
                if (write_successful)
                {
                    cout << "Found the line containing address " << address << " in the cache so writing data " << data << " at that address was successful." << endl;
                }
            }
        }
        C->print_cache_data();
        cout << "-----------------------------------------------------------" << endl;
    }

    cout << "------------------------FINAL STATS------------------------" << endl;
    cout << "Accesses: " << dec << accesses << endl;
    cout << "Hits: " << dec << hits << endl;
    cout << "Misses: " << dec << misses << endl;
    cout << "-----------------------------------------------------------" << endl;
    
    return 1;
}