#include "circular_queue.h"

void init_buffer(circBuff_t *p, uint8_t *ptr, uint16_t size_p)
{
	p->read_p = 0;
	p->write_p = 0;
	p->size = size_p;
	p->baseAdd = ptr;
}	

void clear_buffer(circBuff_t *p)
{	
	uint16_t i;

	p->read_p = 0;
	p->write_p = 0;
	for (i = 0; i < p->size; i++)
		*(p->baseAdd + i) = 0;
}		

uint16_t getAvblData(circBuff_t *p)
{
	uint16_t avblData = (p->write_p >= p->read_p) ?
			p->write_p - p->read_p : p->size - p->read_p + p->write_p;

	return avblData;
}

uint16_t getFreeSpace(circBuff_t *p)
{
	return (p->size - getAvblData(p));
}

void readData(circBuff_t *p, uint8_t *ptr, uint16_t size)
{
	for (uint16_t inc = 0; inc < size; inc ++) {
		*(ptr + inc) = *(p->baseAdd + p->read_p);
		p->read_p = (p->read_p + 1) % p->size;
	}
}

void writeData(circBuff_t *p, uint8_t *ptr, uint16_t size)
{	
	for (uint16_t inc = 0; inc < size; inc ++) {
		*(p->baseAdd + p->write_p) = *(ptr + inc);
		p->write_p = (p->write_p + 1) % p->size;
	}
}
