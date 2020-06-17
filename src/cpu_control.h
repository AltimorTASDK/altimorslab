#pragma once

#include "util.h"
#include "melee.h"

void CPUControl_UpdateInput(Player *player);
void CPUControl_PhysicsMove(Physics *physics);
BOOL CPUControl_ASChange(Player *player);
void CPUControl_Init(void);