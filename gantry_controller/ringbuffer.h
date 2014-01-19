#pragma once

#define RB_SIZE 256

struct RingBuffer{
  char data[RB_SIZE];
  int rd_indx;
  int wr_indx;
};
