/*
 * Copyright (C) 2008-2010 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
//Basic headers
#include "WaypointMovementGenerator.h"
#include "DestinationHolderImp.h"
//Extended headers
#include "ObjectMgr.h"
#include "World.h"
#include "MapManager.h" // for flightmaster grid preloading
//Creature-specific headers
#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureGroups.h"
//Player-specific
#include "Player.h"

template<class T>
void
WaypointMovementGenerator<T>::Initialize(T & /*u*/){}

template<>
void
WaypointMovementGenerator<Creature>::Finalize(Creature & /*u*/){}

template<>
void
WaypointMovementGenerator<Player>::Finalize(Player & /*u*/){}

template<class T>
void
WaypointMovementGenerator<T>::MovementInform(T & /*unit*/){}

template<>
void WaypointMovementGenerator<Creature>::MovementInform(Creature &unit)
{
    unit.AI()->MovementInform(WAYPOINT_MOTION_TYPE, i_currentNode);
}

template<>
bool WaypointMovementGenerator<Creature>::GetDestination(float &x, float &y, float &z) const
{
    if (i_destinationHolder.HasArrived())
        return false;

    i_destinationHolder.GetDestination(x, y, z);
    return true;
}

template<>
bool WaypointMovementGenerator<Player>::GetDestination(float & /*x*/, float & /*y*/, float & /*z*/) const
{
    return false;
}

template<>
void WaypointMovementGenerator<Creature>::Reset(Creature & /*unit*/)
{
    StopedByPlayer = true;
    i_nextMoveTime.Reset(0);
}

template<>
void WaypointMovementGenerator<Player>::Reset(Player & /*unit*/){}

template<>
void WaypointMovementGenerator<Creature>::InitTraveller(Creature &unit, const WaypointData &node)
{
    node.run ? unit.RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING):
        unit.AddUnitMovementFlag(MOVEMENTFLAG_WALKING);

    unit.SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
    unit.SetUInt32Value(UNIT_FIELD_BYTES_1, 0);

    // TODO: make this part of waypoint node, so that creature can walk when desired?
    if (unit.canFly())
        unit.SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);

    unit.addUnitState(UNIT_STAT_ROAMING);
}

template<>
void
WaypointMovementGenerator<Creature>::Initialize(Creature &u)
{
    u.StopMoving();
    //i_currentNode = -1; // uint32, become 0 in the first update
    //i_nextMoveTime.Reset(0);
    StopedByPlayer = false;
    if (!path_id)
        path_id = u.GetWaypointPath();
    waypoints = sWaypointMgr->GetPath(path_id);
    i_currentNode = 0;
    if (waypoints && waypoints->size())
    {
        node = waypoints->front();
        Traveller<Creature> traveller(u);
        InitTraveller(u, *node);
        i_destinationHolder.SetDestination(traveller, node->x, node->y, node->z);
        i_nextMoveTime.Reset(i_destinationHolder.GetTotalTravelTime());

        //Call for creature group update
        if (u.GetFormation() && u.GetFormation()->getLeader() == &u)
            u.GetFormation()->LeaderMoveTo(node->x, node->y, node->z);
    }
    else
        node = NULL;
}

template<>
void WaypointMovementGenerator<Player>::InitTraveller(Player & /*unit*/, const WaypointData & /*node*/){}

template<class T>
bool
WaypointMovementGenerator<T>::Update(T & /*unit*/, const uint32 & /*diff*/)
{
    return false;
}

