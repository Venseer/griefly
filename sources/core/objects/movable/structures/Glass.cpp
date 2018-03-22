#include "Glass.h"

#include "SynchronizedRandom.h"
#include "objects/mobs/Mob.h"
#include "Breakable.h"
#include "objects/movable/items/Shard.h"
#include "objects/PhysicsEngine.h"

using namespace kv;

FlatGlass::FlatGlass()
{
    transparent = true;

    tick_speed_ = 5;
    SetHitPoints(20);

    v_level = 9;

    SetSprite("icons/structures.dmi");
    SetState("window");
    
    name = "Window";
}

void FlatGlass::AfterWorldCreation()
{
    Structure::AfterWorldCreation();

    SetPassable(Dir::NORTH, passable::FULL);
    SetPassable(Dir::SOUTH, passable::FULL);
    SetPassable(Dir::WEST, passable::FULL);
    SetPassable(Dir::EAST, passable::FULL);

    SetPassable(GetDir(), passable::EMPTY);
}

void FlatGlass::ApplyForce(Vector force)
{
    const int max = std::max(std::max(force.x, force.y), force.z);
    const int BORDERLINE = 30 * FORCE_UNIT;
    if (max > BORDERLINE)
    {
        Break();
        return;
    }
    Breakable::ApplyForce(force);
}

void FlatGlass::Bump(const Vector& vector, IdPtr<Movable> item)
{
    if (IdPtr<Mob> mob = item)
    {
        if (item->GetDir() != GetDir())
        {
            if (!CanPass(GetOwner()->GetPassable(Dir::ALL), passable_level) || anchored_)
            {
                return;
            }

            Rotate(item->GetDir());
            return;
        }
        Movable::Bump(vector, item);
        return;
    }
    Breakable::Bump(vector, item);
}

bool FlatGlass::Rotate(Dir dir)
{
    SetPassable(GetDir(), passable::FULL);
    Structure::Rotate(dir);
    SetPassable(GetDir(), passable::EMPTY);
    return true;
}

void FlatGlass::Break()
{
    QString sound = QString("Glassbr%1.wav").arg(GenerateRandom() % 3 + 1);
    PlaySoundIfVisible(sound);
    Create<Item>(Shard::GetTypeStatic(), GetOwner());
    Delete();
}

void FlatGlass::PlayOnHitSound()
{
    PlaySoundIfVisible("Glasshit.wav");
}

void FlatGlass::AttackBy(IdPtr<Item> item)
{
    if (!item.IsValid())
    {
/*      GetGame().GetChat().PostSimpleText(
        name + " knocks on window. " +, GetRoot().Id());*/
        PlaySoundIfVisible("Glasshit.wav");
        return;
    }
    Breakable::AttackBy(item);
}

ReinforcedFlatGlass::ReinforcedFlatGlass()
{
    SetState("rwindow");
    name = "Reinforced window";
    SetHitPoints(100);
}
