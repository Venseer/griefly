#include "Gun.h"
#include "core/objects/Tile.h"

using namespace kv;

Gun::Gun()
{
    SetSprite("icons/guns.dmi");
    ammunition_ = 1;
    max_ammunition_ = 1;
}

bool Gun::UseAmmo()
{
    if (ammunition_ <= 0)
    {
        ammunition_ = 0;
        return false;
    }
    --ammunition_;
    return true;
}

void Gun::ShootImpl(Vector target, const QString& sound,
                    const QString& projectile_type, const QString& casing_type)
{
    IdPtr<CubeTile> tile = GetOwner()->GetOwner();
    IdPtr<Movable> shooter = GetOwner();
    if (tile.IsValid())
    {	
        if (UseAmmo())
        {
            int x = target.x;
            int y = target.y;
            Dir shooting_direction;
            Dir facing = shooter->GetDir();
            if(qAbs(y) != qAbs(x))
            {
                shooting_direction = VDirToDir(target);
            }
            else
            {
                if (y < 0)
                {
                    if (facing == Dir::WEST || facing == Dir::EAST)
                    {
                        if (x > 0)
                        {
                            shooting_direction = Dir::EAST;
                        }
                        else
                        {
                            shooting_direction = Dir::WEST;
                        }
                    }
                    else
                    {
                        shooting_direction = Dir::NORTH;
                    }
                }
                else
                {
                    if (facing == Dir::WEST || facing == Dir::EAST)
                    {
                        if (x > 0)
                        {
                            shooting_direction = Dir::EAST;
                        }
                        else
                        {
                            shooting_direction = Dir::WEST;
                        }
                    }
                    else
                    {
                        shooting_direction = Dir::SOUTH;
                    }
                }
            }
            shooter->Rotate(shooting_direction);
            auto projectile = Create<Projectile>(projectile_type, tile);
            projectile->MakeMovementPattern(target, facing);
            PlaySoundIfVisible(sound);
            projectile->Process();
            if (!casing_type.isEmpty())
            {
                Dir dir = static_cast<Dir>(GenerateRandom() % 4);
                IdPtr<Item> casing = Create<Item>(casing_type, tile.Id());
                casing->Rotate(dir);
            } 	
        }
        else
        {
            PlaySoundIfVisible("empty.wav");
            // the *click* text from ss13
        }
    }	
}

void Gun::Shoot(Vector target)
{
}

bool Gun::AddAmmo()
{
    if (ammunition_ < max_ammunition_)
    {
        ++ammunition_;
        return true;
    }
    return false;
}
void Gun::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<AmmunitionBox> box = item)
    {
        if (box->CheckBullets())
        {
            if (AddAmmo())
            {
                box->RemoveBullet();
                return;
            }
        }
        // message which tells the player that the box is empty 
    }
}

bool Gun::Targetable(IdPtr<MapObject> item)
{
    if (!item.IsValid())
    {
        return false;
    }

    if (!item->GetOwner())
    {
        return false;
    }

    IdPtr<CubeTile> cube_tile = item->GetOwner();
    if (!cube_tile.IsValid())
    {
        return false;
    }

    return true;
}

Vector Gun::TargetTileLoc(IdPtr<MapObject> item) const
{
    IdPtr<CubeTile> cube_tile = item->GetOwner();
    Vector f;
    // TODO: `Vector operator-(Position, Position)`
    const Position cube_position = cube_tile->GetPosition();
    const Position owner_position = GetOwner()->GetPosition();
    f.x = cube_position.x - owner_position.x;
    f.y = cube_position.y - owner_position.y;
    f.z = 0;
    return f;
}
