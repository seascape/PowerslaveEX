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
//      Menu logic
//

#include "kexlib.h"
#include "game.h"
#include "renderMain.h"
#include "menu.h"

//-----------------------------------------------------------------------------
//
// kexMenuItem
//
//-----------------------------------------------------------------------------

DECLARE_KEX_CLASS(kexMenuItem, kexObject)

//
// kexMenuItem::kexMenuItem
//

kexMenuItem::kexMenuItem(void)
{
    this->x             = 0;
    this->y             = 0;
    this->scale         = 1;
    this->bLerping      = false;
    this->bHighLighted  = false;
    this->bInteract     = true;
    this->bDisabled     = false;
    this->destX         = 0;
    this->destY         = 0;
    this->time          = 0;
    this->highlightTime = 0;
    this->lerpCallback  = NULL;
}

//
// kexMenuItem::kexMenuItem
//

kexMenuItem::kexMenuItem(const char *label, const float x, const float y,
                         const float scale, menuItemLerpDone_t callback)
{
    this->x             = x;
    this->y             = y;
    this->scale         = scale;
    this->bLerping      = false;
    this->bInteract     = true;
    this->destX         = 0;
    this->destY         = 0;
    this->time          = 0;
    this->label         = label;
    this->highlightTime = 0;
    this->lerpCallback  = callback;
}

//
// kexMenuItem::~kexMenuItem
//

kexMenuItem::~kexMenuItem(void)
{
}

//
// kexMenuItem::LerpTo
//

void kexMenuItem::LerpTo(const float destx, const float desty)
{
    this->bLerping = true;
    this->startX = x;
    this->startY = y;
    this->destX = destx;
    this->destY = desty;
    this->time = 1;
    this->bInteract = false;
    this->bHighLighted = false;
    this->highlightTime = 0;
}

//
// kexMenuItem::LerpTo
//

void kexMenuItem::LerpTo(const float destx)
{
    LerpTo(destx, y);
}

//
// kexMenuItem::Move
//

void kexMenuItem::Move(void)
{
    float c = 1.0f - time;

    x = c * (destX - startX) + startX;
    y = c * (destY - startY) + startY;

    time -= 0.03f;

    if(time <= 0)
    {
        bLerping = false;
        bInteract = true;
        x = destX;
        y = destY;

        if(lerpCallback)
        {
            lerpCallback(this);
        }
    }
}

//
// kexMenuItem::OnCursor
//

bool kexMenuItem::OnCursor(void)
{
    float width = kexGame::cLocal->BigFont()->StringWidth(label.c_str(), scale, 0) * 0.5f;
    float height = kexGame::cLocal->BigFont()->StringHeight(label.c_str(), scale, 0) * 0.5f;
    float sy = y + height;
    float mx = (float)kex::cInput->MouseX();
    float my = (float)kex::cInput->MouseY();

    kexRender::cScreen->CoordsToRenderScreenCoords(mx, my);

    return !(mx < (x - width) || my < (sy - height) ||
             mx > (x + width) || my > (sy + height));
}

//
// kexMenuItem::Tick
//

void kexMenuItem::Tick(void)
{
    static kexMenuItem *lastItem;

    if(bLerping)
    {
        Move();
    }

    if(bInteract && !bSelected && !bDisabled)
    {
        if(!(bHighLighted = OnCursor()))
        {
            highlightTime = 0;

            if(lastItem == this)
            {
                lastItem = NULL;
            }
        }
        else
        {
            highlightTime += 1.0f;
        }
    }

    if(bHighLighted && lastItem != this)
    {
        lastItem = this;
        kexGame::cLocal->PlaySound("sounds/menu_highlight.wav");
    }
}

//
// kexMenuItem::DrawSmallString
//

void kexMenuItem::DrawSmallString(const char *string, float x, float y, float scale, bool center, bool flash)
{
    byte c = (flash || bSelected || bDisabled) ? 255 : 224;
    kexFont *font = kexGame::cLocal->SmallFont();

    kexGame::cLocal->DrawSmallString(string, x, y, scale, center, c, c, c);

    if(flash)
    {
        byte pulse = (byte)(kexMath::Sin(highlightTime * 0.1f) * 64.0f) + 64;

        kexRender::cBackend->SetBlend(GLSRC_ONE, GLDST_ONE);
        font->DrawString(string, x, y, scale, center, RGBA(pulse, pulse, pulse, 0xff));
    }
}

//
// kexMenuItem::DrawBigString
//

void kexMenuItem::DrawBigString(const char *string, float x, float y, float scale, bool center, bool flash)
{
    byte c = (flash || bSelected || bDisabled) ? 255 : 224;
    kexFont *font = kexGame::cLocal->BigFont();

    kexGame::cLocal->DrawBigString(string, x, y, scale, center, c, c, c);

    if(flash)
    {
        byte pulse = (byte)(kexMath::Sin(highlightTime * 0.1f) * 64.0f) + 64;

        kexRender::cBackend->SetBlend(GLSRC_ONE, GLDST_ONE);
        font->DrawString(string, x, y, scale, center, RGBA(pulse, pulse, pulse, 0xff));
    }
}

//
// kexMenuItem::Draw
//

void kexMenuItem::Draw(void)
{
    DrawBigString(label.c_str(), x, y, scale, true, bHighLighted);
}

//
// kexMenuItem::operator=
//

kexMenuItem &kexMenuItem::operator=(const kexMenuItem &item)
{
    this->x             = item.x;
    this->y             = item.y;
    this->scale         = item.scale;
    this->label         = item.label;
    this->bInteract     = item.bInteract;
    this->bLerping      = item.bLerping;

    return *this;
}

