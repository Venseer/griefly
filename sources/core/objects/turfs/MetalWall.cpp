#include "MetalWall.h"

#include "objects/movable/items/Weldingtool.h"
#include "objects/movable/structures/Girder.h"
#include "Floor.h"
#include "objects/movable/items/Materials.h"

#include "ObjectFactory.h"

using namespace kv;

MetalWall::MetalWall()
{
    SetTransparency(false);
    SetPassable(Dir::ALL, passable::EMPTY);

    SetVisibleLevel(visible_level::HIGH_TURF);

    SetSprite("icons/walls.dmi");
    SetState("metal0");

    SetName("Metal wall");
}

void MetalWall::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Weldingtool> wtool = item)
    {
        if (wtool->IsWorking())
        {
            PlaySoundIfVisible("Welder.wav");
            Create<Girder>(GetOwner());
            Create<Metal>(GetOwner());
            Create<Plating>(GetOwner());
            Delete();
        }
    }
}

ReinforcedWall::ReinforcedWall()
{
    SetTransparency(false);
    SetPassable(Dir::ALL, passable::EMPTY);

    SetVisibleLevel(visible_level::HIGH_TURF);

    SetSprite("icons/walls.dmi");
    SetState("r_wall");

    SetName("Reinforced wall");
}
