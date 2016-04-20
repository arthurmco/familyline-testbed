
#include "AttackableObject.hpp"

using namespace Tribalia::Logic;


AttackableObject::AttackableObject(int oid, int tid, const char* name)
    : LocatableObject(oid, tid, name)
{
    this->AddProperty("maxHP", 0);
    this->AddProperty("HP", 0.0f);
    this->AddProperty("baseAtk", 0.0f);
    this->AddProperty("baseArmor", 0.0f);

}
AttackableObject::AttackableObject(int oid, int tid, const char* name,
        float x, float y, float z)
        : LocatableObject(oid, tid, name, x, y, z)
{
    this->AddProperty("maxHP", 0);
    this->AddProperty("HP", 0.0f);
    this->AddProperty("baseAtk", 0.0f);
    this->AddProperty("baseArmor", 0.0f);

}
AttackableObject::AttackableObject(int oid, int tid, const char* name,
        float x, float y, float z, int maxHP,
        float baseAtk, float baseArmor)
        : LocatableObject(oid, tid, name, x, y, z)
{
    this->AddProperty("maxHP", maxHP);
    this->AddProperty("HP", (float)maxHP);
    this->AddProperty("baseAtk", baseAtk);
    this->AddProperty("baseArmor", baseArmor);
}

int AttackableObject::GetMaxHP(){
    return this->GetProperty<int>("maxHP");
}
float AttackableObject::GetHP(){
    return this->GetProperty<float>("HP");
}
float AttackableObject::GetBaseAttack(){
    return this->GetProperty<float>("baseAtk");
}
float AttackableObject::GetBaseArmor(){
    return this->GetProperty<float>("baseArmor");
}
