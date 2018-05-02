#pragma once

#include "Breakable.h"

namespace kv
{

class FlatGlass : public Breakable
{
public:
    DECLARE_SAVEABLE(FlatGlass, Breakable);
    REGISTER_CLASS_AS(FlatGlass);
    FlatGlass();

    virtual void AfterWorldCreation() override;

    virtual void ApplyForce(const Vector& force, ForceSource source = ForceSource::UNKNOWN) override;

    virtual void Bump(const Vector& vector, IdPtr<Movable> item) override;
    virtual bool Rotate(Dir dir) override;
    virtual void Break() override;
    virtual void PlayOnHitSound() override;
    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(FlatGlass);

class ReinforcedFlatGlass : public FlatGlass
{
public:
    DECLARE_SAVEABLE(ReinforcedFlatGlass, FlatGlass);
    REGISTER_CLASS_AS(ReinforcedFlatGlass);
    ReinforcedFlatGlass();
};
END_DECLARE(ReinforcedFlatGlass);

}
