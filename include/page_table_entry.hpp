#pragma once

#include <vector>

class page_table_entry
{
    private:
        /// @brief Dirty bit of the entry
        bool dirty_bit;
        /// @brief Valid bit of the entry
        bool valid_bit;
        /// @brief Virtual Page Number (VPN)
        size_t VPN;
        /// @brief Physical Page Number (PPN)
        size_t PPN;
        
    public:
        /**
         * @brief Construct a new `page_table_entry` object
         * 
         * @param VPN_ Virtual Page Number (VPN) associated with the entry
         * @param PPN_ Physical Page Number (PPN) associated with the entry
         * @param dirty_bit_ Dirty bit of the entry
         * @param valid_bit_ Valid bit of the entry
         */
        page_table_entry(size_t VPN_,
                         size_t PPN_,
                         bool dirty_bit_,
                         bool valid_bit_);

        /**
         * @brief Returns the VPN associated with the entry
         * 
         * @return `size_t` representing the VPN 
         */
        size_t get_VPN();

        /**
         * @brief Return the PPN associated with the entry
         * 
         * @return `size_t` representing the PPN
         */
        size_t get_PPN();

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