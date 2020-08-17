#pragma once

#include "int_types.h"

typedef enum _CollisionLineType {
    CollisionLineType_Platform = 0x100,
    CollisionLineType_Ledge = 0x200
} CollisionLineType;

typedef struct _CollisionLineDef {
    u16 points[2];
    u16 connected_lines[4];
    u16 flags;
    u16 type;
} CollisionLineDef;

typedef struct _CollisionLine {
    CollisionLineDef *def;
    u32 flags;
} CollisionLine;

typedef struct _CollisionParams {
    char pad0000[0x4];
    s32 vertex_count;
    CollisionLineDef *line_defs;
    int line_count;
    char pad0010[0x28 - 0x10];
    int joint_count;
} CollisionParams;