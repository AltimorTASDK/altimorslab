#pragma once

#include "int_types.h"

#define GX_FIFO 0xCC008000
#define GX_WRITE_U8(value) (*(volatile u8*)GX_FIFO = value)
#define GX_WRITE_U32(value) (*(volatile u32*)GX_FIFO = value)
#define GX_WRITE_FLOAT(value) (*(volatile float*)GX_FIFO = value)

// Vertex format
#define GX_VTXFMT0 0
#define GX_VTXFMT1 1
#define GX_VTXFMT2 2
#define GX_VTXFMT3 3
#define GX_VTXFMT4 4
#define GX_VTXFMT5 5
#define GX_VTXFMT6 6
#define GX_VTXFMT7 7

// Primitive type
#define GX_QUADS 0x80
#define GX_TRIANGLES 0x90
#define GX_TRIANGLESTRIP 0x98
#define GX_TRIANGLEFAN 0xA0
#define GX_LINES 0xA8
#define GX_LINESTRIP 0xB0
#define GX_POINTS 0xB8

// Texture offset
#define GX_TO_ZERO 0
#define GX_TO_SIXTEENTH 1
#define GX_TO_EIGHTH 2
#define GX_TO_FOURTH 3
#define GX_TO_HALF 4
#define GX_TO_ONE 5
#define GX_MAX_TEXOFFSET 6

void GXBegin(u32 prim_type, u32 vertex_format, u16 vertex_count);
void GXSetLineWidth(u8 width, u8 texture_offset);