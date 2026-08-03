#pragma once

#include <cstdio>
#include <variant>

class page_table_entry
{
    private:
        /// @brief Dirty bit of the entry
        bool dirty_bit;
        /// @brief Valid bit of the entry
        bool valid_bit;
        /// @brief Physical Page Number (PFN)
        size_t PFN;
        /// @brief Virtual Page Number (VPN)
        size_t VPN;
        
    public:
        /**
         * @brief Construct a new `page_table_entry` object
         * 
         * @param VPN_ Virtual Page Number (VPN) associated with the entry
         * @param PFN_ Physical Frame Number (PFN) associated with the entry
         * @param dirty_bit_ Dirty bit of the entry
         * @param valid_bit_ Valid bit of the entry
         */
        page_table_entry(size_t VPN_,
                         size_t PFN_,
                         bool dirty_bit_,
                         bool valid_bit_);

        /**
         * @brief Destroy the page table entry object
         */
        ~page_table_entry() = default;

        /**
         * @brief Update the entry with new values
         * 
         * @param VPN_ 
         * @param PFN_ 
         * @param dirty_bit_ 
         * @param valid_bit_ 
         */
        void update_entry(size_t VPN_,
                          size_t PFN_,
                          bool dirty_bit_,
                          bool valid_bit_);

        /**
         * @brief Get the VPN associated with the entry
         * 
         * @return size_t 
         */
        size_t get_VPN();
        /**
         * @brief Return the PFN associated with the entry
         * 
         * @return `size_t` representing the PFN
         */
        size_t get_PFN();

        /**
         * @brief Get the dirty bit 
         * 
         * @return `bool` representing the dirty bit 
         */
        bool get_dirty_bit();

        /**
         * @brief Get the valid bit
         * 
         * @return `bool` representing the valid bit 
         */
        bool get_valid_bit();
        
        /**
         * @brief Set the dirty bit to `true` or `false`
         * 
         * @param dirty_bit_ Desired value of dirty bit
         */
        void set_dirty_bit(bool dirty_bit_);

        /**
         * @brief Set the valid bit to `true` to `false`
         * 
         * @param valid_bit_ Desired value of valid bit
         */
        void set_valid_bit(bool valid_bit_);
};