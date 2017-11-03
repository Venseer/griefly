#include "Ghost.h"

#include "core/Map.h"

#include "net/MagicStrings.h"
#include "core/ObjectFactory.h"
#include "core/objects/Lobby.h"
#include "LoginMob.h"
#include "core/Game.h"

#include "representation/Representation.h"

#include "Human.h"

using namespace kv;

Ghost::Ghost()
{
    tick_speed_ = 1;

    v_level = 11;

    SetSprite("icons/mob.dmi");
    SetState("ghost_grey");

    passable_level = passable::EMPTY;

    name = "Ghost";

    seconds_until_respawn_ = 15;
}

void Ghost::AfterWorldCreation()
{
    Mob::AfterWorldCreation();
    SetFreq(10);
}

bool Ghost::IsMobGhost()
{
    static quint32 mob_id = 0;
    static bool draw = true;
    if (!GetGame().GetMob())
    {
        return false;
    }
    if (mob_id != GetGame().GetMob().Id())
    {
        if (IdPtr<Ghost> g = GetGame().GetMob())
        {
            draw = true;
        }
        else
        {
            draw = false;
        }
        mob_id = GetGame().GetMob().Id();
    }
    return draw;
}

void Ghost::Represent(GrowingFrame* frame)
{
    if (IsMobGhost())
    {
        Mob::Represent(frame);
    }
}

void Ghost::CalculateVisible(VisiblePoints* visible_list) const
{
    visible_list->clear();
    Position p;
    p.z = GetPosition().z;

    // TODO: proper viewport
    const int SIZE_H_SQ = 12; // visible size const
    const int SIZE_W_SQ = 12;

    int x_low_border = qMax(0, GetPosition().x - SIZE_H_SQ);
    int x_high_border = qMin(GetGame().GetMap().GetWidth(), GetPosition().x + SIZE_H_SQ);
    int y_low_border = qMax(0, GetPosition().y - SIZE_W_SQ);
    int y_high_border = qMin(GetGame().GetMap().GetHeight(), GetPosition().y + SIZE_W_SQ);
    for (int i = x_low_border; i < x_high_border; ++i)
    {
        for (int j = y_low_border; j < y_high_border; ++j)
        {
            p.x = i;
            p.y = j;
            visible_list->push_back(p);
        }
    }
}

void Ghost::ProcessMessage(const Message& message)
{
    Mob::ProcessMessage(message);
}

void Ghost::GenerateInterfaceForFrame(GrowingFrame* frame)
{
    const QString text = QString("Until respawn: %1").arg(seconds_until_respawn_);
    frame->Append(FrameData::TextEntry{"Main", text});
}

void Ghost::MindEnter()
{
    // Nothing
}

void Ghost::MindExit()
{
    // Nothing
}

void Ghost::Process()
{
    --seconds_until_respawn_;
    if (seconds_until_respawn_ < 0)
    {
        if (IsMinded())
        {
            auto login_mob = Create<Mob>(LoginMob::GetTypeStatic(), 0);
            MoveMindTo(login_mob);
            Delete();
        }
    }
}
