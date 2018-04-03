#include "AmmunitionBoxes.h"

using namespace kv;

AmmunitionBox::AmmunitionBox()
{
    SetSprite("icons/ammo.dmi");
    SetState("357");

    SetName("Revolver Ammunition"),

    bullets_ = 50;
}

bool AmmunitionBox::CheckBullets()
{
    if (bullets_ > 0)
    {
        return true;
    }
    return false;
}

void AmmunitionBox::RemoveBullet()
{
    --bullets_;
}
