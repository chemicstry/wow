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
        SAY_SPAWN       = -1752027,
        SAY_AGGRO       = -1752028,
        SAY_SLAY1       = -1752029,
        SAY_SLAY2       = -1752030,
        SAY_DEATH       = -1752031,
        SAY_BERSERK     = -1752032,
        SAY_SPECIAL1    = -1752033,
        SAY_SPECIAL2    = -1752034,
        SAY_PHASE2      = -1752035,
        SAY_PHASE3      = -1752036
};

enum eSpells
{
    //All
    SPELL_TWILIGHT_PRECISION        = 78243,
    SPELL_CLEAVE                    = 74524,
    SPELL_TAIL_LASH                 = 74531,
    SPELL_BERSEK                    = 26662,
    SPELL_TWILIGHT_DIVISION         = 75063,
    SPELL_SUMMON_TWILIGHT_PORTAL    = 74809,

    //halion
    SPELL_FLAME_BREATH_10_NORMAL    = 74525,
    SPELL_FLAME_BREATH_25_NORMAL    = 74526,
    SPELL_FLAME_BREATH_10_HEROIC    = 74527,
    SPELL_FLAME_BREATH_25_HEROIC    = 74528,
    SPELL_FIERY_COMBUSTION          = 74562,

    //twilight halion
    SPELL_DUSK_SHROUD               = 75476,
    SPELL_SOUL_CONSUMPTION          = 74792,
    SPELL_DARK_BREATH_10_NORMAL     = 74806,
    SPELL_DARK_BREATH_25_NORMAL     = 75954,
    SPELL_DARK_BREATH_10_HEROIC     = 75955,
    SPELL_DARK_BREATH_25_HEROIC     = 75956,

    SPELL_CORPOREALITY_EVEN         = 74826,
    SPELL_CORPOREALITY_20I          = 74827, 
    SPELL_CORPOREALITY_40I          = 74828,
    SPELL_CORPOREALITY_60I          = 74829,
    SPELL_CORPOREALITY_80I          = 74830,
    SPELL_CORPOREALITY_100I         = 74831,
    SPELL_CORPOREALITY_20D          = 74832,
    SPELL_CORPOREALITY_40D          = 74833,
    SPELL_CORPOREALITY_60D          = 74834,
    SPELL_CORPOREALITY_80D          = 74835,
    SPELL_CORPOREALITY_100D         = 74836
};

enum eEvents
{
    EVENT_CAST_TWILIGHT_PRECISION   = 1,
    EVENT_CAST_CLEAVE               = 2,
    EVENT_CAST_FIERY_COMBUSTION     = 3,
    EVENT_CAST_TAIL_LASH            = 4,
    EVENT_CAST_FLAME_BREATH         = 5,
    EVENT_CAST_DARK_BREATH          = 6,
    EVENT_CAST_SOUL_CONSUMPTION     = 7,
    EVENT_CAST_DUSK_SHROUD          = 8,
    EVENT_FLAME_WALL                = 9,
    EVENT_CAST_BERSEK               = 10,
    EVENT_DPS                       = 11
};

enum ePhases
{
    PHASE_ALL = 0,
    PHASE_1   = 1,
    PHASE_2   = 2,
    PHASE_3   = 3,

    PHASE_1_MASK  = 1 << PHASE_1,
    PHASE_2_MASK  = 1 << PHASE_2,
    PHASE_3_MASK  = 1 << PHASE_3
};

Creature* pHalion;
Creature* pTwilight;
uint32 HalionDamage, TwilightDamage;
static const Position SpawnPos = {3144.93f, 527.233f, 72.8887f, 0.110395f};

class boss_halion : public CreatureScript
{
    public:
        boss_halion() : CreatureScript("boss_halion") { }

        struct boss_halionAI : public BossAI
        {
            boss_halionAI(Creature *pCreature) : BossAI(pCreature, DATA_HALION)
                {
                    ASSERT(instance);
                    pHalion = me;
                }

