#include "Pipes.h"

#include "objects/Tile.h"
#include "objects/movable/items/ElectricTools.h"

using namespace kv;

PipeBase::PipeBase()
{
    anchored_ = true;
    v_level = 1;

    SetSprite("icons/pipes.dmi");
    //SetState("intact");

    SetName("Please do not create me");

    AddDefaultValues(&atmos_holder_);
}

void PipeBase::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<AtmosTool> tool = item)
    {
        PostHtmlFor(AtmosTool::GetHtmlInfo(atmos_holder_), tool->GetOwner());
        return;
    }
}

void PipeBase::ConnectHelper(IdPtr<PipeBase>& connection, Dir dir)
{
    if (!connection.IsValid())
    {
        GetNeighbour(dir)->ForEach(
        [&](IdPtr<MapObject> obj)
        {
            if (connection.IsValid())
            {
                return;
            }
            if (IdPtr<PipeBase> pipe = obj)
            {
                if (pipe->Connect(RevertDir(dir), GetId()))
                {
                    connection = pipe;
                }
            }
        });
    }
}

void PipeBase::ProcessHelper(IdPtr<PipeBase>& connection, Dir dir)
{
    if (connection.IsValid())
    {
        if (connection->CanTransferGas(RevertDir(dir)))
        {
            connection->GetAtmosHolder()->Connect(GetAtmosHolder());
        }
    }
    else
    {
        if (IdPtr<CubeTile> cube = GetOwner())
        {
            cube->GetAtmosHolder()->Connect(GetAtmosHolder());
        }
    }
}


Pipe::Pipe()
{
    SetState("intact");

    SetName("Pipe");
}

bool Pipe::Connect(Dir dir, IdPtr<PipeBase> pipe)
{
    Dir head;
    Dir tail;
    GetTailAndHead(GetDir(), &head, &tail);

    if (   (dir == head)
        && (!head_.IsValid()))
    {
        head_ = pipe;
        return true;
    }
    if (   (dir == tail)
        && (!tail_.IsValid()))
    {
        tail_ = pipe;
        return true;
    }

    return false;
}

void Pipe::AfterWorldCreation()
{
    PipeBase::AfterWorldCreation();
    SetFreq(1);

    Dir head;
    Dir tail;
    GetTailAndHead(GetDir(), &head, &tail);

    ConnectHelper(head_, head);
    ConnectHelper(tail_, tail);
}

void Pipe::Process()
{
    Dir head;
    Dir tail;
    GetTailAndHead(GetDir(), &head, &tail);

    ProcessHelper(head_, head);
    ProcessHelper(tail_, tail);
}

void Pipe::GetTailAndHead(Dir dir, Dir* head, Dir* tail)
{
    static Dir DIRS_DATA[10][2]
        = {
        {Dir::WEST, Dir::EAST}, // Dir::LEFT
        {Dir::EAST, Dir::WEST}, // Dir::RIGHT
        {Dir::NORTH, Dir::SOUTH}, // Dir::UP
        {Dir::SOUTH, Dir::NORTH}, // Dir::DOWN
        {Dir::UP, Dir::DOWN}, // Dir::ZUP
        {Dir::DOWN, Dir::UP}, // Dir::ZDOWN
        {Dir::EAST, Dir::SOUTH}, // D_SOUTHEAST
        {Dir::SOUTH, Dir::WEST}, // D_SOUTHWEST
        {Dir::NORTH, Dir::EAST}, // D_NORTHEAST
        {Dir::WEST, Dir::NORTH} // D_NORTHWEST
        };
    *head = DIRS_DATA[static_cast<int>(dir)][0];
    *tail = DIRS_DATA[static_cast<int>(dir)][1];
}

Manifold::Manifold()
{
    SetState("manifold");
    SetName("Manifold");
}

bool Manifold::Connect(Dir dir, IdPtr<PipeBase> pipe)
{
    Dir tail;
    Dir left;
    Dir right;
    GetConnectionsDirs(GetDir(), &tail, &left, &right);

    if (   (dir == tail)
        && (!tail_.IsValid()))
    {
        tail_ = pipe;
        return true;
    }

    if (   (dir == left)
        && (!left_.IsValid()))
    {
        left_ = pipe;
        return true;
    }

    if (   (dir == right)
        && (!right_.IsValid()))
    {
        right_ = pipe;
        return true;
    }

    return false;
}

void Manifold::AfterWorldCreation()
{
    PipeBase::AfterWorldCreation();

    SetFreq(1);

    Dir tail;
    Dir left;
    Dir right;
    GetConnectionsDirs(GetDir(), &tail, &left, &right);

    ConnectHelper(tail_, tail);
    ConnectHelper(left_, left);
    ConnectHelper(right_, right);
}

bool Manifold::CanTransferGas(Dir dir) const
{
    return true;
}

