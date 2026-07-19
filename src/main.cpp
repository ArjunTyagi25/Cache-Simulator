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

vector<MemoryInfo>extract_memory_level_info(vector<string> memory_info_string_)
{
    vector<MemoryInfo> Memory_Infos;
    for (string memory_level : memory_info_string_)
    {
        stringstream ss(memory_level);
        string token;
        vector<string> result;

        while (std::getline(ss, token, ',')) 
            result.push_back(token);

        if (result.size() != 6)
        {
            throw invalid_argument(
                "Invalid memory level config. Expected 6 arguments: " + memory_level
            );
        }

        struct MemoryInfo memory_info;
        memory_info.name = result[0];
        memory_info.memory_size = stoul(result[1]);
        memory_info.page_size = stoul(result[2]);
        memory_info.line_size = stoul(result[3]);
        memory_info.read_latency = stoul(result[4]);
        memory_info.write_latency = stoul(result[5]);
        
        Memory_Infos.push_back(memory_info);
    }
    
    return Memory_Infos;
}

vector<CacheInfo> extract_cache_level_info(vector<string> cache_info_string_)
{
    vector<CacheInfo> Cache_Infos;
    for (string cache_level : cache_info_string_)
    {
        stringstream ss(cache_level);
        string token;
        vector<string> result;

        while (std::getline(ss, token, ',')) 
            result.push_back(token);

        if (result.size() != 9)
        {
            throw invalid_argument(
                "Invalid cache level config. Expected 9 arguments: " + cache_level
            );
        }

        struct CacheInfo cache_info;
        cache_info.name = result[0];
        cache_info.cache_size = stoul(result[1]);
        cache_info.line_size = stoul(result[2]);
        cache_info.assoc = stoul(result[3]);
        cache_info.replacement_policy = result[4];
        cache_info.write_policy = result[5];
        cache_info.write_allocate = (result[6] == "true");
        cache_info.read_latency = stoul(result[7]);
        cache_info.write_latency = stoul(result[8]);
        
        Cache_Infos.push_back(cache_info);
    }
    
    return Cache_Infos;
}

int main(int argc, char** argv)
{
    if (argc < 13)
    {
        cerr << "Missing arguments. Usage: " << argv[0] << " --num_memory_levels <number of memory levels> --memory_level <details of memory level> --num_cache_levels <number of levels of caches> --cache_level <details about cache level> --trace_file <path to trace file> --verbose true" << endl;
        return -1;
    }

    size_t num_memory_levels = 1;
    vector<string> memory_info_string;
    size_t num_cache_levels = 2;
    vector<string> cache_info_string;
    string trace_file_path = "../test/sample_trace.txt";
    bool verbose = false;

    for (int i = 1; i < argc; i += 2)
    {
        if (!strcmp(argv[i], "--num_memory_levels"))
            num_memory_levels = strtoull(argv[i+1], NULL, 10);
        else if (!strcmp(argv[i], "--memory_level"))
            memory_info_string.push_back(argv[i+1]);
        else if (!strcmp(argv[i], "--num_cache_levels"))
            num_cache_levels = strtoull(argv[i+1], NULL, 10);
        else if (!strcmp(argv[i], "--cache_level"))
            cache_info_string.push_back(argv[i+1]);
        else if (!strcmp(argv[i], "--trace_file"))
            trace_file_path = argv[i+1];
        else if (!strcmp(argv[i], "--verbose"))
            verbose = (!strcmp(argv[i+1], "true")); 
        else
        {
            cerr << "Undefined flag. Usage: " << argv[0] << " --memory_size <memory size in B> --page_size <page size in B> --num_cache_levels <number of levels of caches> --cache_level <details about cache level> --trace_file <path to trace file> --verbose true" << endl;
            return -1;
        }        
    }

    vector<MemoryInfo> Memory_Infos = extract_memory_level_info(memory_info_string);
    vector<CacheInfo> Cache_Infos = extract_cache_level_info(cache_info_string);
    
    bool test_case_passed = true;

    cout << "========================SIMULATION PARAMETERS========================" << endl;
    cout << "Number of Levels of Memory: " << num_memory_levels << endl;
    for (size_t level = 0; level < num_memory_levels; level++)
    {
        cout << Memory_Infos[level].name << "'s Details" << endl;
        cout << "\tMemory Size (B): " << Memory_Infos[level].memory_size << endl;
        cout << "\tPage Size (B): " << Memory_Infos[level].page_size << endl;
        cout << "\tLine Size (B): " << Memory_Infos[level].line_size << endl;
        cout << "\tRead Latency (in cycles): " << Memory_Infos[level].read_latency << endl;
        cout << "\tWrite Latency (in cycles): " << Memory_Infos[level].write_latency << endl;
    }
    cout << "Number of Levels of Cache: " << num_cache_levels << endl;
    for (size_t level = 0; level < num_cache_levels; level++)
    {
        cout << Cache_Infos[level].name << "'s Details" << endl;
        cout << "\tCache size (B): " << Cache_Infos[level].cache_size << endl;
        cout << "\tLine size (B): " << Cache_Infos[level].line_size << endl;
        cout << "\tAssociativity: " << Cache_Infos[level].assoc << endl;
        cout << "\tReplacement Policy: " << Cache_Infos[level].replacement_policy << endl;
        cout << "\tWrite Policy: " << Cache_Infos[level].write_policy << endl;
        cout << "\tWrite Allocate: " << Cache_Infos[level].write_allocate << endl;
        cout << "\tRead Latency (in cycles): " << Cache_Infos[level].read_latency << endl;
        cout << "\tWrite Latency (in cycles): " << Cache_Infos[level].write_latency << endl;
    }
    cout << "Trace Path: " << trace_file_path << endl;
    cout << "Verbose Mode: " << verbose << endl;
    cout << "=====================================================================" << endl;

    ifstream trace_file(trace_file_path);
    if (!trace_file)
    {
        cerr << "Unable to open trace file!" << trace_file_path << endl;
        return -1;
    }

    trace_file.close();

    vector<vector<string>> trace = read_trace_file(trace_file_path);

    memory_subsystem* mem_subsys = new memory_subsystem(num_memory_levels, Memory_Infos, num_cache_levels, Cache_Infos, verbose);
    
    for (size_t lines = 0; lines < trace.size(); lines++)
    {
        string operation = trace[lines][0];
        size_t address = static_cast<size_t>(stoul(trace[lines][1], nullptr, 16));
        size_t data = static_cast<size_t>(stoul(trace[lines][2], nullptr, 16));

        if (operation == "R")
        {
            size_t read_data = static_cast<size_t>(mem_subsys->read(address));
            if (read_data == data)
            {
                cout << "Read data (" << hex << read_data << ") matches with the expected value (" << hex << data << ")." << endl;
            }
            else
            {
                cout << "Read data (" << hex << read_data << ") different from the expected value (" << hex << data << ")." << endl;
                test_case_passed = false;
            }
        }
        else if (operation == "W")
        {
            mem_subsys->write(address, data);
        }
    }

    if (test_case_passed)
        cout << "The test case with the given trace passed successfully!" << endl;
    else
        cout << "The test case with the given trace failed! Check the logs." << endl;

    mem_subsys->report_stats();
    cout << "Total Latency (in cycles): " << dec << mem_subsys->total_latency << endl;
    
    return 1;
}
