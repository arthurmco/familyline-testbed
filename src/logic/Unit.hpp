
/***
    Unit (aka 'any person') base class

    Copyright 2016 Arthur M.

***/

#include "AttackableObject.hpp"

#ifndef UNIT_H
#define UNIT_H

namespace Tribalia {
namespace Logic {

    class Unit : public AttackableObject
    {
    public:

        Unit(int oid, int tid, const char* name,
	     float x = -1, float y = -1, float z = -1, int maxHP = 1,
	     float HP = -1, float baseAtk = 0, float baseArmor = 0,
	     float speed = 1.0, float armatureMaterial = 1.0,
	     float piercingAttack = 1.0);
	Unit() : Unit(-1, -1, "") {}

        float GetSpeed(); void SetSpeed(float);
        float GetArmatureMaterial(); void SetArmatureMaterial(float);
        float GetPiercingAttack(); void SetPiercingAttack(float);

        /* Move, one tick at a time, towards a specified unit vector */
	void Move(glm::vec3 vec_dest);

	virtual ~Unit(){}

    };

}
}



#endif /* end of include guard: UNIT_H */
