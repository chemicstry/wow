/**
*
* @File : npc_ihelp.cpp
*
* @Authors : Intel
*
* @Date : 27/10/2010
*
* @Version : 1.0
*
**/
 /*
player->ADD_GOSSIP_ITEM_EXTENDED(0, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1, "", 0, true);
if (action == GOSSIP_ACTION_INFO_DEF+2)
{
DoScriptText(SAY_NOT_INTERESTED, creature);
player->CLOSE_GOSSIP_MENU();
}

        bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
        {
            if (sender == GOSSIP_SENDER_MAIN)
            {
                switch (action)
                {
                case GOSSIP_ACTION_INFO_DEF+1:
                    if (std::strcmp(code, player->GetName()) != 0)
                    {
                        DoScriptText(SAY_WRONG, creature);
                        creature->CastSpell(player, SPELL_POLYMORPH, true);
                    }
                    else
                    {
                        DoScriptText(SAY_CORRECT, creature);
                        creature->CastSpell(player, SPELL_MARK_OF_THE_WILD, true);
                    }
                    player->CLOSE_GOSSIP_MENU();

                    return true;
                }
            }

            return false;
        }
*/
#include "ScriptPCH.h"
#include <cstring>

enum eEnums
{
    SPELL_BUFF_1           = 58054, // Blessing of kings
    SPELL_BUFF_2           = 42995, // Arcane Intellect
    SPELL_BUFF_3           = 48161, // Power Word: Fortitude
    SPELL_BUFF_4           = 48469, // Mark of the Wild
    SPELL_BUFF_5           = 48102, // Scroll of Stamina
    SPELL_BUFF_6_0         = 48100, // Scroll of Intellect
    SPELL_BUFF_6_1         = 58449, // Scroll of Strenght
    SPELL_BUFF_6_2         = 58451, // Scroll of Agility
    
    SPELL_RESURRECTION_SICKNESS = 15007,

    SOUND_1                = 5934, // Gnome greetings
    SOUND_2                = 5935, // Gnome goodbye
    SOUND_3                = 5936, // Gnome pissed
};

#define GOSSIP_NUM_DEST 8
#define GOSSIP_NO_DEST "I have no destinations!!!"
#define GOSSIP_PREV "<- [Previous Page]"
#define GOSSIP_NEXT "-> [Next Page]"

#define GOSSIP_MAIN_1 "Teleport Me!"
#define GOSSIP_MAIN_2 "Buff Me!"
#define GOSSIP_MAIN_3 "Remove Resurrection Sickness"
#define GOSSIP_MAIN_4 "Token Exchange"

// Teleport Type Menu
#define GOSSIP_TELE_1 "Cities"
#define GOSSIP_TELE_2 "Zones"
#define GOSSIP_TELE_3 "Instances"
#define GOSSIP_TELE_4 "Raids"
#define GOSSIP_TELE_5 "Arenas"

// Zone Type Menu
#define GOSSIP_MAP_0 "Eastern Kingdoms"
#define GOSSIP_MAP_1 "Kalimdor"
#define GOSSIP_MAP_2 "Outlands"
#define GOSSIP_MAP_3 "Northrend"

// Instance/Raid Type Menu
#define GOSSIP_DUNGEON_0 "Vanilla"
#define GOSSIP_DUNGEON_1 "Burning Crusade"
#define GOSSIP_DUNGEON_2 "Wrath of the Lich King"

class npc_ihelp : public CreatureScript
{
    public:

        npc_ihelp() : CreatureScript("npc_ihelp") {}

        bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_MAIN_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GOSSIP_MAIN_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GOSSIP_MAIN_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, GOSSIP_MAIN_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);

            player->PlayerTalkClass->SendGossipMenu(907, creature->GetGUID());

            creature->PlayDirectSound(SOUND_1, player);

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch(action)
            {
                case GOSSIP_ACTION_INFO_DEF+2:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_TELE_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+110);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_TELE_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+62);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_TELE_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+63);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_TELE_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+64);
                    //player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_TELE_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+50);
                    player->SEND_GOSSIP_MENU(907, creature->GetGUID());
                    break;

                case GOSSIP_ACTION_INFO_DEF+3:
                    player->CLOSE_GOSSIP_MENU();
                    creature->CastSpell(player, SPELL_BUFF_1, true);
                    creature->CastSpell(player, SPELL_BUFF_2, true);
                    creature->CastSpell(player, SPELL_BUFF_3, true);
                    creature->CastSpell(player, SPELL_BUFF_4, true);
                    creature->CastSpell(player, SPELL_BUFF_5, true);
                    switch (player->getClass())
                    {
                        case 2:
                        case 5:
                        case 7:
                        case 8:
                        case 9:
                        case 11:
                            creature->CastSpell(player, SPELL_BUFF_6_0, true);
                            break;
                         
                        case 4:
                            creature->CastSpell(player, SPELL_BUFF_6_2, true);
                            break;
                            
                        default:
                            creature->CastSpell(player, SPELL_BUFF_6_1, true);
                            break;
                    }
                    creature->PlayDirectSound(SOUND_2, player);
                    break;

                case GOSSIP_ACTION_INFO_DEF+4:
                    player->CLOSE_GOSSIP_MENU();
                    if(player->HasAura(SPELL_RESURRECTION_SICKNESS,0))
                    {
                        creature->CastSpell(player,38588,false); // Healing effect
                        player->RemoveAurasDueToSpell(SPELL_RESURRECTION_SICKNESS);
                    }
                    creature->PlayDirectSound(SOUND_2, player);
                    break;
                    
                case GOSSIP_ACTION_INFO_DEF+5:
                    player->CLOSE_GOSSIP_MENU();
                    creature->PlayDirectSound(SOUND_2, player);
                    break;
                    
                case GOSSIP_ACTION_INFO_DEF+6:  // No tele locations
                    player->CLOSE_GOSSIP_MENU();
                    creature->PlayDirectSound(SOUND_2, player);
                    break;
                    
                case GOSSIP_ACTION_INFO_DEF+62:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_MAP_0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+120);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_MAP_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+130);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_MAP_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+140);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_MAP_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+150);
                    player->SEND_GOSSIP_MENU(907, creature->GetGUID());
                    break;
                    
                case GOSSIP_ACTION_INFO_DEF+63:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_DUNGEON_0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+160);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_DUNGEON_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+170);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_DUNGEON_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+180);
                    player->SEND_GOSSIP_MENU(907, creature->GetGUID());
                    break;
                    
                case GOSSIP_ACTION_INFO_DEF+64:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_DUNGEON_0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+190);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_DUNGEON_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+200);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_DUNGEON_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+210);
                    player->SEND_GOSSIP_MENU(907, creature->GetGUID());
                    break;
                    
                default:
                    if (action >= GOSSIP_ACTION_INFO_DEF+100 && action <= GOSSIP_ACTION_INFO_DEF+300)
                    {
                        ShowDest(player, creature, action);
                    }
                    else
                    {
                        player->CLOSE_GOSSIP_MENU();
                        creature->PlayDirectSound(SOUND_2, player);
                    }
                    break;
            }

            return true;
        }
        
        void ShowDest(Player* player, Creature* creature, uint32 action)
        {
            uint32 mysql_limit = 7;
            uint32 i;
            
            if ((action - (floor(action / 10)) * 10 == 0) || (action - (floor(action / 10)) * 10 == 9))
            {
                mysql_limit = 8;
            }
            
            QueryResult result = WorldDatabase.PQuery("SELECT * FROM custom_npc_tele_destination WHERE type = %d LIMIT %d OFFSET %d", floor(action / 10) - 10, mysql_limit, ((action - ((floor(action / 10)) * 10))) * 7);
            
            if (action - (floor(action / 10)) * 10 != 0)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_PREV, GOSSIP_SENDER_MAIN, action - 1);
            
            if (result)
            {
                do
                {
                    Field *fields = result->Fetch();
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, fields[1].GetString(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+300+fields[0].GetUInt32());
                    i++;
                } while (result->NextRow());
            } else player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_NO_DEST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            
            if ((action - (floor(action / 10)) * 10 != 9) && (i == mysql_limit))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_NEXT, GOSSIP_SENDER_MAIN, action + 1);
            
            player->SEND_GOSSIP_MENU(907, creature->GetGUID());
        }
};

void AddSC_npc_ihelp()
{
    new npc_ihelp();
}