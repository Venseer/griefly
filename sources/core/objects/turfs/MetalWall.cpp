#include "MetalWall.h"

#include "objects/movable/items/Weldingtool.h"
#include "objects/movable/structures/Girder.h"
#include "Floor.h"
#include "objects/movable/items/Materials.h"

#include "ObjectFactory.h"

using namespace kv;

MetalWall::MetalWall()
{
    transparent = false;
    SetPassable(Dir::ALL, passable::EMPTY);

    v_level = 2;

    SetSprite("icons/walls.dmi");
    SetState("metal0");

    name = "Metal wall";
}

void MetalWall::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Weldingtool> wtool = item)
    {
        if (wtool->Working())
        {
            PlaySoundIfVisible("Welder.wav");
            Create<MaterialObject>(Girder::GetTypeStatic(), GetOwner());
            Create<MaterialObject>(Metal::GetTypeStatic(), GetOwner());
            Create<Turf>(Plating::GetTypeStatic(), GetOwner());
            Delete();
        }
    }
}

ReinforcedWall::ReinforcedWall()
{
    transparent = false;
    SetPassable(Dir::ALL, passable::EMPTY);

    v_level = 2;

    SetSprite("icons/walls.dmi");
    SetState("r_wall");

    name = "Reinforced wall";
}
