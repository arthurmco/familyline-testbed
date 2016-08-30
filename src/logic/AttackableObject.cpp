
#include "AttackableObject.hpp"

using namespace Tribalia::Logic;

AttackableObject::AttackableObject(int oid, int tid, const char* name)
    : LocatableObject(oid, tid, name)
{
    DEF_PROPERTY("maxHP", 0);
    DEF_PROPERTY("HP", 0.0f);
    DEF_PROPERTY("baseAtk", 0.0f);
    DEF_PROPERTY("baseArmor", 0.0f);
    DEF_PROPERTY("bonusUnitAtk", 1.0f);
    DEF_PROPERTY("bonusBasicBuildingAtk", 1.0e-1f);
    DEF_PROPERTY("bonusMediumBuildingAtk", 1.0e-2f);
    DEF_PROPERTY("bonusAdvancedBuildingAtk", 1.0e-3f);
    DEF_PROPERTY("experience", 0);

}
AttackableObject::AttackableObject(int oid, int tid, const char* name,
        float x, float y, float z)
        : LocatableObject(oid, tid, name, x, y, z)
{
    DEF_PROPERTY("maxHP", 0);
    DEF_PROPERTY("HP", 0.0f);
    DEF_PROPERTY("baseAtk", 0.0f);
    DEF_PROPERTY("baseArmor", 0.0f);
    DEF_PROPERTY("bonusUnitAtk", 1.0f);
    DEF_PROPERTY("bonusBasicBuildingAtk", 1.0e-1f);
    DEF_PROPERTY("bonusMediumBuildingAtk", 1.0e-2f);
    DEF_PROPERTY("bonusAdvancedBuildingAtk", 1.0e-3f);
    DEF_PROPERTY("experience", 0);

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
    float f = GET_PROPERTY(float,"HP");

    f =  std::max(f + val, 0.0f);

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

int attack_chance = 0;
float AttackableObject::Hit(AttackableObject* other) 
{
    
    /*  Check death of both
        Give a experience bonus to the killer */
    if (this->GetHP() <= 0) {
        other->SetProperty("experience", 
            other->GetProperty<int>("experience") + 10);
        attack_chance = 0;
    } else if (other->GetHP() <= 0) {
        SET_PROPERTY("experience", GetExperience() + 10);
    }

    /* Calculate hit chance */
    float atk = GetBaseAttack();
    float def = other->GetBaseArmor();
 
    if (attack_chance == 0) {
        srand((int)(atk * def));
    }

    attack_chance = rand();
    float atk_bonus = ((attack_chance / atk) * GetExperience()) /
         ((attack_chance / def) * other->GetExperience());

    float ret = (atk * atk_bonus) - def;

    if (ret >= 1.0f) {
        SET_PROPERTY("experience", GetExperience() + 1);
        return ret;
    } else {
        other->SetProperty("experience", 
            other->GetProperty<int>("experience") + 1);
        return 0.0f;
    }
}

/*  Get all actions. Return the action count.
    'names' is an array of C strings */
int AttackableObject::GetActionNames(const char**& names)
{
    int count = 0;
    for (auto it = _actions.begin(); it != _actions.end(); it++) {
        if (names) {
            if (names[count]) {
                names[count] = it->name;
            }
        }
        count++;
    }

    return count;
}

/*  Get the data for each action */
UserAction* AttackableObject::GetAction(const char* name)
{
    for (auto it = _actions.begin(); it != _actions.end(); it++) {
        if (!strcmp(it->name, name)) {
            return (UserAction*)&it;
        }
    }

    return nullptr;
}
