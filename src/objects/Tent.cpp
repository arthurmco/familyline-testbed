#include "Tent.hpp"

using namespace familyline;
using namespace familyline::logic;

//static AttackAttributes atk(0, 1, glm::radians(0.0), 0, 0);

Tent::Tent() : GameObject("tent", "Tent", glm::vec2(15, 15), 250, 250)
{
    DEF_MESH("Tent.obj");
};

/* Called on object initialization */
bool Tent::Initialize() { return true; }

/* Called on each engine loop, when an action is performed */
bool Tent::DoAction(void) {return true; }

CLONE_MACRO_CPP(Tent)

//ADD_COPY_CTOR(Tent)