            void Reset()
    	    {
                if (instance->GetData(DATA_TWILIGHT_HALION)==IN_PROGRESS)
                {
                    pHalion->SetHealth(pTwilight->GetHealth());
                    return;
                }
                instance->SetBossState(DATA_HALION, NOT_STARTED);
                events.Reset();
                events.SetPhase(PHASE_1);
                events.ScheduleEvent(EVENT_CAST_TWILIGHT_PRECISION, 1,PHASE_ALL);
                events.ScheduleEvent(EVENT_CAST_CLEAVE, 15000,PHASE_ALL);
                events.ScheduleEvent(EVENT_CAST_TAIL_LASH, urand(10000,15000),PHASE_ALL);
                events.ScheduleEvent(EVENT_CAST_FLAME_BREATH, urand(20000,25000),PHASE_ALL);
                events.ScheduleEvent(EVENT_CAST_FIERY_COMBUSTION, 15000,PHASE_ALL);
                events.ScheduleEvent(SPELL_BERSEK, 9999999,PHASE_ALL);
                PercentDamage = 0;
                HalionDamage = 0;
    	    }

    	    void EnterCombat(Unit*)
    	    {
                instance->SetBossState(DATA_HALION, IN_PROGRESS);
                
                if(events.GetPhaseMask() & PHASE_3_MASK)
                {
                    pHalion->SetHealth(pTwilight->GetHealth());
                    return;
                }
                
                events.ScheduleEvent(EVENT_FLAME_WALL, 1000,PHASE_1);
                DoScriptText(SAY_AGGRO, me);
    	    }

    	    void UpdateAI(const uint32 diff)
    	    {
                if (!UpdateVictim() || !CheckInRoom())
                    return;

                if (events.GetPhaseMask() & PHASE_3_MASK)
                {
                    if (instance->GetData(DATA_DAMAGE) != 0)
                        me->SetHealth(instance->GetData(DATA_DAMAGE));
                    else
                        me->SetHealth(1);
                }

                if (me->hasUnitState(UNIT_STAT_CASTING))
                    return;

                events.Update(diff);

                GameObject* go_flame;

                if ((events.GetPhaseMask() & PHASE_1_MASK) || (events.GetPhaseMask() & PHASE_3_MASK))
                {
                    while (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case EVENT_CAST_TWILIGHT_PRECISION:
                                DoCast(me, SPELL_TWILIGHT_PRECISION);
                                break;
                            case EVENT_FLAME_WALL:
                                go_flame = me->SummonGameObject(GO_FLAME_WALLS2, (float)3154.56, (float)535.418, (float)72.8889, (float)4.47206, 0, 0, (float)0.786772, (float)-0.617243, 300);
                                go_flame->SetPhaseMask(0,true);
                                break;
                            case EVENT_CAST_TAIL_LASH:
                                DoCast(me, SPELL_TAIL_LASH);
                                events.ScheduleEvent(EVENT_CAST_TAIL_LASH, urand(10000,15000),PHASE_ALL);
                                break;
                            case EVENT_CAST_CLEAVE:
                                DoCast(me, SPELL_CLEAVE);
                                events.ScheduleEvent(EVENT_CAST_CLEAVE, 15000,PHASE_ALL);
                                break;
                            case EVENT_CAST_FLAME_BREATH:
                                DoCast(me, RAID_MODE(SPELL_FLAME_BREATH_10_NORMAL, SPELL_FLAME_BREATH_25_NORMAL, SPELL_FLAME_BREATH_10_HEROIC, SPELL_FLAME_BREATH_25_HEROIC));
                                events.ScheduleEvent(EVENT_CAST_FLAME_BREATH, urand(20000,25000),PHASE_ALL);break;
                            case EVENT_DPS:
                                CastCorporeality();
                                events.ScheduleEvent(EVENT_DPS, 5000,PHASE_3);
                                break;
                            case EVENT_CAST_FIERY_COMBUSTION:
                                Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0);
    				            if (pTarget && !pTarget->HasAura(SPELL_FIERY_COMBUSTION))
    					            DoCast(pTarget, SPELL_FIERY_COMBUSTION);
                                events.ScheduleEvent(EVENT_CAST_FIERY_COMBUSTION, 15000,PHASE_ALL);
                                break;
                        }
                    }
                }

