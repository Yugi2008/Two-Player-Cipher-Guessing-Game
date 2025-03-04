/*
 * Graphics.h
 *
 */

#ifndef HAL_GRAPHICS_H_
#define HAL_GRAPHICS_H_

#include <HAL/LcdDriver/Crystalfontz128x128_ST7735.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>


#define FG_COLOR GRAPHICS_COLOR_WHITE
#define BG_COLOR GRAPHICS_COLOR_BLACK


struct _GFX
{
    Graphics_Context context;
    uint32_t foreground;
    uint32_t background;
    uint32_t defaultForeground;
    uint32_t defaultBackground;
};
typedef struct _GFX GFX;

void GFX_construct(Graphics_Context *gfxContext, uint32_t defaultForeground, uint32_t defaultBackground);

void GFX_clear(GFX* gfx_p);

void GFX_print(GFX* gfx_p, char* string, int row, int col);




#endif /* HAL_GRAPHICS_H_ */
