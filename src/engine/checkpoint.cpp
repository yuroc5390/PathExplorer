/*
 * Copyright (C) 2013  Ta Thanh Dinh <thanhdinh.ta@inria.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "checkpoint.h"
#include "../analysis/dataflow.h"

namespace engine
{

using namespace analysis;
extern boost::shared_ptr<dataflow> 						program_dataflow;
extern boost::unordered_map<UINT32, ADDRINT>  execution_order_address_map;

/**
 * @brief a checkpoint is created before the instruction (pointed by the current address) executes. 
 * 
 * @param current_address the address pointed by the instruction pointer (IP).
 * @param current_context the cpu context (values of registers) when the IP is at this address.
 */
checkpoint::checkpoint(UINT32 execution_order, CONTEXT* current_context)
{
	// save the execution order,
	this->execution_order = execution_order;
  
  // the current cpu context,
  PIN_SaveContext(current_context, &(this->cpu_context));
  
  // and the current memory state
  boost::unordered_map<ADDRINT, UINT8>::iterator address_value_map_iter;
  ADDRINT memory_address;
  for (address_value_map_iter = program_dataflow->address_original_value_map.begin(); 
       address_value_map_iter != program_dataflow->address_original_value_map.end(); 
       ++address_value_map_iter) 
  {
    memory_address = address_value_map_iter->first;
    this->memory_state[memory_address].first() = address_value_map_iter->second;
    this->memory_state[memory_address].second() = *(reinterpret_cast<UINT8*>(memory_address));
  }
}


/**
 * @brief the checkpoint stores the original values at memory addresses before the executed 
 * instruction overwrites these values.
 * 
 * @param memory_written_address the beginning address that will be written.
 * @param memory_written_length the length of written addresses.
 * @return void
 */
void checkpoint::log_before_execution(ADDRINT memory_written_address, UINT8 memory_written_length)
{
	ADDRINT address;
  ADDRINT upper_bound_address = memory_written_address + memory_written_length;
  
  for (address = memory_written_address; address < upper_bound_address; ++address) 
  {
    // log the original value at this written address
    if (this->memory_change_log.find(address) == this->memory_change_log.end()) 
    {
      this->memory_change_log[address] = *(reinterpret_cast<UINT8*>(address));
    }
  
//     // update the total memory state
//     if (current_memory_state.find(address) == current_memory_state.end()) 
//     {
//       current_memory_state[address].first() = *(reinterpret_cast<UINT8*>(address));
//     }
//     current_memory_state[address].second() = *(reinterpret_cast<UINT8*>(address));
  }
  
  return;
}

} // end of engine namespace
