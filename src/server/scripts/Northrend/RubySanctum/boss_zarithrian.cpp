/* Copyright (C) 2010 Easy for TrinityCore <http://trinity-core.ru/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "ScriptPCH.h"
#include "ruby_sanctum.h"

enum eTexts
{
	SAY_AGGRO = -1752017,
	SAY_SLAY1 = -1752018,
	SAY_SLAY2 = -1752019,
	SAY_DEATH = -1752020
};

enum eSpells
{
	SPELL_CLEAVE_ARMOR         = 74367,
	SPELL_INTIMIDATING_ROAR    = 74384
};

enum eEvents
{
    EVENT_CAST_CLEAVE_ARMOR      = 1,
    EVENT_CAST_INTIMIDATING_ROAR = 2
};

class boss_zarithrian : public CreatureScript
{
    public:
        boss_zarithrian() : CreatureScript("boss_zarithrian") { }

        struct boss_zarithrianAI : public BossAI
        {
            boss_zarithrianAI(Creature *pCreature) : BossAI(pCreature, DATA_ZARITHRIAN)
    	    {
                ASSERT(instance);
    	    }

    	    void Reset()
    	    {
                instance->SetBossState(DATA_ZARITHRIAN, NOT_STARTED);
                events.Reset();
                events.ScheduleEvent(EVENT_CAST_CLEAVE_ARMOR, urand(3500,4500));
                events.ScheduleEvent(EVENT_CAST_INTIMIDATING_ROAR, urand(10000,11000));
    	    }

    	    void EnterCombat(Unit*)
    	    {
                instance->SetBossState(DATA_ZARITHRIAN, IN_PROGRESS);
                DoScriptText(SAY_AGGRO, me);
    	    }

    	    void UpdateAI(const uint32 diff)
    	    {
                if(!UpdateVictim() || !CheckInRoom())
			        return;

                if (me->hasUnitState(UNIT_STAT_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CAST_CLEAVE_ARMOR:
                            DoCastVictim(SPELL_CLEAVE_ARMOR);
                            events.ScheduleEvent(EVENT_CAST_CLEAVE_ARMOR, urand(3500,4500));
                            break;
                        case EVENT_CAST_INTIMIDATING_ROAR:
                            DoCast(SPELL_INTIMIDATING_ROAR);
                            events.ScheduleEvent(EVENT_CAST_INTIMIDATING_ROAR, urand(10000,11000));
                            break;
                    }
                }

    		    DoMeleeAttackIfReady();
    	    }

    	    void KilledUnit(Unit *victim)
    	    {
                DoScriptText(RAND(SAY_SLAY1,SAY_SLAY2), me);
    	    }

    	    void JustDied(Unit*)
    	    {
                _JustDied();
                DoScriptText(SAY_DEATH, me);
    	    }
        };

        CreatureAI* GetAI(Creature *pCreature) const
        {
    	    return new boss_zarithrianAI(pCreature);
        }

};

void AddSC_boss_zarithrian()
{
    new boss_zarithrian;
}
