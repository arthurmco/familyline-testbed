
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

        Unit(int oid, int tid, const char* name);
        Unit(int oid, int tid, const char* name,
            float x, float y, float z);
        Unit(int oid, int tid, const char* name,
            float x, float y, float z, int maxHP,
            float baseAtk, float baseArmor);
        Unit(int oid, int tid, const char* name,
            float x, float y, float z, int maxHP,
            float baseAtk, float baseArmor,
            float speed, float armatureMaterial,
            float piercingAttack);

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