                if ((events.GetPhaseMask() & PHASE_1_MASK) && ((me->GetHealth()*100) / me->GetMaxHealth() <= 75))
    			{
                    events.SetPhase(PHASE_2);
    				DoScriptText(SAY_PHASE2, me);
    				me->SetReactState(REACT_PASSIVE);
    				me->AttackStop();
    				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    DoCast(me, SPELL_SUMMON_TWILIGHT_PORTAL);
    				DoCast(me, SPELL_TWILIGHT_DIVISION);
                    me->SummonCreature(NPC_TWILIGHT_HALION,SpawnPos);
                    me->SetVisibility(VISIBILITY_OFF);
                    instance->SetBossState(DATA_HALION, SPECIAL);
    			}
                DoMeleeAttackIfReady();
    	    }

            void CastCorporeality()
            {
                if (!(events.GetPhaseMask() & PHASE_3_MASK))
                        return;
                uint32 TotalDamage = HalionDamage+TwilightDamage;
                if (TotalDamage > 0)
                {
                    PercentDamage = HalionDamage / TotalDamage * 100;
                }
                else
                {
                    PercentDamage=50;
                }
                HalionDamage = 0;
                TwilightDamage = 0;

                if (PercentDamage < 5)
                {
                    pTwilight->CastSpell(pTwilight, SPELL_CORPOREALITY_100I, true);
                    pHalion->CastSpell(pHalion, SPELL_CORPOREALITY_100D, true);
                }
                else if (PercentDamage < 15)
                {
                    pTwilight->CastSpell(pTwilight, SPELL_CORPOREALITY_80I, true);
                    pHalion->CastSpell(pHalion, SPELL_CORPOREALITY_80D, true);
                }
                else if (PercentDamage < 25)
                {
                    pTwilight->CastSpell(pTwilight, SPELL_CORPOREALITY_60I, true);
                    pHalion->CastSpell(pHalion, SPELL_CORPOREALITY_60D, true);
                }
                else if (PercentDamage < 35)
                {
                    pTwilight->CastSpell(pTwilight, SPELL_CORPOREALITY_40I, true);
                    pHalion->CastSpell(pHalion, SPELL_CORPOREALITY_40D, true);
                }
                else if (PercentDamage < 45)
                {
                    pTwilight->CastSpell(pTwilight, SPELL_CORPOREALITY_20I, true);
                    pHalion->CastSpell(pHalion, SPELL_CORPOREALITY_20D, true);
                }
                else if (PercentDamage < 55)
                {
                    pTwilight->CastSpell(pTwilight, SPELL_CORPOREALITY_EVEN, true);
                    pHalion->CastSpell(pHalion, SPELL_CORPOREALITY_EVEN, true);
                }
                else if (PercentDamage < 65)
                {
                    pHalion->CastSpell(pHalion, SPELL_CORPOREALITY_20I, true);
                    pTwilight->CastSpell(pTwilight, SPELL_CORPOREALITY_20D, true);
                }
                else if (PercentDamage < 75)
                {
                    pHalion->CastSpell(pHalion, SPELL_CORPOREALITY_40I, true);
                    pTwilight->CastSpell(pTwilight, SPELL_CORPOREALITY_40D, true);
                }
                else if (PercentDamage < 85)
                {
                    pHalion->CastSpell(pHalion, SPELL_CORPOREALITY_60I, true);
                    pTwilight->CastSpell(pTwilight, SPELL_CORPOREALITY_60D, true);
                }
                else if (PercentDamage < 95)
                {
                    pHalion->CastSpell(pHalion, SPELL_CORPOREALITY_80I, true);
                    pTwilight->CastSpell(pTwilight, SPELL_CORPOREALITY_80D, true);
                }
                else
                {
                    pHalion->CastSpell(pHalion, SPELL_CORPOREALITY_100I, true);
                    pTwilight->CastSpell(pTwilight, SPELL_CORPOREALITY_100D, true);
                }
            }

            void SetPhase3()
            {
                events.SetPhase(PHASE_3);
                GameObject* portal = me->SummonGameObject(GO_TWILIGHT_PORTAL2, (float)3154.56, (float)535.418, (float)72.8889, (float)4.47206, 0, 0, (float)0.786772, (float)-0.617243, 300000);
                portal->SetPhaseMask(32,true);
                portal->SetRespawnTime(9999999);
                events.ScheduleEvent(EVENT_DPS, 5000,PHASE_3);
                pHalion->SetHealth(pTwilight->GetHealth());
            }

            void DamageTaken(Unit *who, uint32 &dmg)
            {
                if (!(events.GetPhaseMask() & PHASE_3_MASK))
                        return;
                HalionDamage += dmg;	
                
                instance->SetData(DATA_DAMAGE, me->GetHealth() >= dmg ? me->GetHealth() - dmg : 0);
            }

            void JustSummoned(Creature *summon)
            {
                if(summon->GetEntry()==NPC_TWILIGHT_HALION)
                {
                    summon->SetPhaseMask(32,true);
                }
                summons.Summon(summon);
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

        private:
            uint32 PercentDamage;
        };

        CreatureAI* GetAI(Creature *pCreature) const
        {
                return new boss_halionAI(pCreature);
        }

};

