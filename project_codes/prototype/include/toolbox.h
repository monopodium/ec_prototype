#ifndef TOOLBOX_H
#define TOOLBOX_H

#include "meta_definition.h"
namespace ECProject
{ 
 
  int bytes_to_int(std::vector<unsigned char> &bytes);
  std::vector<unsigned char> int_to_bytes(int);
  int value_to_blocks_size(int value_size,int k);
}
#endif