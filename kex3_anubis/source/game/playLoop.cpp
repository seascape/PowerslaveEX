//
// Copyright(C) 2014-2015 Samuel Villarreal
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//      Play loop (in-game) logic
//

#include "kexlib.h"
#include "renderMain.h"
#include "renderView.h"
#include "game.h"

//
// kexPlayLoop::kexPlayLoop
//

kexPlayLoop::kexPlayLoop(void)
{
}

//
// kexPlayLoop::~kexPlayLoop
//

kexPlayLoop::~kexPlayLoop(void)
{
}

//
// kexPlayLoop::Init
//

void kexPlayLoop::Init(void)
{
}

//
// kexPlayLoop::Start
//

void kexPlayLoop::Start(void)
{
    ticks = 0;
    
    if(kexGame::cLocal->Player()->Actor() == NULL)
    {
        kex::cSystem->Warning("No player starts present\n");
        kexGame::cLocal->SetGameState(GS_TITLE);
        return;
    }
    
    renderScene.SetView(&renderView);
    renderScene.SetWorld(kexGame::cLocal->World());

    kexGame::cLocal->Player()->Ready();
}

//
// kexPlayLoop::Stop
//

void kexPlayLoop::Stop(void)
{
    kexGame::cLocal->World()->UnloadMap();
}

//
// kexPlayLoop::Draw
//

