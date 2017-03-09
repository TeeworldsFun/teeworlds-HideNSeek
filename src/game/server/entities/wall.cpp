#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "wall.h"
#include <engine/shared/config.h>
CWall::CWall(CGameWorld *pGameWorld, vec2 Pos, vec2 Pos2, int Owner)
: CEntity(pGameWorld, NETOBJTYPE_LASER)
{
	m_StartPos = Pos;
	m_EndPos = Pos2;
	m_Owner = Owner;
	this->m_ID2 = Server()->SnapNewID();
	GameServer()->m_pController->wallreset = 0;
	GameWorld()->InsertEntity(this);
}

void CWall::HitCharacter(vec2 From, vec2 To)
{
	vec2 At;
	CCharacter *Hit[MAX_CLIENTS] = {0};

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);
		Hit[i] = GameServer()->m_World.LaserIntersectCharacter(From, To, 2.5f, At, Hit);
		if(Hit[i])
		{
			
			if(Hit[i]->m_ActiveWeapon == WEAPON_GUN && OwnerChar == Hit[i]) {
				Reset();
				Hit[i]->m_wallnum = Hit[i]->m_wallnum - 1;
			} 
			if(GameServer()->m_pController->IsTeamplay()) {
				if(OwnerChar->GetPlayer()->GetTeam() != Hit[i]->GetPlayer()->GetTeam()) {
					Hit[i]->m_HittingWall = true;
					Hit[i]->m_PushDirection = normalize(Hit[i]->m_OldPos - At);
				}
			} else if(OwnerChar != Hit[i]) {
				Hit[i]->m_HittingWall = true;
				Hit[i]->m_PushDirection = normalize(Hit[i]->m_OldPos - At); //stolen from XXLDDRace :p
			}		
		}
	}
}

void CWall::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}

void CWall::Tick(){	
	CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);
	if(!OwnerChar || !OwnerChar->IsAlive() || GameServer()->m_apPlayers[m_Owner]->m_Animal != 0) 
		Reset();
	for(int i = 0; i < MAX_CLIENTS; i++) {
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if(!Char)
			continue;
		if(GameServer()->m_pController->wallreset == 1) {
			Char->m_wallnum = Char->m_wallnum - 1;
			GameServer()->m_World.DestroyEntity(this);
		}
	}	
	HitCharacter(m_EndPos, m_StartPos);		
}

void CWall::Snap(int SnappingClient)
{
	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
	pObj->m_X = (int)m_StartPos.x;
	pObj->m_Y = (int)m_StartPos.y;
	pObj->m_FromX = (int)m_EndPos.x;
	pObj->m_FromY = (int)m_EndPos.y;
	pObj->m_StartTick = Server()->Tick();
	
	CNetObj_Laser *pObj2 = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID2, sizeof(CNetObj_Laser)));
	pObj2->m_X = (int)m_EndPos.x;
	pObj2->m_Y = (int)m_EndPos.y;
	pObj2->m_FromX = (int)m_EndPos.x;
	pObj2->m_FromY = (int)m_EndPos.y;
	pObj2->m_StartTick = Server()->Tick();	
}
