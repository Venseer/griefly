#include "Clothes.h"

#include "Slot.h"

Cloth::Cloth(quint32 id) : Item(id)
{
    name = "SimpleCloth";
}

Armor::Armor(quint32 id) : Cloth(id)
{
    SetSprite("icons/suits.dmi");
    SetState("armor");

    type = Slots::SUIT;

    name = "Armor";
}

Helmet::Helmet(quint32 id) : Cloth(id)
{
    SetSprite("icons/hats.dmi");
    SetState("helmet");

    type = Slots::HEAD;

    name = "Armor";
}

Tophat::Tophat(quint32 id) : Cloth(id)
{
    SetSprite("icons/hats.dmi");
    SetState("tophat");

    type = Slots::HEAD;

    name = "Tophat";
}

EngineUniform::EngineUniform(quint32 id) : Cloth(id)
{
    SetSprite("icons/uniforms.dmi");
    SetState("engine");

    type = Slots::UNIFORM;

    name = "Engine uniform";
}

RedUniform::RedUniform(quint32 id) : Cloth(id)
{
    SetSprite("icons/uniforms.dmi");
    SetState("red");

    type = Slots::UNIFORM;

    name = "Red uniform";
}

JanitorUniform::JanitorUniform(quint32 id) : Cloth(id)
{
    SetSprite("icons/uniforms.dmi");
    SetState("janitor");

    type = Slots::UNIFORM;

    name = "Janitor uniform";
}

ClownUniform::ClownUniform(quint32 id) : Cloth(id)
{
    SetSprite("icons/uniforms.dmi");
    SetState("clown");

    type = Slots::UNIFORM;

    name = "Clown uniform";
}

BarmanUniform::BarmanUniform(quint32 id) : Cloth(id)
{
    SetSprite("icons/uniforms.dmi");
    SetState("ba_suit");

    type = Slots::UNIFORM;

    name = "Barman uniform";
}

OrangeBoots::OrangeBoots(quint32 id) : Cloth(id)
{
    SetSprite("icons/shoes.dmi");
    SetState("orange");

    type = Slots::FEET;

    name = "Orange boots";
}

BlackBoots::BlackBoots(quint32 id) : Cloth(id)
{
    SetSprite("icons/shoes.dmi");
    SetState("black");

    type = Slots::FEET;

    name = "Black boots";
}

ClownBoots::ClownBoots(quint32 id) : Cloth(id)
{
    SetSprite("icons/shoes.dmi");
    SetState("clown");

    type = Slots::FEET;

    name = "Clown boots";
}

MedicalUniform::MedicalUniform(quint32 id) : Cloth(id)
{
    SetSprite("icons/uniforms.dmi");
    SetState("medical");

    type = Slots::UNIFORM;

    name = "Medical uniform";
}

WhiteShoes::WhiteShoes(quint32 id) : Cloth(id)
{
    SetSprite("icons/shoes.dmi");
    SetState("white");

    type = Slots::FEET;

    name = "White shoes";
}
