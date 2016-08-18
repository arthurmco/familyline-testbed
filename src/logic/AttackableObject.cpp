
#include "AttackableObject.hpp"

using namespace Tribalia::Logic;


AttackableObject::AttackableObject(int oid, int tid, const char* name)
    : LocatableObject(oid, tid, name)
{
    this->AddProperty("maxHP", 0);
    this->AddProperty("HP", 0.0f);
    this->AddProperty("baseAtk", 0.0f);
    this->AddProperty("baseArmor", 0.0f);
    this->AddProperty("bonusUnitAtk", 1.0f);
    this->AddProperty("bonusBasicBuildingAtk", 1.0e-1f);
    this->AddProperty("bonusMediumBuildingAtk", 1.0e-2f);
    this->AddProperty("bonusAdvancedBuildingAtk", 1.0e-3f);

}
AttackableObject::AttackableObject(int oid, int tid, const char* name,
        float x, float y, float z)
        : LocatableObject(oid, tid, name, x, y, z)
{
    this->AddProperty("maxHP", 0);
    this->AddProperty("HP", 0.0f);
    this->AddProperty("baseAtk", 0.0f);
    this->AddProperty("baseArmor", 0.0f);
    this->AddProperty("bonusUnitAtk", 1.0f);
    this->AddProperty("bonusBasicBuildingAtk", 1.0e-1f);
    this->AddProperty("bonusMediumBuildingAtk", 1.0e-2f);
    this->AddProperty("bonusAdvancedBuildingAtk", 1.0e-3f);

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
    this->AddProperty("bonusUnitAtk", 1.0f);
    this->AddProperty("bonusBasicBuildingAtk", 1.0e-1f);
    this->AddProperty("bonusMediumBuildingAtk", 1.0e-2f);
    this->AddProperty("bonusAdvancedBuildingAtk", 1.0e-3f);
}

int AttackableObject::GetMaxHP(void){
    return this->GetProperty<int>("maxHP");
}
float AttackableObject::GetHP(void){
    return this->GetProperty<float>("HP");
}
float AttackableObject::GetBaseAttack(void){
    return this->GetProperty<float>("baseAtk");
}
float AttackableObject::GetBaseArmor(void){
    return this->GetProperty<float>("baseArmor");
}

float AttackableObject::SetBaseAttack(float f) {
	float bAtk = this->GetProperty<float>("baseAtk");
	bAtk += f;
	this->SetProperty("baseAtk", bAtk);
	return bAtk;
}

float AttackableObject::SetBaseArmor(float f) {
	float bArmor = this->GetProperty<float>("baseArmor");
	bArmor += f;
	this->SetProperty("baseArmor", bArmor);
	return bArmor;
}

/* Increase HP until maximum */
float AttackableObject::Heal(float val)
{
    float f = this->GetHP();
    float max = this->GetMaxHP();

    f = std::min(f + val, max);

    this->SetProperty("HP", f);
	return f;

}

/* Decrease HP until minimum */
float AttackableObject::Damage(float val)
{
    float f = this->GetProperty<float>("HP");

    f =  std::max(f + val, 0.0f);

    this->SetProperty("HP", f);
	return f;
}

float AttackableObject::GetUnitBonus(void) {
    return this->GetProperty<float>("bonusUnitAtk");
}
float AttackableObject::GetBasicBuildingBonus(void) {
	return this->GetProperty<float>("bonusBasicBuildingAtk");
}
float AttackableObject::GetMediumBuildingBonus(void) {
	return this->GetProperty<float>("bonusMediumBuildingAtk");
}
float AttackableObject::GetAdvancedBuildingBonus(void) {
	return this->GetProperty<float>("bonusAdvancedBuildingAtk");
}
