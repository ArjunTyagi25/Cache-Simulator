#include<cstdint>
#include<vector>

using namespace std;

class line
{
    public:
        line(size_t line_size_);
        u_int8_t get_byte(size_t offset_);
        vector<u_int8_t> get_line_data();
        void write_byte(u_int8_t write_data_, size_t tag_, size_t offset_);
        void write_line(vector<u_int8_t> write_data_, size_t tag_);
        size_t get_tag();
        bool get_valid();

    private:
        vector<u_int8_t> line_data;
        size_t tag;
        bool valid; 
        size_t line_size;
};

