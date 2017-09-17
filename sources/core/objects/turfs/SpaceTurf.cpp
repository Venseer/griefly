#include "SpaceTurf.h"

#include "core/SynchronizedRandom.h"

#include "core/objects/movable/items/Item.h"
#include "core/objects/movable/items/Materials.h"
#include "core/objects/movable/structures/Lattice.h"
#include "core/ObjectFactory.h"
#include "core/Game.h"

using namespace kv;

Space::Space()
{
    SetAtmosState(atmos::SPACE);

    SetPassable(Dir::ALL, passable::FULL);
    transparent = true;
    
    SetFriction(0);

    SetSprite("icons/space.dmi");
    SetState("10");
    name = "Space";
}

void Space::AfterWorldCreation()
{
    quint32 value = GenerateRandom() % 25 + 1;
    SetState(QString::number(value));
}

void Space::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Rod> rod = item)
    {
        rod->Delete();
        Create<MaterialObject>(Lattice::GetTypeStatic(), GetOwner());
    }
}
