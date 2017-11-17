#include "ObjectFactory.h"

#include "core/KvAbort.h"

#include "objects/Object.h"
#include "objects/MaterialObject.h"
#include "objects/PhysicsEngine.h"
#include "Game.h"
#include "Map.h"
#include "SynchronizedRandom.h"
#include "net/MagicStrings.h"
#include "AutogenMetadata.h"

#include "MapEditor.h"

ObjectFactory::ObjectFactory(GameInterface* game)
{
    objects_table_.resize(100);
    id_ = 1;
    is_world_generating_ = true;
    game_ = game;
    id_ptr_id_table = &objects_table_;
}

ObjectFactory::~ObjectFactory()
{
    ProcessDeletion();
    for (auto& info : objects_table_)
    {
        if (info.object)
        {
            delete info.object;
        }
    }
}

QVector<ObjectInfo>& ObjectFactory::GetIdTable()
{
    return objects_table_;
}

const QVector<ObjectInfo>& ObjectFactory::GetIdTable() const
{
    return objects_table_;
}

void ObjectFactory::UpdateProcessingItems()
{   
    if (!add_to_process_.size())
    {
        return;
    }
    for (auto it = add_to_process_.begin(); it != add_to_process_.end(); ++it)
    {
        if (!(*it).IsValid())
        {
            continue;
        }
        if ((*it)->GetFreq() == 0)
        {
            continue;
        }
        auto to_add = std::find(process_table_.begin(), process_table_.end(), *it);
        if (to_add == process_table_.end())
        {
            process_table_.push_back(*it);
        }
    }
    std::sort(process_table_.begin(), process_table_.end(),
    [](IdPtr<kv::Object> item1, IdPtr<kv::Object> item2)
    {
        return item1.Id() < item2.Id();
    });
    add_to_process_.clear();
}

void ObjectFactory::ForeachProcess()
{
    // TODO: possible unsync or hash miscalculation issue -
    // something can be added or removed from process list in the same tick after
    // ForeachProcess()
    UpdateProcessingItems();

    const int game_tick = game_->GetGlobals()->game_tick;

    const int CLEAR_TICK = 10;
    if (game_tick % CLEAR_TICK == 1)
    {
        ClearProcessing();
    }

    quint32 table_size = process_table_.size();
    for (quint32 i = 0; i < table_size; ++i)
    {
        if (!(   process_table_[i].IsValid()
              && process_table_[i]->GetFreq()))
        {
            continue;
        }
        else if ((game_tick % process_table_[i]->GetFreq()) == 0)
        {
            process_table_[i]->Process();
        }
    }

    ClearProcessing();
}

kv::Object* ObjectFactory::NewVoidObject(const QString& type)
{
    auto creator = GetItemsCreators()->find(type);
    if (creator == GetItemsCreators()->end())
    {
        kv::Abort(QString("Unable to find creator for type: %1").arg(type));
    }
    return creator->second();
}

kv::Object* ObjectFactory::NewVoidObjectSaved(const QString& type)
{
    auto creator = GetVoidItemsCreators()->find(type);
    if (creator == GetVoidItemsCreators()->end())
    {
        kv::Abort(QString("Unable to find void creator for type: %1").arg(type));
    }
    return creator->second();
}

void ObjectFactory::Clear()
{
    quint32 table_size = objects_table_.size();
    for (quint32 i = 1; i < table_size; ++i)
    {
        if (objects_table_[i].object != nullptr)
        {
            delete objects_table_[i].object;
        }
    }
    if (table_size != objects_table_.size())
    {
        qDebug() << "WARNING: table_size != idTable_.size()!";
    }

    ids_to_delete_.clear();
    process_table_.clear();
    add_to_process_.clear();
    add_to_process_.clear();

    id_ = 1;
}

void ObjectFactory::BeginWorldCreation()
{
    is_world_generating_ = true;
}

void ObjectFactory::FinishWorldCreation()
{
    MarkWorldAsCreated();
    quint32 table_size = objects_table_.size();
    for (quint32 i = 1; i < table_size; ++i)
    {
        if (objects_table_[i].object != nullptr)
        {
            objects_table_[i].object->AfterWorldCreation();
        }
    }
}

void ObjectFactory::MarkWorldAsCreated()
{
    is_world_generating_ = false;
}

quint32 ObjectFactory::CreateImpl(const QString &type, quint32 owner_id)
{
    kv::Object* item = NewVoidObject(type);
    kv::internal::GetObjectGame(item) = game_;

    if (id_ >= objects_table_.size())
    {
        objects_table_.resize(id_ * 2);
    }
    objects_table_[id_].object = item;

    kv::internal::GetObjectId(item) = id_;
    item->SetFreq(item->GetFreq());

    quint32 retval = id_;
    ++id_;
    IdPtr<kv::MapObject> owner = owner_id;
    if (owner.IsValid())
    {
        if (CastTo<kv::Turf>(item) != nullptr)
        {
            owner->SetTurf(item->GetId());
        }
        else if (!owner->AddObject(item->GetId()))
        {
            kv::Abort("AddItem failed");
        }
    }

    if (item->ToHearer())
    {
        game_->GetGlobals()->hearers.append(retval);
    }

    if (!is_world_generating_)
    {
        item->AfterWorldCreation();
    }
    return retval;
}

kv::Object* ObjectFactory::CreateVoid(const QString &hash, quint32 id_new)
{
    kv::Object* item = NewVoidObjectSaved(hash);
    kv::internal::GetObjectGame(item) = game_;
    if (id_new >= objects_table_.size())
    {
        objects_table_.resize(id_new * 2);
    }

    if (id_new >= id_)
    {
        id_ = id_new + 1;
    }
    objects_table_[id_new].object = item;
    kv::internal::GetObjectId(item) = id_new;
    item->SetFreq(item->GetFreq());
    return item;
}

void ObjectFactory::DeleteLater(quint32 id)
{
    ids_to_delete_.push_back(objects_table_[id].object);
    objects_table_[id].object = nullptr;
}

void ObjectFactory::ProcessDeletion()
{
    for (auto it = ids_to_delete_.begin(); it != ids_to_delete_.end(); ++it)
    {
        delete *it;
    }
    ids_to_delete_.clear();
}

quint32 ObjectFactory::Hash() const
{
    unsigned int h = 0;
    quint32 table_size = objects_table_.size();
    for (quint32 i = 1; i < table_size; ++i)
    {
        if (objects_table_[i].object != nullptr)
        {
            h += objects_table_[i].object->HashMembers();
        }
    }

    int i = 1;
    for (auto p = process_table_.begin(); p != process_table_.end(); ++p)
    {
        h += p->Id() * i;
        i++;
    }

    return h;
}

void ObjectFactory::AddProcessingItem(quint32 item)
{
    add_to_process_.push_back(item);
}

void ObjectFactory::ClearProcessing()
{
    std::vector<IdPtr<kv::Object>> remove_from_process;
    quint32 table_size = process_table_.size();
    for (quint32 i = 0; i < table_size; ++i)
    {
        if (!(   process_table_[i].IsValid()
              && process_table_[i]->GetFreq()))
        {
            remove_from_process.push_back(process_table_[i]);
        }
    }

    for (auto it = remove_from_process.begin(); it != remove_from_process.end(); ++it)
    {
        process_table_.erase(std::find(process_table_.begin(), process_table_.end(), *it));
    }
}
