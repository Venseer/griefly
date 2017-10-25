#pragma once

#include "core/objects/movable/Movable.h"

#include "net/Network2.h"

#include "CoreInterface.h"

namespace kv
{

class Mob : public Movable
{
public:
    DECLARE_SAVEABLE(Mob, Movable);
    REGISTER_CLASS_AS(imob);
    Mob();
    virtual void AfterWorldCreation() override;
    virtual void Delete();

    virtual void MindEnter() { }
    virtual void MindExit() { }
    virtual void GenerateInterfaceForFrame(GrowingFrame* representation);
    virtual void ProcessMessage(const WorldInterface::Message& message);

    void MoveMindTo(IdPtr<Mob> other);
    bool IsMinded() const;

    virtual void CalculateVisible(VisiblePoints* visible_list) const { Q_UNUSED(visible_list); }
};
END_DECLARE(Mob);

}
