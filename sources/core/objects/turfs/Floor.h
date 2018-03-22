#pragma once

#include "Turf.h"

namespace kv
{

class Floor : public Turf
{
public:
    DECLARE_SAVEABLE(Floor, Turf);
    REGISTER_CLASS_AS(Floor);
    Floor();
    virtual void AfterWorldCreation() override;

    virtual void AttackBy(IdPtr<Item> item) override;
    void SetOpen(bool o);

    virtual void ApplyFire(int intensity) override;

    bool KV_SAVEABLE(bloody);
protected:
    bool KV_SAVEABLE(open_);
    QString KV_SAVEABLE(floor_type_);
};
END_DECLARE(Floor);

class Plating : public Floor
{
public:
    DECLARE_SAVEABLE(Plating, Floor);
    REGISTER_CLASS_AS(Plating);
    Plating();
};
END_DECLARE(Plating);

}