class boss_twilight_halion : public CreatureScript
{
    public:
        boss_twilight_halion() : CreatureScript("boss_twilight_halion") { }

        struct boss_twilight_halionAI : public ScriptedAI
        {
            boss_twilight_halionAI(Creature *pCreature) : ScriptedAI(pCreature)
                {
                    pInstance = me->GetInstanceScript();
                    pTwilight = me;
                }

            void Reset()
    	    {
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                events.Reset();
                events.SetPhase(PHASE_2);
                events.ScheduleEvent(EVENT_CAST_TWILIGHT_PRECISION, 1,PHASE_ALL);
                events.ScheduleEvent(EVENT_CAST_CLEAVE, 15000,PHASE_ALL);
                events.ScheduleEvent(EVENT_CAST_TAIL_LASH, urand(10000,15000),PHASE_ALL);
                events.ScheduleEvent(EVENT_CAST_DUSK_SHROUD, urand(15000,20000),PHASE_ALL);
                events.ScheduleEvent(EVENT_CAST_DARK_BREATH, urand(20000,25000),PHASE_ALL);
                events.ScheduleEvent(EVENT_CAST_SOUL_CONSUMPTION, urand(15000,25000),PHASE_ALL);
                events.ScheduleEvent(EVENT_CAST_BERSEK, 9999999,PHASE_ALL);
                me->SetMaxHealth(pHalion->GetMaxHealth());
                TwilightDamage = 0;
    	    }

    	    void EnterCombat(Unit*)
    	    {
                me->SetHealth(pHalion->GetHealth());
                pInstance->SetData(DATA_TWILIGHT_HALION,IN_PROGRESS);
    	    }

