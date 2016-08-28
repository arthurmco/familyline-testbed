/*  Action header and controller
    Controls actions performed by the user on the entities

*/

#include "LocatableObject.hpp"

#ifndef ACTION_HPP
#define ACTION_HPP

namespace Tribalia {
namespace Logic {

  /* Data binded to the action */
  struct ActionData {
      float xpos, ypos; /* X and Y cursor position */
      LocatableObject* o; /* The object that caused the action */
  };

  /* Callback used for when an action was triggered */
  typedef int (*ActionCallback)(ActionData* data);

  struct UserAction {
      /*  Short name that identifies the action
          It isn't the name that appears on the interface, but must be unique
          for each entity and their childs */
      char name[32];

      /* The callback */
      ActionCallback callback;

      /* Can the entity be executed right now? */
      bool enabled;
  };

}
}



#endif /* end of include guard: ACTION_HPP */
