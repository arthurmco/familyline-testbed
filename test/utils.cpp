#include "utils.hpp"

using namespace familyline::logic;

TestObject::TestObject(const struct object_init& init)
    : GameObject(init.type, init.name, init.size, init.health,
                 init.maxHealth, init.showHealth),
      init_params_(init)
{
    this->fnUpdate_ = init.fnUpdate;

    if (init.atkComponent) {
        this->cAttack = init.atkComponent;
        this->cAttack.value().object = this;
    }
    
}

void TestObject::update()
{
    fnUpdate_();
}

std::shared_ptr<GameObject> TestObject::create()
{
    auto r = std::make_shared<TestObject>(init_params_);
    r->_id = -1;

    return r;
}

std::shared_ptr<TestObject> make_object(const struct object_init& init)
{
    return std::make_shared<TestObject>(init);
}
