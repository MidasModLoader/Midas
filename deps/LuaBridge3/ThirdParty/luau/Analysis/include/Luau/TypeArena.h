// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#pragma once

#include "Luau/TypedAllocator.h"
#include "Luau/TypeVar.h"
#include "Luau/TypePack.h"

#include <vector>

namespace Luau
{

struct TypeArena
{
    TypedAllocator<TypeVar> typeVars;
    TypedAllocator<TypePackVar> typePacks;

    void clear();

    template<typename T>
    TypeId addType(T tv)
    {
        if constexpr (std::is_same_v<T, UnionTypeVar>)
            LUAU_ASSERT(tv.options.size() >= 2);

        return addTV(TypeVar(std::move(tv)));
    }

    TypeId addTV(TypeVar&& tv);

    TypeId freshType(TypeLevel level);
    TypeId freshType(Scope* scope);
    TypeId freshType(Scope* scope, TypeLevel level);

    TypePackId freshTypePack(Scope* scope);

    TypePackId addTypePack(std::initializer_list<TypeId> types);
    TypePackId addTypePack(std::vector<TypeId> types, std::optional<TypePackId> tail = {});
    TypePackId addTypePack(TypePack pack);
    TypePackId addTypePack(TypePackVar pack);

    template<typename T>
    TypePackId addTypePack(T tp)
    {
        return addTypePack(TypePackVar(std::move(tp)));
    }
};

void freeze(TypeArena& arena);
void unfreeze(TypeArena& arena);

} // namespace Luau