template<>
bool
WaypointMovementGenerator<Creature>::Update(Creature &unit, const uint32 &diff)
{
    if (!&unit)
        return true;

    if (!path_id)
        return false;

    // Waypoint movement can be switched on/off
    // This is quite handy for escort quests and other stuff
    if (unit.hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_STUNNED | UNIT_STAT_DISTRACTED))
        return true;

    // Clear the generator if the path doesn't exist
    if (!waypoints || !waypoints->size())
        return false;

    // We have to set the destination here (for the first point), right after Initialize. Without, we may not have valid xyz for GetResetPosition
    Traveller<Creature> traveller(unit);
    MoveToNextNode(traveller);

    if (i_nextMoveTime.GetExpiry() < TIMEDIFF_NEXT_WP)
    {
        if (unit.IsStopped())
        {
            if (StopedByPlayer)
            {
                ASSERT(node);
                InitTraveller(unit, *node);
                MoveToNextNode(traveller);
                StopedByPlayer = false;
                return true;
            }

            if (i_currentNode == waypoints->size() - 1) // If that's our last waypoint
            {
                if (repeating)         // If the movement is repeating
                    i_currentNode = 0; // Start moving all over again
                else
                {
                    unit.SetHomePosition(node->x, node->y, node->z, unit.GetOrientation());
                    unit.GetMotionMaster()->Initialize();
                    return false; // Clear the waypoint movement
                }
            }
            else
                ++i_currentNode;

            node = waypoints->at(i_currentNode);
            InitTraveller(unit, *node);
            MoveToNextNode(traveller);

            //Call for creature group update
            if (unit.GetFormation() && unit.GetFormation()->getLeader() == &unit)
                unit.GetFormation()->LeaderMoveTo(node->x, node->y, node->z);
        }
        else
        {
            //Determine waittime
            if (node->delay)
                i_nextMoveTime.Reset(node->delay);

            //note: disable "start" for mtmap
            if (node->event_id && urand(0,99) < node->event_chance)
                unit.GetMap()->ScriptsStart(sWaypointScripts, node->event_id, &unit, NULL/*, false*/);

            i_destinationHolder.ResetTravelTime();
            MovementInform(unit);
            unit.UpdateWaypointID(i_currentNode);
            unit.clearUnitState(UNIT_STAT_ROAMING);
            unit.Relocate(node->x, node->y, node->z);
        }
    }
    else
    {
        if (unit.IsStopped() && !i_destinationHolder.HasArrived())
        {
            if (!StopedByPlayer)
            {
                i_destinationHolder.IncreaseTravelTime(STOP_TIME_FOR_PLAYER);
                i_nextMoveTime.Reset(STOP_TIME_FOR_PLAYER);
                StopedByPlayer = true;
            }
        }
    }
    return true;
}

template<>
void
WaypointMovementGenerator<Creature>::MoveToNextNode(CreatureTraveller &traveller)
{
    Creature* owner = &(traveller.i_traveller);
    i_destinationHolder.SetDestination(traveller, node->x, node->y, node->z, false);

    PathInfo sub_path(owner, node->x, node->y, node->z);
    PointPath pointPath = sub_path.getFullPath();

    float speed = traveller.Speed()*0.001f; // in ms
    uint32 traveltime = uint32(pointPath.GetTotalLength()/speed);
    owner->SendMonsterMoveByPath(pointPath, 1, pointPath.size(), traveltime);

    i_nextMoveTime.Reset(traveltime);
}

template void WaypointMovementGenerator<Player>::Initialize(Player &);
template bool WaypointMovementGenerator<Player>::Update(Player &, const uint32 &);
template void WaypointMovementGenerator<Player>::MovementInform(Player &);

//----------------------------------------------------//

uint32 FlightPathMovementGenerator::GetPathAtMapEnd() const
{
    if (i_currentNode >= i_path->size())
        return i_path->size();

    uint32 curMapId = (*i_path)[i_currentNode].mapid;
    for (uint32 i = i_currentNode; i < i_path->size(); ++i)
    {
        if ((*i_path)[i].mapid != curMapId)
            return i;
    }

    return i_path->size();
}

void FlightPathMovementGenerator::Initialize(Player &player)
{
    player.getHostileRefManager().setOnlineOfflineState(false);
    player.addUnitState(UNIT_STAT_IN_FLIGHT);
    player.SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_TAXI_FLIGHT);
    Traveller<Player> traveller(player);
    // do not send movement, it was sent already
    i_destinationHolder.SetDestination(traveller, (*i_path)[i_currentNode].x, (*i_path)[i_currentNode].y, (*i_path)[i_currentNode].z, false);

    // For preloading end grid
    InitEndGridInfo();
    TaxiPathNodeList path = GetPath();
    uint32 pathEndPoint = GetPathAtMapEnd();
    uint32 traveltime = uint32(32.0f*path.GetTotalLength(GetCurrentNode(),pathEndPoint));
    player.SendMonsterMoveByPath(path,GetCurrentNode(),pathEndPoint, traveltime);
}

