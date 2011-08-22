#include "GameObjPlayer.h"
#include "GameObjZombie.h"
#include "GameObjRocket.h"
#include "ServerClient.h" // for SERVER_UPDATETIME in gun sound spam prevention

#ifdef _DEBUG
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

CGameObjPlayer::CGameObjPlayer(CWorld* world) : CGameObj(world)
{
    m_prim_triggered = 0;
    m_prim_triggered_time = 0;
    m_fire_sound = 0;
}

CGameObjPlayer::~CGameObjPlayer(void)
{
}

void CGameObjPlayer::CmdFire(bool active)
{
    if(!m_prim_triggered && active)
        OnCmdFire();
    m_prim_triggered = active;
}

#define PLAYER_GUN_FIRESPEED            400
#define PLAYER_GUN_DAMAGE               28

void CGameObjPlayer::OnCmdFire()
{
#if 0
    vec3_t dir;
    GetLookDir().GetVec3(&dir, NULL, NULL);
    dir = -dir;
    bsp_sphere_trace_t trace;
    trace.dir = dir;
    trace.start = GetOrigin();
    trace.dir = dir * 800.0f;
    trace.radius = 0.02f;
    GetWorld()->GetBSP()->TraceSphere(&trace);
    if(trace.f < 1.0f)
    {
        vec3_t hitpoint = trace.start + trace.f * trace.dir;
        SpawnParticleDust(hitpoint, trace.p.m_n);
    }
    return;
#endif

    if(GetWorld()->GetLeveltime() - m_prim_triggered_time < PLAYER_GUN_FIRESPEED)
        return;
    m_prim_triggered_time = GetWorld()->GetLeveltime();

    FireGun();
}

void CGameObjPlayer::FireGun()
{
    // we have to prevent sound spamming from the machine gun
    // remember the sound obj and only play a new sound if the old one
    // is deleted
    // THE SOUND SYSTEM SUCKS ATM FIXME
    //if(GetWorld()->GetObj(m_fire_sound) == NULL)
    //{
    //    m_fire_sound = PlaySound(GetOrigin(),
    //                             CLynx::GetBaseDirSound() + "rifle.ogg",
    //                             PLAYER_GUN_FIRESPEED);
    //}

    world_obj_trace_t trace;
    vec3_t dir;
    GetLookDir().GetVec3(&dir, NULL, NULL);
    dir = -dir;
    trace.dir = dir;
    trace.start = GetOrigin();
    trace.excludeobj = GetID();
    if(GetWorld()->TraceObj(&trace))
    {
        CGameObj* hitobj = (CGameObj*)GetWorld()->GetObj(trace.objid);
        assert(hitobj);
        if(hitobj)
            hitobj->DealDamage(PLAYER_GUN_DAMAGE, trace.hitpoint, trace.dir, this);
    }
    else
    {
        // Kein Objekt getroffen, mit Levelgeometrie testen
        bsp_sphere_trace_t spheretrace;
        spheretrace.start = trace.start;
        spheretrace.dir = dir * 800.0f;
        spheretrace.radius = 0.01f;
        GetWorld()->GetBSP()->TraceSphere(&spheretrace);
        if(spheretrace.f < 1.0f)
        {
            vec3_t hitpoint = spheretrace.start + spheretrace.f * spheretrace.dir;
            SpawnParticleDust(hitpoint, spheretrace.p.m_n);
        }
    }
}

void CGameObjPlayer::FireRocket()
{
    // we have to prevent sound spamming from the machine gun
    // remember the sound obj and only play a new sound if the old one
    // is deleted
    if(GetWorld()->GetObj(m_fire_sound) == NULL)
    {
        m_fire_sound = PlaySound(GetOrigin(),
                                 CLynx::GetBaseDirSound() + "rifle.ogg",
                                 PLAYER_GUN_FIRESPEED);
    }

    vec3_t dir;
    GetLookDir().GetVec3(&dir, NULL, NULL);
    dir = -dir;

    CGameObjRocket* rocket = new CGameObjRocket(GetWorld());
    rocket->SetVel(dir*rocket->GetRocketSpeed());
    rocket->SetOrigin(this->GetOrigin() + dir*this->GetRadius());
    rocket->SetRot(GetLookDir());
    GetWorld()->AddObj(rocket);
}

void CGameObjPlayer::DealDamage(int damage, const vec3_t& hitpoint, const vec3_t& dir, CGameObj* dealer)
{
    CGameObj::DealDamage(0, hitpoint, dir, dealer);

    SpawnParticleBlood(hitpoint, dir);
}
