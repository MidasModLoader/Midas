// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
#pragma once

#include "Luau/Substitution.h"
#include "Luau/TypeVar.h"
#include "Luau/Unifiable.h"

namespace Luau
{

struct TypeArena;
struct TxnLog;

// A substitution which replaces generic types in a given set by free types.
struct ReplaceGenerics : Substitution
{
    ReplaceGenerics(const TxnLog* log, TypeArena* arena, TypeLevel level, Scope* scope, const std::vector<TypeId>& generics,
        const std::vector<TypePackId>& genericPacks)
        : Substitution(log, arena)
        , level(level)
        , scope(scope)
        , generics(generics)
        , genericPacks(genericPacks)
    {
    }

    TypeLevel level;
    Scope* scope;
    std::vector<TypeId> generics;
    std::vector<TypePackId> genericPacks;
    bool ignoreChildren(TypeId ty) override;
    bool isDirty(TypeId ty) override;
    bool isDirty(TypePackId tp) override;
    TypeId clean(TypeId ty) override;
    TypePackId clean(TypePackId tp) override;
};

// A substitution which replaces generic functions by monomorphic functions
struct Instantiation : Substitution
{
    Instantiation(const TxnLog* log, TypeArena* arena, TypeLevel level, Scope* scope)
        : Substitution(log, arena)
        , level(level)
        , scope(scope)
    {
    }

    TypeLevel level;
    Scope* scope;
    bool ignoreChildren(TypeId ty) override;
    bool isDirty(TypeId ty) override;
    bool isDirty(TypePackId tp) override;
    TypeId clean(TypeId ty) override;
    TypePackId clean(TypePackId tp) override;
};

} // namespace Luau
