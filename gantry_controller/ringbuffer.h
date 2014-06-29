#pragma once

// Data structure for ring buffer used in serial communication

#define RB_SIZE 256

struct RingBuffer{
  char data[RB_SIZE];
  int rd_indx;
  int wr_indx;
};
