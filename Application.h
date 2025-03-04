/*
 * Application.h
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <HAL/HAL.h>

typedef enum {title, askCipher, confirm, askEncrypt, confirm2, guessCipher, confirmGC, checkAnswer} _appScreenShow;
typedef enum {OFF, RED, MAGENTA, WHITE, CYAN, GREEN} Colors;
struct _Application
{
    // Put your application members and FSM state variables here!
    // =========================================================================
    UART_Baudrate baudChoice;
    _appScreenShow screenShow;
    Colors color;
    bool firstCall;
};
typedef struct _Application Application;

// Called only a single time - inside of main(), where the application is constructed
Application Application_construct();

// Called once per super-loop of the main application.
void Application_loop(Application* app, HAL* hal);

// Called only a single time - inside of Application_loop(), but only when
// the firstCall variable is true.
void Application_firstTimeSetup(Application* app_p, HAL* hal_p);

// Called whenever the UART module needs to be updated
void Application_updateCommunications(Application* app, HAL* hal);

// Interprets an incoming character and echoes back to terminal what kind of
// character was received (number, letter, or other)
char Application_interpretIncomingChar(char);
// Screens
void Application_askEncryptScreen(GFX* gfxContext, char text[]);
void Application_titlesScreenText(GFX* gfxContext);
void Application_guessCipherScreen(GFX* gfxContext, HAL* hal, char gnum[], int nguesses, char etext[], char dString[], char num[], char text[]);
void Application_askCipherScreen(GFX* gfxContext, HAL* hal, char num[]);
void Application_wonScreen(GFX* gfxContext, HAL* hal, char gnum[], int nguesses, char etext[], char text[]);
void checkAnswerFunction(_appScreenShow state, _appScreenShow states, HAL* hal_p, char num[], char gnum[], int nguesses, char etext[], char text[],  char dString[], int gccount);
void checkColors(Colors states, HAL* hal_p, int check);
// Generic circular increment function
uint32_t CircularIncrement(uint32_t value, uint32_t maximum);

#endif /* APPLICATION_H_ */
