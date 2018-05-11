#include "AttackableObject.hpp"
#include "TeamCoordinator.hpp"

using namespace Familyline::Logic;

int attack_seed = 0;

AttackableObject::AttackableObject(int oid, int tid, const char* name,
        float x, float y, float z, int maxHP, float HP,
        float baseAtk, float baseArmor)
        : LocatableObject(oid, tid, name, x, y, z)
{
    if (HP < 0)
	HP = float(maxHP);
    
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
    DEF_PROPERTY("attackStatus", AST_ALIVE);
    DEF_PROPERTY("attackRange", 1.0);

    /* This isn't reccomended, but we don't need a nice method
     * just to calculate the attack chance */
    srand((unsigned int)((uintptr_t)this & 0xffffffff));

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

    f =  std::max(f - val, 0.0f);

    if (f <= 0.0) {
	SET_PROPERTY("attackStatus", AST_DEAD);
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
    return GET_PROPERTY(AttackableStatus, "attackStatus");
}

/* Set the object status. 
 * Note that you can't set an object to dead */
void AttackableObject::SetStatus(AttackableStatus a){
    if (a != AST_DEAD)
	SET_PROPERTY("attackStatus", a);
}

/* Check if other object can be attacked.
   The other object can be, for example, an ally
   In this case, makes no sense to attack him */
bool AttackableObject::CheckIfAttackable(AttackableObject* other)
{ 
    City* thiscity = this->GetProperty<City*>("city");
    City* othercity = other->GetProperty<City*>("city");

    // No city. Assume neutral. Neutrals can be attacked
    if (!thiscity || !othercity) return true;

    Team* thist = thiscity->GetTeam();
    Team* othert = othercity->GetTeam();

    TeamCoordinator tc;
    return !(tc.GetDiplomacyFor(thist, othert) == DIPLOMACY_FRIEND);
    
}

float AttackableObject::Hit(AttackableObject* other, double tick) 
{
    /* Check if other object is attackable */
    if (!this->CheckIfAttackable(other)) {
	return 0;
    }
    
    /* Check if this object is invulnerable.
       We can't damage nor be damaged by invulnerable or dead attackers */
    if (this->GetStatus() == AST_INVULNERABLE ||
	other->GetStatus() == AST_INVULNERABLE ||
	other->GetStatus() == AST_DEAD ||
	this->GetStatus() == AST_DEAD) {
	return 0.0f;
    }
    
    attack_seed = (rand() % 85) + 15;
    double atk_percent = attack_seed * 0.01;
    double atk_true = (GetBaseAttack());  //do not use bonuses yet
    int exp = this->GetExperience();
    
    double atk_real = atk_true * atk_percent * (1+(exp*0.001));
    atk_real *= (tick * 10);
    other->Damage(atk_real);

    /* Check if the other is dead.
       You win 1 experience point for each entity killed */
    if (other->GetStatus() == AST_DEAD) {
	SET_PROPERTY("experience", ++exp);
    }

    return (float)atk_real;
}

/* Check if the other object is within attack range */
bool AttackableObject::CheckAttackRange(AttackableObject* other)
{
    /* Basic circle-circle collision detection */
    
    double atkRange = GET_PROPERTY(double, "attackRange");
    double atkRadius = atkRange + this->GetRadius();

    glm::vec2 vRelPos = glm::vec2(other->_xPos - this->_xPos,
				  other->_zPos - this->_zPos);

    return (glm::length(vRelPos) <= (atkRadius + other->GetRadius()));
}


