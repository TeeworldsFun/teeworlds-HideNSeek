#ifndef GAME_SERVER_ENTITIES_WALL_H
#define GAME_SERVER_ENTITIES_WALL_H

#include <game/server/entity.h>

class CWall : public CEntity
{
public:
	CWall(CGameWorld *pGameWorld, vec2 Pos, vec2 Pos2, int Owner);
	
	void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);
	vec2 m_StartPos;
	vec2 m_EndPos;	
	
	inline int GetOwner(){return m_Owner;};
	
protected:
	void HitCharacter(vec2 From, vec2 To);
	
private:	
	int m_Owner;
	int m_ID2;
};

#endif