//-----------------------------------------------------------------------------
//
// kexMenuItemLabel
//
//-----------------------------------------------------------------------------

DECLARE_KEX_CLASS(kexMenuItemLabel, kexMenuItem)

//
// kexMenuItemLabel::kexMenuItemLabel
//

kexMenuItemLabel::kexMenuItemLabel(void)
{
    this->x             = 0;
    this->y             = 0;
    this->scale         = 1;
    this->bLerping      = false;
    this->bHighLighted  = false;
    this->bInteract     = false;
    this->bDisabled     = false;
    this->destX         = 0;
    this->destY         = 0;
    this->time          = 0;
    this->highlightTime = 0;
    this->lerpCallback  = NULL;
}

//
// kexMenuItemLabel::kexMenuItemLabel
//

kexMenuItemLabel::kexMenuItemLabel(const char *label, const float x, const float y, const float scale)
{
    this->x             = x;
    this->y             = y;
    this->scale         = scale;
    this->bLerping      = false;
    this->bInteract     = false;
    this->destX         = 0;
    this->destY         = 0;
    this->time          = 0;
    this->label         = label;
    this->highlightTime = 0;
    this->lerpCallback  = NULL;
}

//
// kexMenuItemLabel::~kexMenuItemLabel
//

kexMenuItemLabel::~kexMenuItemLabel(void)
{
}

//
// kexMenuItemLabel::Tick
//

void kexMenuItemLabel::Tick(void)
{
    if(bLerping)
    {
        Move();
    }
}

//
// kexMenuItemLabel::Draw
//

void kexMenuItemLabel::Draw(void)
{
    DrawBigString(label.c_str(), x, y, scale, true, false);
}

//-----------------------------------------------------------------------------
//
// kexMenuItemSlider
//
//-----------------------------------------------------------------------------

DECLARE_KEX_CLASS(kexMenuItemSlider, kexMenuItem)
#define MAX_MENU_SLIDER_BARS    24

//
// kexMenuItemSlider::kexMenuItemSlider
//

kexMenuItemSlider::kexMenuItemSlider(void)
{
    this->x             = 0;
    this->y             = 0;
    this->scale         = 1;
    this->bLerping      = false;
    this->bHighLighted  = false;
    this->bInteract     = true;
    this->bDisabled     = false;
    this->destX         = 0;
    this->destY         = 0;
    this->time          = 0;
    this->highlightTime = 0;
    this->lerpCallback  = NULL;
}

//
// kexMenuItemSlider::kexMenuItemSlider
//

kexMenuItemSlider::kexMenuItemSlider(const float x, const float y, const float scale, kexCvar &cvar)
{
    this->x             = x;
    this->y             = y;
    this->scale         = scale;
    this->bLerping      = false;
    this->bInteract     = true;
    this->destX         = 0;
    this->destY         = 0;
    this->time          = 0;
    this->label         = "";
    this->highlightTime = 0;
    this->numBars       = 0;
    this->lerpCallback  = NULL;
    this->cvar          = &cvar;
}

//
// kexMenuItemSlider::~kexMenuItemSlider
//

kexMenuItemSlider::~kexMenuItemSlider(void)
{
}

//
// kexMenuItemSlider::Select
//

void kexMenuItemSlider::Select(const bool b)
{
    kexMenuItem::Select(b);
}

//
// kexMenuItemSlider::OnCursor
//

bool kexMenuItemSlider::OnCursor(void)
{
    float width = 2.5f * MAX_MENU_SLIDER_BARS;
    float height = kexGame::cLocal->BigFont()->StringHeight(label.c_str(), scale, 0) * 0.5f;
    float sy = y + height;
    float mx = (float)kex::cInput->MouseX();
    float my = (float)kex::cInput->MouseY();
    
    kexRender::cScreen->CoordsToRenderScreenCoords(mx, my);
    
    return !(mx < (x - width) || my < (sy - height) ||
             mx > (x + width) || my > (sy + height));
}

//
// kexMenuItemSlider::Tick
//

void kexMenuItemSlider::Tick(void)
{
    int bars = 0;
    
    kexMenuItem::Tick();
    
    if(bSelected)
    {
        float width = (kexGame::cLocal->BigFont()->StringWidth(label.c_str(), scale, 0) * 0.5f) * 1.25f;
        float mx = (float)kex::cInput->MouseX();
        float my = (float)kex::cInput->MouseY();
        float val;
        
        kexRender::cScreen->CoordsToRenderScreenCoords(mx, my);
        
        val = ((mx-x)/width) * 0.5f + 0.5f;
        kexMath::Clamp(val, 0, 1);
        
        cvar->Set(cvar->GetMax() * val);
        
        bars = (int)kexMath::Floor(MAX_MENU_SLIDER_BARS * val);
        kexMath::Clamp(bars, 0, MAX_MENU_SLIDER_BARS);

        if(numBars != bars)
        {
            kexGame::cLocal->PlaySound("sounds/select.wav");
        }

        numBars = bars;
    }
    else
    {
        bars = (int)kexMath::Floor(MAX_MENU_SLIDER_BARS * (cvar->GetFloat() / cvar->GetMax()));
    }
    
    label.Clear();
    
    for(int i = 0; i < bars; ++i)
    {
        label += "/";
    }
    for(int i = bars; i < MAX_MENU_SLIDER_BARS; ++i)
    {
        label += ".";
    }
}

//
// kexMenuItemSlider::Draw
//

void kexMenuItemSlider::Draw(void)
{
    DrawBigString(label, x-8, y, scale, true, bHighLighted);
}
