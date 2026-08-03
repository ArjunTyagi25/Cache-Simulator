#pragma once

#include <vector>
#include <variant>
#include <optional>

#include "page_table_entry.hpp"

class page_table
{
    private:
        /// @brief Number of entries in the page table
        size_t num_entries;
        /// @brief Vector containing the content of the page table 
        std::vector<std::variant<std::monostate, page_table*, page_table_entry*>> page_table_;

    public:
        /**
         * @brief Construct a new page table object
         * 
         * @param num_entries_ Number of entries in a page table
         * @param last_level_page_table_ `true` if this is the lowest level page table in a multi-level page table; `false` otherwise
         */
        page_table(size_t num_entries_,
                   bool last_level_page_table_);

        /**
         * @brief Destroy the page table object
         */
        ~page_table();

        /**
         * @brief Find and fetch an entry from the page table
         * 
         * @param index_ Index number in the page table that you want to fetch
         * @return `std::variant<std::monostate, page_table*, page_table_entry*>` i.e., either monostate representing empty slot, pointer to a `page_table`, or a pointer to a `page_table_entry`
         */
        std::variant<std::monostate, page_table*, page_table_entry*> find_entry(size_t index_);

        /**
         * @brief Insert a new entry into the page table
         * 
         * @param index_ Index number in the page table where you want to insert the entry
         * @param entry_ Pointer to the entry, which could either be `page_table*` or `page_table_entry*`
         */
        void insert_entry(size_t index_, 
                          std::variant<page_table_entry*, page_table*> entry_);
};