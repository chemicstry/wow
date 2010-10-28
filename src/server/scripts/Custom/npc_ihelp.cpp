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
 */
#include "ScriptPCH.h"
#include <cstring>

enum eEnums
{
    SPELL_POLYMORPH         = 12826,
    SPELL_MARK_OF_THE_WILD  = 26990,

    SAY_NOT_INTERESTED      = -1999922,
    SAY_WRONG               = -1999923,
    SAY_CORRECT             = -1999924,
	SOUND_1                 = 7224
};

#define GOSSIP_MAIN_1       "Teleport Me!"
#define GOSSIP_MAIN_2       "Buff Me!"
#define GOSSIP_MAIN_3       "Token Exchange"

class npc_ihelp : public CreatureScript
{
    public:

        npc_ihelp() : CreatureScript("npc_ihelp") {}

        bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_MAIN_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GOSSIP_MAIN_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, GOSSIP_MAIN_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);

            player->PlayerTalkClass->SendGossipMenu(907, creature->GetGUID());
			
			creature->PlayDirectSound(SOUND_1, player);

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
			switch(action)
			{
				case GOSSIP_ACTION_INFO_DEF+1:
					player->CLOSE_GOSSIP_MENU();
				
				case GOSSIP_ACTION_INFO_DEF+2:
					player->CLOSE_GOSSIP_MENU();
					
				case GOSSIP_ACTION_INFO_DEF+3:
					player->CLOSE_GOSSIP_MENU();
			}

            return true;
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
};

void AddSC_npc_ihelp()
{
    new npc_ihelp();
}