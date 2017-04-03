#include "AttackableObject.hpp"

using namespace Tribalia::Logic;

int attack_seed = 0;

AttackableObject::AttackableObject(int oid, int tid, const char* name)
    : LocatableObject(oid, tid, name)
{
    DEF_PROPERTY("maxHP", 1);
    DEF_PROPERTY("HP", 1.0f);
    DEF_PROPERTY("baseAtk", 0.0f);
    DEF_PROPERTY("baseArmor", 0.0f);
    DEF_PROPERTY("bonusUnitAtk", 1.0f);
    DEF_PROPERTY("bonusBasicBuildingAtk", 1.0e-1f);
    DEF_PROPERTY("bonusMediumBuildingAtk", 1.0e-2f);
    DEF_PROPERTY("bonusAdvancedBuildingAtk", 1.0e-3f);
    DEF_PROPERTY("experience", 0);
    DEF_PROPERTY("city", nullptr);
    DEF_PROPERTY("attack_status", AST_ALIVE);

    /* This isn't reccomended, but we don't need a nice method
     * just to calculate the attack chance */
    srand((unsigned int)((uintptr_t)this & 0xffffffff));
}

AttackableObject::AttackableObject(int oid, int tid, const char* name,
        float x, float y, float z)
        : LocatableObject(oid, tid, name, x, y, z)
{
    DEF_PROPERTY("maxHP", 1);
    DEF_PROPERTY("HP", 1.0f);
    DEF_PROPERTY("baseAtk", 0.0f);
    DEF_PROPERTY("baseArmor", 0.0f);
    DEF_PROPERTY("bonusUnitAtk", 1.0f);
    DEF_PROPERTY("bonusBasicBuildingAtk", 1.0e-1f);
    DEF_PROPERTY("bonusMediumBuildingAtk", 1.0e-2f);
    DEF_PROPERTY("bonusAdvancedBuildingAtk", 1.0e-3f);
    DEF_PROPERTY("experience", 0);
    DEF_PROPERTY("city", nullptr);
    DEF_PROPERTY("attack_status", AST_ALIVE);

}
AttackableObject::AttackableObject(int oid, int tid, const char* name,
        float x, float y, float z, int maxHP, float HP,
        float baseAtk, float baseArmor)
        : LocatableObject(oid, tid, name, x, y, z)
{
    DEF_PROPERTY("maxHP", maxHP);
    DEF_PROPERTY("HP", HP);
    DEF_PROPERTY("baseAtk", baseAtk);
    DEF_PROPERTY("baseArmor", baseArmor);
    DEF_PROPERTY("bonusUnitAtk", 1.0f);
    DEF_PROPERTY("bonusBasicBuildingAtk", 1.0e-1f);
    DEF_PROPERTY("bonusMediumBuildingAtk", 1.0e-2f);
    DEF_PROPERTY("bonusAdvancedBuildingAtk", 1.0e-3f);
    DEF_PROPERTY("experience", 0);
    DEF_PROPERTY("city", nullptr);
    DEF_PROPERTY("attack_status", AST_ALIVE);
}

AttackableObject::AttackableObject(int oid, int tid, const char* name,
	float x, float y, float z, int maxHP,
        float baseAtk, float baseArmor)
        : LocatableObject(oid, tid, name, x, y, z)
{
    DEF_PROPERTY("maxHP", maxHP);
    DEF_PROPERTY("HP", (float)maxHP);
    DEF_PROPERTY("baseAtk", baseAtk);
    DEF_PROPERTY("baseArmor", baseArmor);
    DEF_PROPERTY("bonusUnitAtk", 1.0f);
    DEF_PROPERTY("bonusBasicBuildingAtk", 1.0e-1f);
    DEF_PROPERTY("bonusMediumBuildingAtk", 1.0e-2f);
    DEF_PROPERTY("bonusAdvancedBuildingAtk", 1.0e-3f);
    DEF_PROPERTY("experience", 0);
    DEF_PROPERTY("city", nullptr);
    DEF_PROPERTY("attack_status", AST_ALIVE);
}

int AttackableObject::GetMaxHP(void){
    return GET_PROPERTY(int,"maxHP");
}
float AttackableObject::GetHP(void){
    return GET_PROPERTY(float,"HP");
}
float AttackableObject::GetBaseAttack(void){
    return GET_PROPERTY(float,"baseAtk");
}
float AttackableObject::GetBaseArmor(void){
    return GET_PROPERTY(float,"baseArmor");
}

float AttackableObject::SetBaseAttack(float f) {
	float bAtk = GET_PROPERTY(float,"baseAtk");
	bAtk += f;
	SET_PROPERTY("baseAtk", bAtk);
	return bAtk;
}

float AttackableObject::SetBaseArmor(float f) {
	float bArmor = GET_PROPERTY(float,"baseArmor");
	bArmor += f;
	SET_PROPERTY("baseArmor", bArmor);
	return bArmor;
}

/* Increase HP until maximum */
float AttackableObject::Heal(float val)
{
    float f = this->GetHP();
    float max = this->GetMaxHP();

    f = std::min(f + val, max);

    SET_PROPERTY("HP", f);
	return f;

}

/* Decrease HP until minimum */
float AttackableObject::Damage(float val)
{
    float f = GET_PROPERTY(float, "HP");

    f =  std::max(f + val, 0.0f);

    if (f <= 0.0) {
	SET_PROPERTY("attack_status", AST_DEAD);
    }

    
    SET_PROPERTY("HP", f);
	return f;
}

float AttackableObject::GetUnitBonus(void) {
    return GET_PROPERTY(float,"bonusUnitAtk");
}
float AttackableObject::GetBasicBuildingBonus(void) {
	return GET_PROPERTY(float,"bonusBasicBuildingAtk");
}
float AttackableObject::GetMediumBuildingBonus(void) {
	return GET_PROPERTY(float,"bonusMediumBuildingAtk");
}
float AttackableObject::GetAdvancedBuildingBonus(void) {
	return GET_PROPERTY(float,"bonusAdvancedBuildingAtk");
}

int AttackableObject::GetExperience()
{
    return GET_PROPERTY(int, "experience");
}

	    
AttackableStatus AttackableObject::GetStatus(void)
{
    return GET_PROPERTY(AttackableStatus, "attack_status");
}

/* Set the object status. 
 * Note that you can't set an object to dead */
void AttackableObject::SetStatus(AttackableStatus a){
    if (a != AST_DEAD)
	SET_PROPERTY("attack_status", a);
}
	    
float AttackableObject::Hit(AttackableObject* other) 
{
    /* Check if this object is invulnerable.
       We can't damage nor be damaged by invulnerable attackers */
    if (this->GetStatus() == AST_INVULNERABLE ||
	other->GetStatus() == AST_INVULNERABLE) {
	return 0.0f;
    }
    
    attack_seed = (rand() % 90) + 10;
    double atk_percent = attack_seed * 0.01;
    double atk_true = (GetBaseAttack());  //do not use bonuses yet
    int exp = this->GetExperience();
    
    double atk_real = atk_true * atk_percent * (1+(exp*0.001));
    other->Damage(atk_real);

    /* Check if the other is dead.
       You win 1 experience point for each entity killed */
    if (other->GetStatus() == AST_DEAD) {
	SET_PROPERTY("experience", ++exp);
    }

    return (float)atk_real;
}
