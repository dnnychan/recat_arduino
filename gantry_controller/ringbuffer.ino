#include "ringbuffer.h"

void initializeRingBuffer(struct RingBuffer * rb) {
  rb->rd_indx = 0;
  rb->wr_indx = 0;
}

int ringBufferFull(struct RingBuffer * rb) {
  return (rb->rd_indx == ((rb->wr_indx + 1) % RB_SIZE));
}

int ringBufferEmpty(struct RingBuffer * rb) {
  return (rb->rd_indx == rb->wr_indx);
}

int ringBufferRemainingSpaces(struct RingBuffer * rb) {
  if (ringBufferEmpty(rb))
    return RB_SIZE;
  else
    return ((RB_SIZE - (rb->wr_indx - rb->rd_indx)) % RB_SIZE);
}

int ringBufferEnque(struct RingBuffer * rb, char data) {
  int is_full = ringBufferFull(rb);
  if (!is_full) {
    rb->data[rb->wr_indx] = data;
    rb->wr_indx = (rb->wr_indx + 1) % RB_SIZE;
  }
  return is_full;
}

char ringBufferDeque(struct RingBuffer * rb) {
  char return_data = 0;
  if (!ringBufferEmpty(rb)) {
    return_data = rb->data[rb->rd_indx];
    rb->rd_indx = (rb->rd_indx + 1) % RB_SIZE;
  }
  return return_data;
}
