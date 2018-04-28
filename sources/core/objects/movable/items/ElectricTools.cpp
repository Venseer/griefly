#include "ElectricTools.h"

#include <QTextStream>

#include "objects/Tile.h"

using namespace kv;

AtmosTool::AtmosTool()
{
    SetSprite("icons/device.dmi");
    SetState("atmos");
    SetName("Atmos tool");
}

QString AtmosTool::GetHtmlInfo(const atmos::AtmosHolder& holder)
{
    QString retval;

    QTextStream ss(&retval);
    ss << "<b>Atmos data:</b><br>";
    ss << "Energy: " << holder.GetEnergy() << "<br>";
    ss << "Volume: " << holder.GetVolume() << "<br>";
    ss << "Temperature: " << holder.GetTemperature() << "<br>";
    ss << "Pressure: " << holder.GetPressure() << "<br>";

    int overall_gases = 0;
    for (quint32 i = 0; i < atmos::GASES_NUM; ++i)
    {
        overall_gases += holder.GetGase(i);
    }
    overall_gases = qMax(1, overall_gases);

    for (quint32 i = 0; i < atmos::GASES_NUM; ++i)
    {
        ss << QString("%1: <b>%2%</b> (%3)<br>")
            .arg(atmos::GASES_NAME[i])
            .arg((holder.GetGase(i) * 100.0) / overall_gases, 0, 'f', 2)
            .arg(holder.GetGase(i));
    }
    return retval;
}

void AtmosTool::AttackBy(IdPtr<Item> item)
{
    if (item.Id() == GetId())
    {
        if (IdPtr<MapObject> super_owner = GetOwner()->GetOwner())
        {
            if (atmos::AtmosHolder* holder = super_owner->GetAtmosHolder())
            {
                PostHtmlFor(GetHtmlInfo(*holder), GetOwner());
            }
        }
    }
}

bool AtmosTool::Attack(IdPtr<MapObject> object)
{
    if (atmos::AtmosHolder* holder = object->GetAtmosHolder())
    {
        PostHtmlFor(AtmosTool::GetHtmlInfo(*holder), GetOwner());
        return false;
    }
    return true;
}

RemoteAtmosTool::RemoteAtmosTool()
{
    SetName("Remote atmos tool");
}
