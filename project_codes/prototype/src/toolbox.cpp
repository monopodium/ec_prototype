#include "toolbox.h"
int ECProject::bytes_to_int(std::vector<unsigned char> &bytes)
{
  int integer;
  unsigned char *p = (unsigned char *)(&integer);
  for (int i = 0; i < int(bytes.size()); i++)
  {
    memcpy(p + i, &bytes[i], 1);
  }
  return integer;
}
int ECProject::value_to_blocks_size(int value_size,int k)
{
    int block_size_byte = ceil(value_size,k);
    block_size_byte = 16 * ceil(block_size_byte, 16);
    return block_size_byte;
}
std::vector<unsigned char> ECProject::int_to_bytes(int integer)
{
  std::vector<unsigned char> bytes(sizeof(int));
  unsigned char *p = (unsigned char *)(&integer);
  for (int i = 0; i < int(bytes.size()); i++)
  {
    memcpy(&bytes[i], p + i, 1);
  }
  return bytes;
}