void kexPlayLoop::Draw(void)
{
    kexPlayer *p = kexGame::cLocal->Player();
    kexWorld *world = kexGame::cLocal->World();
    
    renderView.SetupFromPlayer(p);
    
    world->FindVisibleSectors(renderView, p->Actor()->Sector());
    
    renderScene.Draw();
    
    if(world->MapLoaded())
    {
        kexCpuVertList *vl = kexRender::cVertList;
        
        if(1)
        {
            static spriteAnim_t *sprTest = NULL;

            if(sprTest == NULL)
            {
                sprTest = kexGame::cLocal->SpriteAnimManager()->Get("objects/camel_idle");
            }

            if(sprTest)
            {
                kexCpuVertList *vl = kexRender::cVertList;
                mapActor_t *a = p->Actor()->MapActor();
                kexVec3 org(a->x, a->y, a->z);
                kexMatrix mtx(renderView.Yaw(), 2);
                kexMatrix mtx2(-renderView.Pitch(), 1);
                kexMatrix mtx3 = mtx2 * mtx;
                kexMatrix scale(1.25f, 1.25f, 1.25f);
                spriteFrame_t *frame;
                spriteSet_t *spriteSet;
                kexSprite *sprite;
                spriteInfo_t *info;

                frame = &sprTest->frames[0];
                org += kexVec3(0, 0, 0);

                mtx3.RotateX(kexMath::pi);

                kexRender::cBackend->SetState(GLSTATE_SCISSOR, false);
                vl->BindDrawPointers();

                for(unsigned int i = 0; i < frame->spriteSet.Length(); ++i)
                {
                    spriteSet = &frame->spriteSet[i];
                    sprite = spriteSet->sprite;
                    info = &sprite->InfoList()[spriteSet->index];

                    float x = (float)spriteSet->x;
                    float y = (float)spriteSet->y;
                    float w = (float)info->atlas.w;
                    float h = (float)info->atlas.h;

                    float u1, u2, v1, v2;
                    
                    u1 = info->u[0 ^ spriteSet->bFlipped];
                    u2 = info->u[1 ^ spriteSet->bFlipped];
                    v1 = info->v[0];
                    v2 = info->v[1];

                    sprite->Texture()->Bind();

                    kexVec3 p1 = kexVec3(x, 0, y);
                    kexVec3 p2 = kexVec3(x+w, 0, y);
                    kexVec3 p3 = kexVec3(x, 0, y+h);
                    kexVec3 p4 = kexVec3(x+w, 0, y+h);

                    p1 *= mtx3;
                    p2 *= mtx3;
                    p3 *= mtx3;
                    p4 *= mtx3;

                    p1 *= scale;
                    p2 *= scale;
                    p3 *= scale;
                    p4 *= scale;

                    p1 += org;
                    p2 += org;
                    p3 += org;
                    p4 += org;

                    vl->AddVertex(p1, u1, v1, 255, 255, 255, 255);
                    vl->AddVertex(p2, u2, v1, 255, 255, 255, 255);
                    vl->AddVertex(p3, u1, v2, 255, 255, 255, 255);
                    vl->AddVertex(p4, u2, v2, 255, 255, 255, 255);

                    vl->AddTriangle(0, 2, 1);
                    vl->AddTriangle(1, 2, 3);

                    vl->DrawElements();
                }
            }
        }

        {
            static spriteAnim_t *anim = p->Weapon().Anim();
            const kexGameLocal::weaponInfo_t *weaponInfo = kexGame::cLocal->WeaponInfo(p->CurrentWeapon());
            
            kexRender::cScreen->SetOrtho();
            kexRender::cBackend->SetState(GLSTATE_DEPTHTEST, false);
            kexRender::cBackend->SetState(GLSTATE_SCISSOR, true);
            kexRender::cBackend->SetBlend(GLSRC_SRC_ALPHA, GLDST_ONE_MINUS_SRC_ALPHA);

            if(anim)
            {
                spriteFrame_t *frame = p->Weapon().Frame();
                spriteSet_t *spriteSet;
                kexSprite *sprite;
                spriteInfo_t *info;

                vl->BindDrawPointers();

                for(unsigned int i = 0; i < frame->spriteSet.Length(); ++i)
                {
                    spriteSet = &frame->spriteSet[i];
                    sprite = spriteSet->sprite;
                    info = &sprite->InfoList()[spriteSet->index];

                    float x = (float)spriteSet->x;
                    float y = (float)spriteSet->y;
                    float w = (float)info->atlas.w;
                    float h = (float)info->atlas.h;
                    word c = 0xff;

                    float u1, u2, v1, v2;
                    
                    u1 = info->u[0 ^ spriteSet->bFlipped];
                    u2 = info->u[1 ^ spriteSet->bFlipped];
                    v1 = info->v[0];
                    v2 = info->v[1];

                    kexRender::cScreen->SetAspectDimentions(x, y, w, h);

                    sprite->Texture()->Bind();

                    x += p->Weapon().BobX() + weaponInfo->offsetX;
                    y += p->Weapon().BobY() + weaponInfo->offsetY;

                    if(!(frame->flags & SFF_FULLBRIGHT))
                    {
                        c = (p->Actor()->Sector()->lightLevel << 1);

                        if(c > 255)
                        {
                            c = 255;
                        }
                    }

                    vl->AddQuad(x, y + 8, 0, w, h, u1, v1, u2, v2, (byte)c, (byte)c, (byte)c, 255);
                    vl->DrawElements();
                }
            }
        }
        
        kexRender::cBackend->SetState(GLSTATE_SCISSOR, false);
        vl->BindDrawPointers();
        
        kexTexture *gfx = kexRender::cTextures->Cache("gfx/hud.png", TC_CLAMP, TF_NEAREST);
        gfx->Bind();

        vl->AddQuad(0, 192, 0, 64, 64, 0, 0, 0.25f, 1, 255, 255, 255, 255);
        vl->AddQuad(64, 216, 0, 96, 24, 0.25f, 0, 0.625f, 0.375f, 255, 255, 255, 255);
        vl->AddQuad(160, 216, 0, 96, 24, 0.25f, 0.375f, 0.625f, 0.75f, 255, 255, 255, 255);
        vl->AddQuad(256, 192, 0, 64, 64, 0.625f, 0, 0.875f, 1, 255, 255, 255, 255);
        vl->DrawElements();
    }
}

//
// kexPlayLoop::Tick
//

void kexPlayLoop::Tick(void)
{
    if(ticks > 4)
    {
        kexGame::cLocal->UpdateActors();
        kexGame::cLocal->Player()->Tick();
    }
    
    ticks++;
}

//
// kexPlayLoop::ProcessInput
//

bool kexPlayLoop::ProcessInput(inputEvent_t *ev)
{
    return false;
}