    	    void UpdateAI(const uint32 diff)
    	    {
                if (!UpdateVictim())
                    return;

                if (events.GetPhaseMask() & PHASE_3_MASK)
                {
                    if (pInstance->GetData(DATA_DAMAGE) != 0)
                        me->SetHealth(pInstance->GetData(DATA_DAMAGE));
                    else
                        me->SetHealth(1);
                }

                if (me->hasUnitState(UNIT_STAT_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CAST_TWILIGHT_PRECISION:
                            DoCast(me, SPELL_TWILIGHT_PRECISION);
                            events.ScheduleEvent(EVENT_CAST_TWILIGHT_PRECISION, 10000,PHASE_ALL);
                            break;
                        case EVENT_CAST_TAIL_LASH:
                            DoCast(me, SPELL_TAIL_LASH);
                            events.ScheduleEvent(EVENT_CAST_TAIL_LASH, urand(10000,15000),PHASE_ALL);
                            break;
                        case EVENT_CAST_DARK_BREATH:
                            DoCast(me, RAID_MODE(SPELL_DARK_BREATH_10_NORMAL, SPELL_DARK_BREATH_25_NORMAL, SPELL_DARK_BREATH_10_HEROIC, SPELL_DARK_BREATH_25_HEROIC));
                            events.ScheduleEvent(EVENT_CAST_DARK_BREATH, urand(20000,25000),PHASE_ALL);break;
                        case EVENT_CAST_CLEAVE:
                            DoCast(me, SPELL_CLEAVE);
                            events.ScheduleEvent(EVENT_CAST_CLEAVE, 15000,PHASE_ALL);
                            break;
                        case EVENT_CAST_DUSK_SHROUD:
                            DoCast(me, SPELL_DUSK_SHROUD);
                            events.ScheduleEvent(EVENT_CAST_DUSK_SHROUD, urand(15000,20000),PHASE_ALL);
                            break;
                        case EVENT_CAST_SOUL_CONSUMPTION:
                            DoCast(me, SPELL_SOUL_CONSUMPTION);
                            events.ScheduleEvent(EVENT_CAST_SOUL_CONSUMPTION, urand(15000,25000),PHASE_ALL);
                            break;
                        case EVENT_CAST_BERSEK:
                            DoCast(me, SPELL_BERSEK);
                            events.ScheduleEvent(EVENT_CAST_BERSEK, 9999999,PHASE_ALL);
                            break;
                        case EVENT_DPS:
                            if (pInstance->GetBossState(DATA_HALION)==SPECIAL)
                                CAST_AI(boss_halion::boss_halionAI, pHalion->AI())->CastCorporeality();
                            events.ScheduleEvent(EVENT_DPS, 5000,PHASE_3);
                            break;
                    }
                }

                if ((events.GetPhaseMask() & PHASE_2_MASK) && ((me->GetHealth()*100) / me->GetMaxHealth() <= 50))
    			{
                    events.SetPhase(PHASE_3);
    				DoScriptText(SAY_PHASE3, me);
                    pHalion->SetReactState(REACT_AGGRESSIVE);
    				pHalion->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pHalion->SetVisibility(VISIBILITY_ON);
                    CAST_AI(boss_halion::boss_halionAI, pHalion->AI())->SetPhase3();
                    events.ScheduleEvent(EVENT_DPS, 5000,PHASE_3);
    			}

                DoMeleeAttackIfReady();
    	    }

            void DamageTaken(Unit *who, uint32 &dmg)
            {
                if (!(events.GetPhaseMask() & PHASE_3_MASK))
                        return;
                TwilightDamage += dmg;	

                pInstance->SetData(DATA_DAMAGE, me->GetHealth() >= dmg ? me->GetHealth() - dmg : 0);
            }

    	    void KilledUnit(Unit *victim)
    	    {
                DoScriptText(RAND(SAY_SLAY1,SAY_SLAY2), me);
    	    }

    	    void JustDied(Unit*)
    	    {
                DoScriptText(SAY_DEATH, me);
                
                pInstance->SetData(DATA_TWILIGHT_HALION,DONE);
    	    }

        private:
            EventMap events;
            InstanceScript* pInstance;
        };

        CreatureAI* GetAI(Creature *pCreature) const
        {
                return new boss_twilight_halionAI(pCreature);
        }

};

class spell_halion_portal : public SpellScriptLoader
{
    public:
        spell_halion_portal() : SpellScriptLoader("spell_halion_portal") { }

        class spell_halion_portal_SpellScript : public SpellScript
        {
        	PrepareSpellScript(spell_halion_portal_SpellScript)
            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                //PreventHitDefaultEffect(EFFECT_0);
                GetHitPlayer()->RemoveAurasDueToSpell(74807);
                GetHitPlayer()->SetPhaseMask(1,true);
            }

            void Register()
            {
                OnEffect += SpellEffectFn(spell_halion_portal_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_halion_portal_SpellScript();
        }
};


void AddSC_boss_halion()
{
    new boss_halion;
    new boss_twilight_halion;
    new spell_halion_portal;
}
