#pragma once

#include "util/types.h"
#include <concepts>

inline union {
private:
	volatile u8 as_u8;
	volatile u32 as_u32;
	volatile f32 as_f32;

public:
	void write(u8 value) { as_u8 = value; }
	void write(u32 value) { as_u32 = value; }
	void write(f32 value) { as_f32 = value; }
	void write(auto ...values) { (write(values), ...); }
} *gx_fifo = (decltype(gx_fifo))0xCC008000;

extern "C" {

void GXBegin(u32 prim_type, u32 vertex_format, u16 vertex_count);
void GXSetLineWidth(u8 width, u8 texture_offset);

}