#include "Structure.h"

#include "objects/movable/items/Item.h"

using namespace kv;

void Structure::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Screwdriver> scr = item)
    {
        SetAnchored(!IsAnchored());
        PlaySoundIfVisible("Screwdriver.wav");
    }
}