void FlightPathMovementGenerator::Finalize(Player & player)
{
    player.clearUnitState(UNIT_STAT_IN_FLIGHT);

    float x = 0;
    float y = 0;
    float z = 0;
    i_destinationHolder.GetLocationNow(player.GetBaseMap(), x, y, z);
    player.SetPosition(x, y, z, player.GetOrientation());

}

bool FlightPathMovementGenerator::Update(Player &player, const uint32 &diff)
{
    if (MovementInProgress())
    {
        Traveller<Player> traveller(player);
        if (i_destinationHolder.UpdateTraveller(traveller, diff))
        {
            i_destinationHolder.ResetUpdate(FLIGHT_TRAVEL_UPDATE);
            if (i_destinationHolder.HasArrived())
            {
                DoEventIfAny(player,(*i_path)[i_currentNode], false);

                uint32 curMap = (*i_path)[i_currentNode].mapid;
                ++i_currentNode;
                if (MovementInProgress())
                {
                    DoEventIfAny(player,(*i_path)[i_currentNode], true);

                    sLog.outStaticDebug("loading node %u for player %s", i_currentNode, player.GetName());
                    if ((*i_path)[i_currentNode].mapid == curMap)
                    {
                        // do not send movement, it was sent already
                        i_destinationHolder.SetDestination(traveller, (*i_path)[i_currentNode].x, (*i_path)[i_currentNode].y, (*i_path)[i_currentNode].z, false);
                    }

                     // check if it's time to preload the flightmaster grid at path end
                    if (i_currentNode == m_preloadTargetNode)
                        PreloadEndGrid();

                    return true;
                }
                //else HasArrived()
            }
            else
                return true;
        }
        else
            return true;
    }

    // we have arrived at the end of the path
    return false;
}

void FlightPathMovementGenerator::SetCurrentNodeAfterTeleport()
{
    if (i_path->empty())
        return;

    uint32 map0 = (*i_path)[0].mapid;
    for (size_t i = 1; i < i_path->size(); ++i)
    {
        if ((*i_path)[i].mapid != map0)
        {
            i_currentNode = i;
            return;
        }
    }
}

void FlightPathMovementGenerator::InitEndGridInfo()
{
    // Storage to preload flightmaster grid at end of flight. For multi-stop flights, this will
    // be reinitialized for each flightmaster at the end of each spline (or stop) in the flight.

    uint32 nodeCount = (*i_path).size();        // Get the number of nodes in the path.
    m_endMapId = (*i_path)[nodeCount -1].mapid; // Get the map ID from the last node
    m_preloadTargetNode = nodeCount - 3;        // 2 nodes before the final node, we pre-load the grid
    m_endGridX = (*i_path)[nodeCount -1].x;     // Get the X position from the last node
    m_endGridY = (*i_path)[nodeCount -1].y;     // Get the Y position from the last node
}

void FlightPathMovementGenerator::PreloadEndGrid()
{
    // used to preload the final grid where the flightmaster is
    Map *endMap = sMapMgr.FindMap(m_endMapId);

    // Load the grid
    if (endMap)
    {
        sLog.outDetail("Preloading flightmaster at grid (%f, %f) for map %u", m_endGridX, m_endGridY, m_endMapId);
        endMap->LoadGrid(m_endGridX, m_endGridY);
    }
    else
        sLog.outDetail("Unable to determine map to preload flightmaster grid");
}

void FlightPathMovementGenerator::DoEventIfAny(Player& player, TaxiPathNodeEntry const& node, bool departure)
{
    if (uint32 eventid = departure ? node.departureEventID : node.arrivalEventID)
    {
        sLog.outDebug("Taxi %s event %u of node %u of path %u for player %s", departure ? "departure" : "arrival", eventid, node.index, node.path, player.GetName());
        player.GetMap()->ScriptsStart(sEventScripts, eventid, &player, &player);
    }
}



