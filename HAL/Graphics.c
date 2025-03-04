/*
 * Graphics.c
 */

#include <HAL/Graphics.h>

void GFX_construct(Graphics_Context *gfxContext, uint32_t defaultForeground, uint32_t defaultBackground)
{
    // initializing the display
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    // setting up the graphics
    Graphics_initContext(gfxContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setFont(gfxContext, &g_sFontFixed6x8);

    Graphics_setForegroundColor(gfxContext, defaultForeground);
    Graphics_setBackgroundColor(gfxContext, defaultBackground);
    Graphics_clearDisplay(gfxContext);


}
void GFX_print(GFX* gfx_p, char* string, int row, int col)
{
    int yPosition = row * Graphics_getFontHeight(gfx_p->context.font);
    int xPosition = col * Graphics_getFontMaxWidth(gfx_p->context.font);

    Graphics_drawString(&gfx_p->context, (int8_t*) string, -1, xPosition, yPosition, OPAQUE_TEXT);
}
void GFX_clear(GFX* gfx_p)
{
    Graphics_clearDisplay(&gfx_p->context);
}
