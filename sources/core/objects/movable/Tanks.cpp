#include "Tanks.h"

#include "items/Weldingtool.h"

using namespace kv;

FuelTank::FuelTank()
{
    SetName("Fueltank");

    SetVisibleLevel(visible_level::ITEM + 1);

    SetTransparency(true);

    SetSprite("icons/objects.dmi");
    SetState("weldtank");

    SetPassable(Dir::ALL, passable::AIR);
}

void FuelTank::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Weldingtool> w = item)
    {
        w->AddFuel(250);
        PlaySoundIfVisible("refill.wav");
    }
}

WaterTank::WaterTank()
{
    SetName("Watertank");

    SetVisibleLevel(visible_level::ITEM + 1);

    SetTransparency(true);

    SetSprite("icons/objects.dmi");
    SetState("watertank");

    SetPassable(Dir::ALL, passable::AIR);
}

void WaterTank::AttackBy(IdPtr<Item> /*item*/)
{

}

