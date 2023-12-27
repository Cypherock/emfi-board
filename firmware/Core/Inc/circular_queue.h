#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <stdint.h>	

typedef struct circBuff {
	uint16_t read_p;
	uint16_t write_p;
	uint16_t size;
	uint8_t *baseAdd;	
}circBuff_t;

void init_buffer(circBuff_t *p, uint8_t *ptr, uint16_t size_p);
void clear_buffer(circBuff_t *p);
uint16_t getAvblData(circBuff_t *p);
uint16_t getFreeSpace(circBuff_t *p);
void readData(circBuff_t *p, uint8_t *ptr, uint16_t size);
void writeData(circBuff_t *p, uint8_t *ptr, uint16_t size);

#endif /*CIRCULAR_QUEUE_H*/
