#include <cassert>

#include "MaterialObject.h"
#include "Map.h"
#include "mobs/Mob.h"
#include "Tile.h"

using namespace kv;

void MaterialObject::SetSprite(const QString& sprite)
{
    view_.SetSprite(sprite);
}

void MaterialObject::SetState(const QString& name)
{
    view_.SetState(name);
}

void MaterialObject::Represent(GrowingFrame* frame, IdPtr<Mob> mob)
{ 
    FrameData::Entity ent;
    ent.id = GetId();
    ent.click_id = GetId();
    ent.pos_x = GetPosition().x;
    ent.pos_y = GetPosition().y;
    ent.vlevel = v_level;
    ent.view = GetView()->GetRawData();
    ent.dir = Dir::SOUTH;
    frame->Append(ent);
}

void MaterialObject::Delete()
{
    MapObject::Delete();
}

MaterialObject::MaterialObject()
{
    v_level = 0;
    passable_all = passable::FULL;
    passable_up = passable::FULL;
    passable_down = passable::FULL;
    passable_left = passable::FULL;
    passable_right = passable::FULL;

    passable_level = passable::FULL;

    SetTransparency(true);
    SetName("nameless");
}
