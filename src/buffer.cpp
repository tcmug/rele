
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "buffer.hpp"

using namespace rele;



byte_buffer::byte_buffer(int sz) {
	this->used = 0;
	this->size = sz;
	this->last_read = 0;
	this->data = new char[this->size + 1];
}


byte_buffer::~byte_buffer() {
	delete[] this->data;
}


bool byte_buffer::read_socket(rele::net_socket &socket) {
	assert(this->size >= this->used);
	this->last_read = socket.read(&this->data[this->used], this->size - this->used);
	this->used += this->last_read;
	return this->last_read != -1 && this->last_read >= 0;
}


int byte_buffer::shift(int bytes) {
	assert(this->used >= bytes);
	memcpy(this->data, &this->data[bytes], this->used - bytes);
	this->used -= bytes;
	return this->used;
}
