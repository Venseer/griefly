#include "Object.h"
#include "../Map.h"
#include "../ObjectFactory.h"

#include "objects/GlobalObjectsHolder.h"
#include "SynchronizedRandom.h"
#include "objects/mobs/Mob.h"
#include "ChatFrameInfo.h"

using namespace kv;

void Object::PlayMusic(const QString& name, int volume)
{
    quint32 net_id = GetGame().GetNetId(GetId());
    qDebug() << net_id;
    if (net_id != 0)
    {
        GetGame().PlayMusic(name, volume, net_id);
    }
}

void Object::Delete()
{
    GetGame().GetFactory().DeleteLater(id_);
}

void Object::Save(FastSerializer& serializer)
{
    serializer.WriteType(GetType());
    serializer << id_;
    serializer << how_often_;
}

void Object::Load(FastDeserializer& deserializer)
{
    // It is mainly empty because all data is loaded by
    // the outer function

    deserializer >> how_often_;

    SetFreq(GetFreq());
}

void Object::SetFreq(int freq)
{
    if (game_ == nullptr)
    {
        kv::Abort("SetFreq is called in constructor");
    }

    how_often_ = freq;

    // TODO: Why is it here?
    if (!GetId())
    {
        kv::Abort("GetId() is zero");
    }

    if (how_often_ != 0)
    {
        GetGame().GetFactory().AddProcessingItem(GetId());
    }
}

const GameInterface& Object::GetGame() const
{
    if (game_ == nullptr)
    {
        kv::Abort("Object::GetGame() is called during construction of object");
    }
    return *game_;
}

ObjectFactoryInterface& Object::GetFactory()
{
    return GetGame().GetFactory();
}

GameInterface& Object::GetGame()
{
    if (game_ == nullptr)
    {
        kv::Abort("Object::GetGame() is called during construction of object");
    }
    return *game_;
}

quint32 Object::GenerateRandom()
{
    return GetGame().GetGlobals()->random->Generate();
}

void Object::PostHtmlFor(const QString& html, IdPtr<Object> uncasted_mob)
{
    if (IdPtr<Mob> mob = uncasted_mob)
    {
        if (mob->IsMinded())
        {
            GetGame().GetChatFrameInfo().PostPersonal(html, GetGame().GetNetId(mob.Id()));
        }
    }
}

void Object::PostVisible(const QString& html, const Position& place)
{
    GetGame().GetChatFrameInfo().PostVisible(html, place);
}

qint32 Object::GetGameTick() const
{
    return GetGame().GetGlobals()->game_tick;
}

quint32 Object::CreateImpl(const QString& type, quint32 owner)
{
    return GetFactory().CreateImpl(type, owner);
}