void Manifold::Process()
{
    Dir tail;
    Dir left;
    Dir right;
    GetConnectionsDirs(GetDir(), &tail, &left, &right);

    ProcessHelper(tail_, tail);
    ProcessHelper(left_, left);
    ProcessHelper(right_, right);
}

void Manifold::GetConnectionsDirs(Dir dir, Dir *tail, Dir *left, Dir *right)
{
    *tail = RevertDir(dir);
    if (dir == Dir::SOUTH)
    {
        *left = Dir::EAST;
        *right = Dir::WEST;
    }
    else if (dir == Dir::NORTH)
    {
        *left = Dir::WEST;
        *right = Dir::EAST;
    }
    else if (dir == Dir::EAST)
    {
        *left = Dir::NORTH;
        *right = Dir::SOUTH;
    }
    else
    {
        *left = Dir::SOUTH;
        *right = Dir::NORTH;
    }
}

Vent::Vent()
{
    SetSprite("icons/vent_pump.dmi");

    SetHidden(false);

    v_level = 3;

    SetName("Vent");
}

bool Vent::Connect(Dir dir, IdPtr<PipeBase> pipe)
{
    if (dir != GetDir())
    {
        return false;
    }
    if (tail_.IsValid())
    {
        return false;
    }

    tail_ = pipe;
    return true;
}

void Vent::AfterWorldCreation()
{
    PipeBase::AfterWorldCreation();

    SetFreq(1);
    ConnectHelper(tail_, GetDir());
}

void Vent::Process()
{
    ProcessHelper(tail_, GetDir());

    // TODO: consider to remove such checks
    if (!GetOwner().IsValid())
    {
        return;
    }
    if (atmos::AtmosHolder* holder = GetOwner()->GetAtmosHolder())
    {
        holder->Connect(GetAtmosHolder());
    }
}

void Vent::SetHidden(bool hidden)
{
    if (hidden)
    {
        SetState("hoff");
    }
    else
    {
        SetState("off");
    }
}

Valve::Valve()
{
    SetSprite("icons/digital_valve.dmi");
    SetState("valve0");
    closed_ = true;
}

bool Valve::CanTransferGas(Dir dir) const
{
    if (closed_)
    {
        return false;
    }
    return true;
}

void Valve::Process()
{
    if (closed_)
    {
        return;\
    }
    Pipe::Process();
}

void Valve::AttackBy(IdPtr<Item> item)
{
    if (closed_)
    {
        closed_ = false;
        SetState("valve1");
    }
    else
    {
        closed_ = true;
        SetState("valve0");
    }
}

Connector::Connector()
{
    SetState("connector");
    v_level = 3;

    SetName("Connector");
}

void Connector::ConnectToGasTank(IdPtr<GasTank> tank)
{
    tank_ = tank;
}

void Connector::DisconnectFromGasTank()
{
    tank_ = 0;
}

bool Connector::Connect(Dir dir, IdPtr<PipeBase> pipe)
{
    if (dir != GetDir())
    {
        return false;
    }
    if (tail_.IsValid())
    {
        return false;
    }

    tail_ = pipe;
    return true;
}

void Connector::AfterWorldCreation()
{
    PipeBase::AfterWorldCreation();
    SetFreq(1);
    ConnectHelper(tail_, GetDir());
    if (auto tank = GetOwner()->GetItem<GasTank>())
    {
        ConnectToGasTank(tank);
        tank->anchored_ = true;
    }
}

void Connector::Process()
{
    ProcessHelper(tail_, GetDir());
    if (tank_)
    {
        tank_->GetAtmosHolder()->Connect(GetAtmosHolder());
    }
}

PipePump::PipePump()
{
    SetSprite("icons/pipes2.dmi");
    SetState("pipepump-run");

    pump_pressure_ = 38000;
}

bool PipePump::CanTransferGas(Dir dir) const
{
    if (RevertDir(dir) == GetDir())
    {
        return true;
    }
    return false;
}

void PipePump::Process()
{
    Dir head;
    Dir tail;
    GetTailAndHead(GetDir(), &head, &tail);
    ProcessHelper(tail_, tail);
    atmos::AtmosHolder* head_connection = nullptr;
    if (head_)
    {
        head_connection = head_->GetAtmosHolder();
    }
    if (head_connection == nullptr)
    {
        if (IdPtr<CubeTile> cube = GetOwner())
        {
            head_connection = cube->GetAtmosHolder();
        }
    }
    if (head_connection == nullptr)
    {
        return;
    }

    if (head_connection->GetPressure() >= pump_pressure_)
    {
        return;
    }

    head_connection->Connect(
        GetAtmosHolder(),
        atmos::MAX_GAS_LEVEL,
        atmos::MAX_GAS_LEVEL / 4,
        atmos::MAX_GAS_LEVEL);
}
