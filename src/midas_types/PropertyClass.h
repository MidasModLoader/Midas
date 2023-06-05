#ifndef MIDAS_PropertyClass_H
#define MIDAS_PropertyClass_H

#include "common.h"

class PropertyClass : public std::enable_shared_from_this<PropertyClass>
{
public:
     uint8_t padding[72];

     static void initialize_lua(lua_State *L)
     {
          luabridge::getGlobalNamespace(L)
              .beginClass<PropertyClass>("PropertyClass")
              .endClass();
     }
};

#endif // MIDAS_PropertyClass_H