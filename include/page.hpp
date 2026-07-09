#include<vector>
#include<line.hpp>

class page
{
    public:
        page(size_t page_size_, size_t line_size);
        line* get_line(size_t VA_);
        void write_line(line* l_, size_t VA_);

    private:
        std::vector<line*> page_lines;
        size_t page_size;
        size_t line_size;
        size_t number_of_lines;
        vector<pair<size_t, line*>> address_lines_pairs;
};