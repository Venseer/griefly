#pragma once

#include "core_headers/CoreInterface.h"
#include "Interfaces.h"

#include "ChatFrameInfo.h"
#include "WorldLoaderSaver.h"

namespace kv
{

class WorldImplementation : public WorldInterface, public GameInterface
{
public:
    WorldImplementation();
    ~WorldImplementation();
    // World interface

    // TODO: Look into #360 properly
    virtual void StartTick() override;
    virtual void ProcessMessage(const Message& messages) override;
    virtual void FinishTick() override;

    virtual void Represent(const QVector<PlayerAndFrame>& frames) const override;

    virtual qint32 GetGameTick() const override;
    virtual quint32 Hash() const override;

    virtual QByteArray SaveWorld() const override;

    // Game interface
    virtual AtmosInterface& GetAtmosphere() override;
    virtual const AtmosInterface& GetAtmosphere() const override;
    virtual MapInterface& GetMap() override;
    virtual const MapInterface& GetMap() const override;
    virtual ObjectFactoryInterface& GetFactory() override;
    virtual const ObjectFactoryInterface& GetFactory() const override;
    virtual Names& GetNames() override;
    virtual ChatFrameInfo& GetChatFrameInfo() override;
    virtual const ChatFrameInfo& GetChatFrameInfo() const override;

    virtual ObjectProcessorInterface& GetProcessor() override;

    virtual IdPtr<kv::GlobalObjectsHolder> GetGlobals() const override;
    virtual void SetGlobals(quint32 globals) override;

    virtual void SetPlayerId(quint32 net_id, quint32 real_id) override;
    virtual quint32 GetPlayerId(quint32 net_id) const override;
    virtual quint32 GetNetId(quint32 real_id) const override;

    virtual void AddSound(const QString& name, kv::Position position) override;
    virtual void PlayMusic(const QString& name, int volume, quint32 mob) override;

    virtual void PerformUnsync() override;

    void PrepareToMapgen();
    void AfterMapgen(quint32 id, bool unsync_generation);
private:
    void RemoveStaleRepresentation();
    void ProcessInputMessage(const Message& message);

    void PostOoc(const QString& who, const QString& text);

    void AppendSystemTexts(GrowingFrame* frame) const;
    void AppendSoundsToFrame(GrowingFrame* frame, const VisiblePoints& points, quint32 net_id) const;
    void AppendChatMessages(GrowingFrame* frame, const VisiblePoints& points, quint32 net_id) const;

    void ProcessHearers();

    std::unique_ptr<AtmosInterface> atmos_;
    std::unique_ptr<ObjectFactoryInterface> factory_;
    std::unique_ptr<Names> names_;

    kv::ChatFrameInfo chat_frame_info_;

    IdPtr<kv::GlobalObjectsHolder> global_objects_;

    IdPtr<kv::Mob> current_mob_;

    QVector<QPair<kv::Position, QString>> sounds_for_frame_;

    // Perfomance
    qint64 process_messages_ns_;
    qint64 foreach_process_ns_;
    qint64 physics_process_ns_;
    qint64 atmos_process_ns_;
    qint64 deletion_process_ns_;
    qint64 update_visibility_ns_;
    qint64 frame_generation_ns_;
};

class CoreImplementation : public CoreInterface
{
public:
    CoreImplementation();

    virtual WorldPtr CreateWorldFromSave(const QByteArray& data) override;
    virtual WorldPtr CreateWorldFromJson(
        const QJsonObject& data, quint32 mob_id, const Config& config) override;

    virtual const ObjectsMetadata& GetObjectsMetadata() const override;

    virtual const QString& GetGameVersion() const override;
    virtual const QString& GetBuildInfo() const override;
    virtual const QString& GetQtVersion() const override;
};

}
