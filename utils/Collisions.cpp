//
// Created by João Pedro Berton Palharini on 4/28/18.
//

#include "Collisions.h"

bool rockOnSpaceship(GameObject* rock, GameObject* spaceship) {
    float yRock = rock->vertices[7] + rock->translation[13];
    float ySpaceship = spaceship->vertices[1] + spaceship->translation[13];

    //std::cout << "Checking Collision on Y: Rock: " << yRock << " - Spaceship: " << ySpaceship << std::endl;
    if (yRock >= ySpaceship) {
        float xOneRock = rock->vertices[0] + rock->translation[12];
        float xTwoRock = rock->vertices[6] + rock->translation[12];

        float xOneSpaceship = spaceship->vertices[0] + spaceship->translation[12];
        float xTwoSpaceship = spaceship->vertices[6] + spaceship->translation[12];

        bool collisionOne = xOneRock >= xOneSpaceship && xOneRock <= xTwoSpaceship;
        bool collisionTwo = xTwoRock >= xOneSpaceship && xTwoRock <= xTwoSpaceship;

        return collisionOne || collisionTwo;
    } else {
        return false;
    }
}

bool projectileOnRock(GameObject* projectile, GameObject* rock) {
    float yProjectile = projectile->vertices[1] + projectile->translation[13];
    float yRock = rock->vertices[7] + rock->translation[13];

    //std::cout << "Checking Collision on Y: Projectile: " << yProjectile << " - Rock: " << yRock << std::endl;
    if (yProjectile <= yRock) {
        float xLeftRock = rock->vertices[0] + rock->translation[12];
        float xRightRock = rock->vertices[6] + rock->translation[12];

        float xLeftProjectile = projectile->vertices[0] + projectile->translation[12];
        float xRightProjectile = projectile->vertices[6] + projectile->translation[12];

        bool collisionLeftRock = xLeftProjectile >= xLeftRock && xLeftProjectile <= xRightRock;
        bool collisionRightRock = xRightProjectile >= xLeftRock && xRightProjectile <= xRightRock;

        return collisionLeftRock || collisionRightRock;
    } else {
        return false;
    }
}