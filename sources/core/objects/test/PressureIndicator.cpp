#include "PressureIndicator.h"

#include "../Tile.h"

using namespace kv;

PressureIndicator::PressureIndicator()
{
    SetVisibleLevel(11);
    SetName("Pressure indicator");
    SetSprite("icons/numbers.dmi");
    SetState("empty");
}

void PressureIndicator::Process()
{
    if (IdPtr<CubeTile> tile = GetRoot())
    {
        atmos::AtmosHolder* holder = tile->GetAtmosHolder();
        SetNumber(holder->GetPressure() / 1000);
    }
}

void PressureIndicator::AfterWorldCreation()
{
    MaterialObject::AfterWorldCreation();
    SetFreq(1);
}

void PressureIndicator::SetNumber(int number)
{
    if (number >= 100)
    {
        number = 99;
    }
    if (number < 0)
    {
        number = 0;
    }

    static const QString NUMBERS[10] =
        { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    static const QString SPRITE = "icons/numbers.dmi";

    int first_digit = number / 10;
    int second_digit = number % 10;

    GetView().RemoveOverlays();
    GetView().AddOverlay(SPRITE, NUMBERS[first_digit]).SetShift(-7, 0);
    GetView().AddOverlay(SPRITE, NUMBERS[second_digit]).SetShift(7, 0);
}
