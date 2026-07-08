#include<cache.hpp>
#include<iostream>
#include<vector>
#include<stdlib.h>
#include<fstream>
#include<sstream>
#include<cstdint>

using namespace std;

void print_line_data(vector<u_int8_t> data)
{
    for (auto byte : data)
    {
        cout << hex << static_cast<int>(byte) << " ";
    }
    cout << endl;
}

void print_cache_data(cache* C)
{
    vector<vector<uint8_t>> cache_data = C->get_cache_data();
    vector<line*> cache_lines = C->get_cache_lines();

    cout << "------------------------------------------------------------" << endl;
    cout << "Valid\tTag\tData" << endl;
    for (size_t i = 0; i < cache_data.size(); i++)
    {
        cout << cache_lines[i]->get_valid() << "\t" << cache_lines[i]->get_tag() << "\t";
        print_line_data(cache_data[i]);
    }
    cout << "------------------------------------------------------------" << endl;
}

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
    if (argc < 9)
    {
        cerr << "Missing arguments. Usage: " << argv[0] << " --cache_size <cache size in kB> --line_size <line size in kB> --assoc <associativity> --trace_file <path to trace file>" << endl;
        return -1;
    }
    size_t cache_size = 64;
    size_t line_size = 8;
    size_t assoc = 4;
    string trace_file_path;

    for (int i = 1; i < 8; i += 2)
    {
        if (!strcmp(argv[i], "--cache_size"))
            cache_size = strtoull(argv[i+1], NULL, 10);
        else if (!strcmp(argv[i], "--line_size"))
            line_size = strtoull(argv[i+1], NULL, 10); 
        else if (!strcmp(argv[i], "--assoc"))
            assoc = strtoull(argv[i+1], NULL, 10); 
        else if (!strcmp(argv[i], "--trace_file"))
            trace_file_path = argv[i+1];
        else
        {
            cerr << "Undefined flag. Usage: " << argv[0] << " --cache_size <cache size in kB> --line_size <line size in kB> --assoc <associativity>" << endl;
            return -1;
        }        
    }

    size_t offset_bits = log2(line_size);
    size_t offset_mask = (1u << offset_bits) - 1;
    size_t index_bits = log2(cache_size/(line_size*assoc));
    size_t index_mask = (1u << index_bits) - 1;


    cout << "========================SIMULATION PARAMETERS========================" << endl;
    cout << "Cache size (kB): " << cache_size << endl;
    cout << "Line size (kB): " << line_size << endl;
    cout << "Associativity: " << assoc << endl;
    cout << "Trace path: " << trace_file_path << endl;
    cout << "Number of offset bits: " << offset_bits << endl;
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

    cache* C = new cache(cache_size, line_size, assoc); 
    
    for (size_t lines = 0; lines < trace.size(); lines++)
    {
        string operation = trace[lines][0];
        size_t address = static_cast<size_t>(stoul(trace[lines][1], nullptr, 16));
        size_t data;
        if (operation == "W")
            data = static_cast<size_t>(stoul(trace[lines][2], nullptr, 16));

        if (operation == "R")
        {
            size_t byte = static_cast<size_t>(C->find_byte(address));
            if (byte != NULL)
                cout << "Found the data " << byte << " at address: " << address << endl; 
            else
                cout << "Line containing address " << address << " was not found in the cache." << endl;
        }
        else if (operation == "W")
        {
            bool write_successful = C->write_byte(address, data);
            if (write_successful)
            {
                cout << "Found the line containing address " << address << " in the cache so writing data " << data << " at that address was successful." << endl;
            }
            else
            {
                cout << "Line containing address " << address << " is not present in the cache so need to request the line from lower-level cache" << endl;
                // Putting a dummy line for now
                size_t offset = (address & offset_mask);
                size_t tag = (address >> (index_bits + offset_bits));
                line* dummy_line = new line(line_size);
                dummy_line->write_byte(data, tag, offset);
                C->replace_line(dummy_line, address);
            }
        }

        print_cache_data(C);
    }
    
    return 1;
}