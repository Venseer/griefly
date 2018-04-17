#include "ProjectileTypes.h"

using namespace kv;

Bullet::Bullet()
{
    SetDamage(20);
    SetState("bullet");
    SetName("Bullet");
}

Laser::Laser()
{
    SetBurnDamage(10);
    SetState("laser");
    SetName("Laser");
}

bool Laser::CheckPassable()
{
    if (!harmless_)
    {
        if (CheckHumanTile())
        {
            return false;
        }
    }
    auto tile = GetOwner();
    if (tile->Istransparent())
    {
        auto nearby = tile->GetNeighbour(GetDir());
        // Check for map borders
        if ((nearby != tile) && nearby->Istransparent())
        {
            return true;
        }
    }

    // TODO (?): Bump() by Laser.
    // We cannot call IMovable::CheckPassable here because then
    // laser projectiles will bump glasses, so it is needed to add bump here

    Delete();
    return false;
}
