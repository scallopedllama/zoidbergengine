/**
 * @file physics.cpp
 *
 * @brief The Decapodian Physics Engine implementation.
 *
 * This file contains the physics methods that the zoidberg engine will utilize.
 *
 * @see object.h
 * @see level.h
 * @author Dan Tracy
 */

/**
 *  Copyright (c) 2010 zoidberg engine
 *
 *  This file is part of the zoidberg engine.
 *
 *  The zoidberg engine is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The zoidberg engine is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the zoidberg engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <nds.h>
#include "object.h"

static const int gravity = 10;
namespace decapod{
	bool intersection(object sprite1, object sprite2);
	bool smallProjectileCollide(const object sprite, const object bullet);
	void jump(object sprite);
};

#endif
