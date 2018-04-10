#pragma once

#include <functional>

#include "Object.h"
#include "atmos/AtmosHolder.h"

namespace kv
{
class Turf;
class Movable;
class Item;

using PassableLevel = int;
namespace passable
{
    const PassableLevel EMPTY = 0;
    const PassableLevel AIR = 10;
    const PassableLevel SMALL_ITEM = 20;
    const PassableLevel BIG_ITEM = 30;
    const PassableLevel SMALL_CREATURE = 40;
    const PassableLevel BIG_CREATURE = 50;
    const PassableLevel FULL = 100;
}

inline bool CanPass(PassableLevel block, PassableLevel object)
{
    return block >= object;
}

class MapObject : public Object
{
public:
    DECLARE_SAVEABLE(MapObject, Object);
    REGISTER_CLASS_AS(MapObject);
    MapObject() { owner_ = 0; }
    virtual void ForEach(std::function<void(IdPtr<MapObject>)> callback)
    {
        Q_UNUSED(callback)
        return;
    }
    // 0 - passable
    // 10 - air passable
    //
    //
    virtual PassableLevel GetPassable(Dir direct) const
    {
        Q_UNUSED(direct)
        return passable::EMPTY;
    }
    virtual bool Istransparent() const
    {
        return true;
    }
    
    virtual void Delete() override
    {
        if (owner_.IsValid())
        {
            owner_->RemoveObject(GetId());
        }
        Object::Delete();
    }

    virtual void AttackBy(IdPtr<Item> item);

    virtual void ApplyForce(Vector force) { Q_UNUSED(force) }

    virtual bool CanTouch(IdPtr<MapObject> object) const
    {
        if (owner_.IsValid())
        {
            return owner_->CanTouch(object);
        }
        return false;
    }

    // Add some item
    // True - added
    // False - failed
    // This must not remove item from old place
    virtual bool AddObject(IdPtr<MapObject> object)
    {
        if (owner_.IsValid())
        {
            return owner_->AddObject(object);
        }
        return false;
    }
    // Remove some item
    // True - removed
    // False failed
    virtual bool RemoveObject(IdPtr<MapObject> object)
    {
        Q_UNUSED(object)
        return false;
    }
    // If id equal with object id, dir fail or something else (operation unsupported)
    virtual IdPtr<MapObject> GetNeighbour(Dir direct) const
    {
        if (owner_.IsValid())
        {
            return owner_->GetNeighbour(direct);
        }
        return GetId();
    }

    virtual bool Contains(IdPtr<MapObject> object) const
    {
        Q_UNUSED(object);
        return false;
    }

    virtual void Bump(const Vector& force, IdPtr<Movable> item);
    virtual void BumpByGas(const Vector& force, bool inside = false);

    virtual void ApplyFire(int intensity) { Q_UNUSED(intensity); }

    virtual atmos::AtmosHolder* GetAtmosHolder() { return nullptr; }

    template<class T>
    IdPtr<T> GetItem()
    {
        return GetItemImpl(T::GetTypeIndexStatic());
    }
    void SetOwner(IdPtr<MapObject> owner)
    {
        owner_ = owner;
    }
    IdPtr<MapObject> GetOwner() const
    {
        return owner_;
    }

    virtual Position GetPosition() const
    {
        return owner_->GetPosition();
    }
    virtual void Represent(GrowingFrame* frame, IdPtr<kv::Mob> mob) const
    {
        Q_UNUSED(frame)
        Q_UNUSED(mob)
    }
    virtual void UpdatePassable() { }
    virtual IdPtr<Turf> GetTurf() const;
    virtual void SetTurf(IdPtr<Turf> turf);
    IdPtr<MapObject> GetRoot();
    void PlaySoundIfVisible(const QString& name);
protected:
    virtual quint32 GetItemImpl(int type_index)
    {
        Q_UNUSED(type_index)
        return 0;
    }
private:
    IdPtr<MapObject> KV_SAVEABLE(owner_);
};
END_DECLARE(MapObject);

}
