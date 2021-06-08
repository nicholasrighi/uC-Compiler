#pragma once

/*  System includes */
#include <iostream>
#include <fstream>
#include <string>
#include <list>

/*
  This class is responsible for taking a container of 3 addr codes and generating an assembly file 
  that implements those 3 addr codes
*/
class Reg_allocator
{

public:
  /*  Creates a reg allocator that generates an assembly file with the specified name */
  Reg_allocator(std::string file_name);

  /*  
      Takes an input list of 3 addr codes, converts the 3 addr codes to x86-64 assembly, and allocates
      physical resgisters to the resulting assembly 
  */
  void gen_asm(std::vector three_addr_codes);

private:
  std::ofstream asm_file;
};