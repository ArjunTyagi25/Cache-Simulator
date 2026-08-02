#pragma once

#include <vector>
#include <optional>
#include <map>
#include <random>

#include "page_table_entry.hpp"

class TLB
{
    private:
        /// @brief Number of page table entries in the TLB
        size_t num_entries;
        /// @brief Vector holding all the page table entries residing in the TLB
        std::vector<page_table_entry*> TLB_content;
        /// @brief Pseudo-random number generator used for evicting entries from the TLB
        std::mt19937 gen;

    public:
        /**
         * @brief Construct a new TLB object
         * 
         * @param num_entries_ Size of the TLB in terms of the number of page table entries it is hold
         */
        TLB(size_t num_entries_);

        /**
         * @brief Find an entry in the TLB
         * 
         * @param VPN_ VPN from the virtual address
         * @return `page_table_entry*` representing the `page_table_entry` associated with the given `VPN_` 
         */
        page_table_entry* find_entry(size_t VPN_);

        /**
         * @brief Update an entry into the TLB
         * 
         * @param VPN_ VPN from the virtual address associated with the entry
         * @param PFN_ PFN from the physical address associated with the entry
         * @param dirty_bit_ Dirty bit associated with the entry
         * @param valid_bit_ Valid bit associated with the entry
         * @return `page_table_entry*` representing the evicted `page_table_entry` from the TLB, if any. 
         */
        page_table_entry* update_entry(size_t VPN_,
                                       size_t PFN_,
                                       bool dirty_bit_,
                                       bool valid_bit_);

        void print_TLB_data();
};