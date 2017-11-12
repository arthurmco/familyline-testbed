#include "TeamCoordinator.hpp"

using namespace Tribalia::Logic;

TeamCoordinator::TeamCoordinator()
{
    srand((uintptr_t)this);
}

void TeamCoordinator::AddTeam(Team* t)
{
    if (t->tinfo != nullptr) {
	TeamCities* tc = (TeamCities*)t->tinfo;
	tc->team = t;
	this->teamlist.push_back(tc);
    } else {
	TeamCities* te = new TeamCities(t);
	this->teamlist.push_back(te);
    }
}

Team* TeamCoordinator::CreateTeam(const char* name)
{
    Team* t = new Team;
    t->name = name;
    t->id = (decltype(t->id)) (uintptr_t)t;
    return t;
}

bool TeamCoordinator::AddCity(Team* t, City* c)
{
    bool isHere = false;
    for (auto& tt : teamlist) {
	if (tt->team->id == t->id) {
	    isHere = true;
	    tt->cities.push_back(c);
	    t->tinfo = tt;
	    c->SetTeam(t);
	    t->cref++;
	    break;
	}
    }

    return isHere;    
}

/* Removes a city from a team.
   Note that, as a city *needs* to have a team, for correct friend-foe recognizement,
   we're going to create a new team just for it */
void TeamCoordinator::RemoveCity(City* c)
{
    Team* oldt = c->GetTeam();
    TeamCities* oldtc = (TeamCities*)oldt->tinfo;
    printf("oldid %d", oldt->id);

    for (auto ct = oldtc->cities.begin(); ct != oldtc->cities.end(); ++ct) {
	if (!strcmp((*ct)->GetName(), c->GetName())) {
	    oldtc->cities.erase(ct);
	    break;
	}
    }
    
    int oldtref = oldt->cref--;

    auto t = this->CreateTeam(c->GetName());
    printf("| newid %d\n", t->id);

    TeamCities* newtc = new TeamCities(*oldtc);
    newtc->cities.clear();
    t->tinfo = newtc;
    
    this->AddTeam(t);
    this->AddCity(t, c);

    if (oldtref < 0) {
	delete oldt;
    }
}

TeamDiplomacy TeamCoordinator::GetDiplomacyFor(Team* __restrict t1, Team* __restrict t2)
{
    if (t1 == t2) return DIPLOMACY_FRIEND;
    
    TeamCities* tt1 = (TeamCities*)(t1->tinfo);

    if (std::find(tt1->friends.begin(), tt1->friends.end(), t2) != tt1->friends.end()) {
	// THey are friends
	return DIPLOMACY_FRIEND;
    }

    if (std::find(tt1->foes.begin(), tt1->foes.end(), t2) != tt1->foes.end()) {
	return DIPLOMACY_FOE;
    }

    return DIPLOMACY_NEUTRAL;
}

/* Sets the diplomacy of t2 to 'dip' in t1.
   This doesn't set the other team diplomacy, this is a thing the other team 
   has to do */
void TeamCoordinator::SetDiplomacyFor(Team* t1, TeamDiplomacy dip, Team* t2)
{
    /* Remove any current set diplomacy */
    TeamDiplomacy old_dip = this->GetDiplomacyFor(t1, t2);
    TeamCities* tt1 = (TeamCities*)(t1->tinfo);

    if (old_dip == dip) {
	/* Same thing. Nothing to do */
	return;
    }
    
    switch (old_dip) {
    case DIPLOMACY_FRIEND:
	std::remove(tt1->friends.begin(), tt1->friends.end(), t2);
	break;

    case DIPLOMACY_FOE:
	std::remove(tt1->foes.begin(), tt1->foes.end(), t2);
	break;
	
    default:
	break;
    }


    switch (dip) {
    case DIPLOMACY_FRIEND:
	tt1->friends.push_back(t2);
	break;

    case DIPLOMACY_FOE:
	tt1->foes.push_back(t2);
	break;


    default:
	break;
    }
    
}
