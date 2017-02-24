#include "LoginMob.h"

#include "representation/Text.h"
#include "representation/Sound.h"
#include "Human.h"
#include "../ObjectFactory.h"
#include "../Map.h"
#include "../Game.h"
#include "Lobby.h"
#include "net/MagicStrings.h"
#include "representation/Chat.h"
#include "../Helpers.h"
#include "representation/Representation.h"
#include "Professions.h"

#include <QDebug>

const char* LOGIN_CLICK = "login_click";

LoginMob::LoginMob(quint32 id) : IMob(id)
{
    name = QString("LobbyPlayer%1").arg(id);

    interface_.Init();
}

void LoginMob::DeinitGUI()
{
    GetGame().GetTexts().Delete("LoginScreenCount");
    PlayMusic("");
}


void LoginMob::InitGUI()
{
    GetGame().GetTexts()["LoginScreenCount"].SetUpdater
    ([this](QString* str)
    {
        if (GetLobby().GetSecondUntilStart() < 0)
        {
            *str = "Round is in process, click on the screen";
            return;
        }
        else
        {
            *str = QString::number(GetLobby().GetSecondUntilStart());
        }
        *str = "Until start: " + *str;
    });

    PlayMusic("lobby.ogg", 10.0f);
}


void LoginMob::GenerateInterfaceForFrame()
{
    interface_.Draw();
}

const QString GENERIC_TEXT =
    "A space anomaly has moved a piece of your Space Station into an unknown part of space."
    " All laws and rules are gone, and you are just a poor <b>%1</b>."
    " Seems like atmos still works, so maybe you won't die from asphyxiation.<br>";

const int HUMAN_STATES_AMOUNT = 12;
const QString HUMAN_STATES[HUMAN_STATES_AMOUNT] =
    {"caucasian1_m_s", "caucasian2_m_s", "caucasian3_m_s",
     "latino_m_s", "mediterranean_m_s", "asian1_m_s",
     "asian2_m_s", "arab_m_s", "indian_m_s",
     "african1_m_s", "african2_m_s", "albino_m_s"};

void LoginMob::ProcessMessage(const Message2& msg)
{
    QJsonObject obj = Network2::ParseJson(msg);

    if (    msg.type == MessageType::ORDINARY
         && Network2::IsKey(obj, LOGIN_CLICK))
    {
        if (GetLobby().GetSecondUntilStart() > 0)
        {
            return;
        }
        qDebug() << "Begin human creation in LoginMob";
        quint32 net_id = GetGame().GetFactory().GetNetId(GetId());
        if (net_id)
        {
            IdPtr<Human> human = Create<Human>(Human::T_ITEM_S());

            QString human_state = HUMAN_STATES[GetRand() % HUMAN_STATES_AMOUNT];
            human->SetState(human_state);

            std::vector<IdPtr<CubeTile>> tiles;
            QString text;
            switch (GetRand() % 5)
            {
            case 0:
                professions::ToSecurityOfficer(human);
                tiles = GetLobby().GetTilesFor("security");
                text = GENERIC_TEXT.arg("security officer");
                break;
            case 1:
                professions::ToDoctor(human);
                tiles = GetLobby().GetTilesFor("doctor");
                text = GENERIC_TEXT.arg("doctor");
                break;
            case 2:
                professions::ToAssistant(human);
                tiles = GetLobby().GetTilesFor("assistant");
                text = GENERIC_TEXT.arg("assistant");
                break;
            case 3:
                professions::ToClown(human);
                tiles = GetLobby().GetTilesFor("clown");
                text = GENERIC_TEXT.arg("clown");
                break;
            case 4:
                professions::ToBarman(human);
                tiles = GetLobby().GetTilesFor("barman");
                text = GENERIC_TEXT.arg("barman");
                break;
            default:
                qDebug() << "Unknown profession id!";
            }

            GetGame().GetFactory().SetPlayerId(net_id, human.Id());

            if (tiles.empty())
            {
                auto& map = GetGame().GetMap();
                int x = map.GetWidth() / 2;
                int y = map.GetHeight() / 2;
                int z = map.GetDepth() / 2;
                tiles.push_back(map.GetSquares()[x][y][z]);
            }
            int index = GetRand() % tiles.size();
            tiles[index]->AddItem(human);
            if (GetId() == GetGame().GetMob().Id())
            {
                GetGame().ChangeMob(human);
            }

            GetGame().GetChat().PostHtmlFor(text, human);
        }
        qDebug() << "End human creation in LoginMob";
    }
}

void LoginMob::Process()
{

}

void LoginMob::CalculateVisible(std::list<PosPoint>* visible_list)
{
    visible_list->clear();
}

FastSerializer& operator<<(FastSerializer& file, LoginInterface& interf)
{
    WrapWriteMessage(file, interf.view_);
    return file;
}
FastDeserializer& operator>>(FastDeserializer& file, LoginInterface& interf)
{
    WrapReadMessage(file, interf.view_);
    return file;
}

void LoginInterface::Init()
{
    view_.SetSprite("icons/login_screen.jpg");
}

void LoginInterface::Draw()
{
    //view_.Draw(0, 0, 0);
    Representation::InterfaceUnit unit;
    unit.name = LOGIN_CLICK;
    unit.pixel_x = 0;
    unit.pixel_y = 0;
    unit.view = view_;
    GetRepresentation().AddToNewFrame(unit);
    //qDebug() << "Login interface";
}

unsigned int LoginInterface::hash() const
{
    return ::hash(view_);
}
