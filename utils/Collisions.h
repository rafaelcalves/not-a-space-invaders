#ifndef OPENGL_BABYSTEPS_COLLISIONS_H
#define OPENGL_BABYSTEPS_COLLISIONS_H

#include "Includes.h"
#include "GameObject.hpp"

bool rockOnSpaceship(GameObject* rock, GameObject* spaceship);

bool projectileOnRock(GameObject* projectile, GameObject* rock);

#endif //OPENGL_BABYSTEPS_COLLISIONS_H
