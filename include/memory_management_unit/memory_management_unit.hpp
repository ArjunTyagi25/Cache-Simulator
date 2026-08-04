#pragma once

#include <iostream>
#include <vector>

#include "page_table_entry.hpp"
#include "page_table.hpp"
#include "TLB.hpp"
#include "address_translation_info.hpp"
#include "../memory/memory.hpp"

class memory_management_unit
{
    private:
        /// @brief Number of bits in the virtual address
        size_t num_bits_virtual_address;
        /// @brief Number of bits in the physical address
        size_t num_bits_physical_address;
        /// @brief Number of levels in the page table
        size_t num_levels_page_table;
        /// @brief Size of a page table entry (PTE) in bytes (B)
        size_t PTE_size;
        /// @brief Size of the TLB in bytes (B)
        size_t TLB_size;
        /// @brief Allocation policy when requesting a free physical frame from memory
        std::string allocation_policy;
        /// @brief Size of the page in bytes (B)
        size_t page_size;
        /// @brief Number of bits in the page offset
        size_t page_offset_bits;
        /// @brief Mask to extract the page offset
        size_t page_offset_mask;
        /// @brief Number of PTE entries in a single page table
        size_t num_PTE_per_page_table;
        /// @brief Total number of page tables in the MMU
        size_t total_num_of_page_tables;
        /// @brief For each level, the number of bits to index into that level's page table
        size_t num_bits_in_per_level_index;
        /// @brief Pointer to the root page table
        page_table* root_table;
        /// @brief Pointer to the TLB
        TLB* tlb;

        /**
         * @brief Perform the page table walk to find a particular entry
         * 
         * @param page_table_indices_ `vector` representing the indices to index into each level
         * @return page_table_entry* representing the entry if it was found; otherwise `nullptr` 
         */
        page_table_entry* page_table_walk(std::vector<size_t> page_table_indices_);

        /**
         * @brief Request a free physical frame from the memory
         * 
         * @param memories_ `vector` representing all levels of memory
         * @return `size_t` representing the PFN
         */
        size_t request_physical_page(std::vector<memory*> memories_);

        /**
         * @brief Insert a PTE into the page table
         * 
         * @param VPN_ VPN associated with the PTE that is to be entered
         * @param PFN_ PFN associated with the PTE that is to be entered
         * @param dirty_bit_ Dirty bit of the PTE that is to be entered
         * @param page_table_indices_ Indices extracted from the VPN and used to index into each level of page table
         */
        void insert_entry_in_page_table(size_t VPN_,
                                        size_t PFN_,
                                        bool dirty_bit_,
                                        std::vector<size_t> page_table_indices_);
                                        
    public:
        /**
         * @brief Construct a new memory management unit object
         * 
         * @param num_bits_virtual_address_ Number of bits in the virtual address
         * @param num_bits_physical_address_ Number of bits in the physical address
         * @param num_levels_page_table_ Number of levels of page table
         * @param PTE_size_ Size of the PTE in bytes (B)
         * @param TLB_size_ Size of the TLB in bytes (B)
         * @param page_size_ Size of the page in bytes (B)
         * @param allocation_policy_ Allocation policy used when requesting a free physical frame from memory
         */
        memory_management_unit(size_t num_bits_virtual_address_,
                               size_t num_bits_physical_address_,
                               size_t num_levels_page_table_,
                               size_t PTE_size_,
                               size_t TLB_size_,
                               size_t page_size_,
                               std::string allocation_policy_);

        /**
         * @brief Destroy the memory management unit object
         */
        ~memory_management_unit();

        /**
         * @brief Translate a virtual address to physical address
         * 
         * @param VA_ Virtual address to be translated
         * @param is_write_ `true` if operation is write; otherwise `false`
         * @param memories_ `vector` representing all levels of memory
         * @return `size_t` representing the physical address 
         */
        size_t address_translate(size_t VA_,
                                 bool is_write_,
                                 std::vector<memory*> memories_);

        /**
         * @brief Print the contents of the TLB
         */
        void print_TLB_data();
};