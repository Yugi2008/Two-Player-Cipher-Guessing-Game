/* DriverLib Includes */
 // Antonio Dominguez
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ti/grlib/grlib.h> //added


/* HAL and Application includes */
#include <Application.h>
#include <HAL/HAL.h>
#include <HAL/Timer.h>

#define BUFFER_SIZE 100

//declaring functions
_appScreenShow switches(Application* app_p, _appScreenShow state, HAL* hal_p, char rxchar, char txchar);
void checkColors(Colors states, HAL* hal_p,int check);
void Application_askEncryptScreen(GFX* gfxContext, char text[]);
void Application_titlesScreenText(GFX* gfxContext);
void Application_guessCipherScreen(GFX* gfxContext, HAL* hal, char gnum[], int nguesses, char etext[], char dString[], char num[], char text[]);
void Application_askCipherScreen(GFX* gfxContext, HAL* hal, char num[]);
void Application_wonScreen(GFX* gfxContext, HAL* hal, char gnum[], int nguesses, char etext[], char text[]);

// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void InitNonBlockingLED()
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}

// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void PollNonBlockingLED()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == 0) {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}

/**
 * The main function should immediately
 * stop the Watchdog timer, call the Application constructor, and then
 * repeatedly call the main super-loop function. The Application constructor
 * should be responsible for initializing all hardware components as well as all
 * other finite state machines you choose to use in this project.
 *
 * THIS FUNCTION IS ALREADY COMPLETE. Unless you want to temporarily experiment
 * with some behavior of a code snippet you may have, we DO NOT RECOMMEND
 * modifying this function in any way.
 */
int main(void)
{
    // Stop Watchdog Timer - THIS SHOULD ALWAYS BE THE FIRST LINE OF YOUR MAIN
    WDT_A_holdTimer();

    // Initialize the system clock and background hardware timer, used to enable
    // software timers to time their measurements properly.
    InitSystemTiming();

    // Initialize the main Application object and HAL object
    HAL hal = HAL_construct();
    Application app = Application_construct();

    // Do not remove this line. This is your non-blocking check.
    InitNonBlockingLED();

    // Main super-loop! In a polling architecture, this function should call
    // your main FSM function over and over.
    while (true)
    {
        // Do not remove this line. This is your non-blocking check.
        PollNonBlockingLED();
        HAL_refresh(&hal);
        Application_loop(&app, &hal);
    }
}



/**
 * A helper function which increments a value with a maximum. If incrementing
 * the number causes the value to hit its maximum, the number wraps around
 * to 0.
 */
uint32_t CircularIncrement(uint32_t value, uint32_t maximum)
{
    return (value + 1) % maximum;
}

/**
 * The main constructor for your application. This function should initialize
 * each of the FSMs which implement the application logic of your project.
 *
 * @return a completely initialized Application object
 */
Application Application_construct()
{
    Application app;

    // Initialize local application state variables here!
    app.baudChoice = BAUD_9600;
    app.firstCall = true;
    // start screen
    app.screenShow = title;
    app.color = OFF;

    return app;
}

/**
 * The main super-loop function of the application. We place this inside of a
 * single infinite loop in main. In this way, we can model a polling system of
 * FSMs. Every cycle of this loop function, we poll each of the FSMs one time,
 * followed by refreshing all inputs to the system through a convenient
 * [HAL_refresh()] call.
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void Application_loop(Application* app_p, HAL* hal_p)
{
    // Sets up application-specific settings on the first run of the program
    if(app_p->firstCall) {
        Application_firstTimeSetup(app_p, hal_p);
        app_p->firstCall = false; // prevents this if-statement from running again
        Application_titlesScreenText(&hal_p->gfxContext);


    }


//test
// Application_titlesScreenText(app_p, &hal_p->gfxContext);

// start
    char rxchar = '\0';
    char txchar = '0';


    _appScreenShow state = app_p->screenShow;
    if(UART_hasChar(&hal_p->uart)){
        rxchar = UART_receiveChar(&hal_p->uart);

        txchar = Application_interpretIncomingChar(rxchar);
        if(UART_canSend(&hal_p->uart)){

            UART_sendChar(&hal_p->uart, txchar);

        }

    }

    // if statements constantly updating for button presses
    // for first button press
    if( Button_isTapped(&hal_p->boosterpackS1) && state == confirm){
        state = askEncrypt;

        GFX_clear(&hal_p->gfxContext);

        Application_askEncryptScreen(&hal_p->gfxContext, "");
    }

    // for second button press
    if(Button_isTapped(&hal_p->boosterpackS1) && state == confirm2){
        state = guessCipher;

        GFX_clear(&hal_p->gfxContext);

        state = switches(app_p, state, hal_p, rxchar, txchar);
    }

    // for third button press
    if (Button_isTapped(&hal_p->boosterpackS1) && state == confirmGC){

        state = checkAnswer;

        GFX_clear(&hal_p->gfxContext);


        state = switches(app_p, state, hal_p, rxchar, txchar);

    }






    if(rxchar != '\0'){
         state = switches(app_p, state, hal_p, rxchar, txchar);

    }

    app_p->screenShow = state;

// end


    // Restart/Update communications if BoosterPack S2 is pressed
    // (which means a new baudrate is being set up)
    if (Button_isTapped(&hal_p->boosterpackS2)) {
        Application_updateCommunications(app_p, hal_p);
    }


    // HW 7

    // Whenever rxChar doesn't have a value of '\0', a new character is received
   // rxchar = '\0';
/*
    if (UART_hasChar(&hal_p->uart)) {
        // The character received from your serial terminal
        rxchar = UART_receiveChar(&hal_p->uart);


        txchar = Application_interpretIncomingChar(rxchar);

        unsigned char text[20]  = {1,1,1,1,1,
                                     1,1,1,1,1,
                                     1,1,1,1,1,
                                     1,1,1,1,1};

        snprintf((char *)text, 20, "%c", rxchar );
        GFX_print(&(hal_p->gfxContext), text , 0, 0);




        if(txchar == 'N'){
            LED_turnOn(&hal_p->boosterpackBlue);
        }
        else if(txchar != 'N')
        {
            LED_turnOff(&hal_p->boosterpackBlue);
        }


        // Only send a character if the UART module can send it
        if (UART_canSend(&hal_p->uart)){
            UART_sendChar(&hal_p->uart, txchar);
        }
        }
*/


}

/**
 * Sets up parameters and settings that need to be initialized when the
 * program starts up. This includes setting up the initial baudrate for
 * UART communication, but can be expanded to include other features, such
 * as drawing the initial state of the game on the LCD.
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void Application_firstTimeSetup(Application* app_p, HAL* hal_p) {
    Application_updateCommunications(app_p, hal_p);
}

/**
 * Updates which LEDs are lit and what baud rate the UART module communicates
 * with, based on what the application's baud choice is at the time this
 * function is called.
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void Application_updateCommunications(Application* app_p, HAL* hal_p)
{
    // When this application first loops, the proper LEDs aren't lit. The
    // firstCall flag is used to ensure that the initial baudrate and LED are set
    if (!app_p->firstCall)
    {
        // When BoosterPack S2 is tapped, circularly increment which baud rate is used.
        uint32_t newBaudNumber = CircularIncrement((uint32_t) app_p->baudChoice, NUM_BAUD_CHOICES);
        app_p->baudChoice = (UART_Baudrate) newBaudNumber;
    }

    // Start/update the baud rate according to the one set above.
    UART_SetBaud_Enable(&hal_p->uart, app_p->baudChoice);

    // Based on the new application choice, turn on the correct LED.
    // To make your life easier, we recommend turning off all LEDs before
    // selectively turning back on only the LEDs that need to be relit.
    // -------------------------------------------------------------------------
    LED_turnOff(&hal_p->launchpadLED2Red);
    LED_turnOff(&hal_p->launchpadLED2Green);
    LED_turnOff(&hal_p->launchpadLED2Blue);

    // TODO: Turn on all appropriate LEDs according to the tasks below.
    switch (app_p->baudChoice)
    {
        // When the baud rate is 9600, turn on Launchpad LED Red
        case BAUD_9600:
            LED_turnOn(&hal_p->launchpadLED2Red);
            LED_turnOff(&hal_p->launchpadLED2Green);
            LED_turnOff(&hal_p->launchpadLED2Blue);
            break;

        // TODO: When the baud rate is 19200, turn on Launchpad LED Green
        case BAUD_19200:
            LED_turnOff(&hal_p->launchpadLED2Red);
            LED_turnOn(&hal_p->launchpadLED2Green);
            LED_turnOff(&hal_p->launchpadLED2Blue);

            break;

        // TODO: When the baud rate is 38400, turn on Launchpad LED Blue
        case BAUD_38400:

            LED_turnOff(&hal_p->launchpadLED2Red);
            LED_turnOff(&hal_p->launchpadLED2Green);
            LED_turnOn(&hal_p->launchpadLED2Blue);

            break;

        // TODO: When the baud rate is 57600, turn on all Launchpad LEDs (illuminates white)
        case BAUD_57600:
            LED_turnOn(&hal_p->launchpadLED2Red);
            LED_turnOn(&hal_p->launchpadLED2Green);
            LED_turnOn(&hal_p->launchpadLED2Blue);
            break;

        // In the default case, this program will do nothing.
        default:
            break;
    }
}

/**
 * Interprets a character which was incoming and returns an interpretation of
 * that character. If the input character is a letter, it return L for Letter, if a number
 * return N for Number, and if something else, it return O for Other.
 *
 * @param rxChar: Input character
 * @return :  Output character
 */
char Application_interpretIncomingChar(char rxChar)
{
    // The character to return back to sender. By default, we assume the letter
    // to send back is an 'O' (assume the character is an "other" character)
    char txChar = 'O';

    // Numbers - if the character entered was a number, transfer back an 'N'
    if (rxChar >= '0' && rxChar <= '9') {
        txChar = 'N';
    }

    // Letters - if the character entered was a letter, transfer back an 'L'
    if ((rxChar >= 'a' && rxChar <= 'z') || (rxChar >= 'A' && rxChar <= 'Z')) {
        txChar = 'L';
    }

    return (txChar);
}



// title screen display
void Application_titlesScreenText(GFX* gfxContext){

        // Display the text
        GFX_print(gfxContext, "Name:", 0, 0);
        GFX_print(gfxContext, "Antonio Dominguez", 1, 0);

        GFX_print(gfxContext, "=== Instructions === ", 3, 0);
        GFX_print(gfxContext, "P1 Encrypts, P2 Guess ", 4, 0);

        GFX_print(gfxContext, "BB1: Confirm", 6, 0);
        GFX_print(gfxContext, "BB2: Change Baudrate", 7, 0);
        GFX_print(gfxContext, "UART: Type", 8, 0);

        GFX_print(gfxContext, "Try to guess the", 10, 0);
        GFX_print(gfxContext, "correct cipher!  ", 11, 0);


}


void Application_askCipherScreen(GFX* gfxContext, HAL* hal, char num[]){

    // clear
    GFX_clear(gfxContext);

    // Display the text
    GFX_print(gfxContext, "Use 9600 bauderate", 0, 0);
    GFX_print(gfxContext, "Enter a", 2, 0);
    GFX_print(gfxContext, "two-digit cipher", 3, 0);

    GFX_print(gfxContext, num, 5, 0);


    GFX_print(gfxContext, "Press BB1 when done. ", 7, 0);
    GFX_print(gfxContext, "Adjust baudrate w/BB2", 8, 0);

    GFX_print(gfxContext, "9600:  RED", 9, 0);
    GFX_print(gfxContext, "19200: GREEN", 10, 0);
    GFX_print(gfxContext, "37400: BLUE", 11, 0);
    GFX_print(gfxContext, "57600: WHITE", 12, 0);
}




void Application_askEncryptScreen(GFX* gfxContext, char text[]){

    // clear
    GFX_clear(gfxContext);

    // Display the text
    GFX_print(gfxContext, "Use 9600 bauderate", 0, 0);
    GFX_print(gfxContext, "Enter a message", 2, 0);
    GFX_print(gfxContext, "to encrypt", 3, 0);
    GFX_print(gfxContext, text, 5, 0);
    GFX_print(gfxContext, "Press BB1 when done. ", 7, 0);
    GFX_print(gfxContext, "Adjust baudrate w/BB2", 8, 0);

    GFX_print(gfxContext, "9600:  RED", 9, 0);
    GFX_print(gfxContext, "19200: GREEN", 10, 0);
    GFX_print(gfxContext, "37400: BLUE", 11, 0);
    GFX_print(gfxContext, "57600: WHITE", 12, 0);



}

void Application_guessCipherScreen(GFX* gfxContext, HAL* hal, char gnum[], int nguesses, char etext[], char dString[], char num[], char text[]){
    char buffferg[BUFFER_SIZE];
    // converting the char array of cipher number to the encryption char array
    static int j;
    static int i;
    for(j = 0; text[j] != NULL; j++){
        sscanf(num, "%d", &i);
        etext[j] = text[j] + ((int)i);

        // regulating so its in ASCII
        if(text[j] + ((int)i )> 127){
            etext[j] =  etext[j] - 128;
        }
        else if (text[j] + ((int)i) < 0){

            etext[j] =  etext[j] + 128;
        }
    }

    // clear display
    GFX_clear(gfxContext);
    // Display the text
    GFX_print(gfxContext, "Guess the cipher!", 0, 0);

    GFX_print(gfxContext, "Encrypted String", 2, 0);
    GFX_print(gfxContext, etext, 3, 0);


    GFX_print(gfxContext, "Cipher: ", 5, 0);
    GFX_print(gfxContext, gnum, 6, 0);


    GFX_print(gfxContext, "Decrypted string:", 8, 0);
    GFX_print(gfxContext, dString, 9, 0);

    GFX_print(gfxContext, "Guesses:", 11, 0);
    snprintf((char *)buffferg, BUFFER_SIZE, "%d", nguesses);
    GFX_print(gfxContext, buffferg, 11, 9);
}



void Application_wonScreen(GFX* gfxContext, HAL* hal, char gnum[], int nguesses, char etext[], char text[]){
    char buffferg[BUFFER_SIZE];

    // clear display
    GFX_clear(gfxContext);

    // Display the text
    GFX_print(gfxContext, "Guess the cipher!", 0, 0);
    GFX_print(gfxContext, "Encrypted String", 2, 0);
    GFX_print(gfxContext, etext, 3, 0);

    // plugging the char array straight out
    GFX_print(gfxContext, "Cipher: ", 5, 0);
    GFX_print(gfxContext, gnum, 6, 0);


    GFX_print(gfxContext, "Decrypted string:", 8, 0);
    GFX_print(gfxContext, text, 9, 0);

    // have to convert the guess to output
    GFX_print(gfxContext, "Guesses:", 11, 0);
    snprintf((char *)buffferg, BUFFER_SIZE, "%d", nguesses);
    GFX_print(gfxContext, buffferg, 11, 9);

    GFX_print(gfxContext, "You Win!", 13, 0);


}


_appScreenShow switches(Application* app_p, _appScreenShow state, HAL* hal_p, char rxchar, char txchar){
    Colors states = app_p->color;
    // initialized variables needed
    static int titlec = 0;
    static int ccount = 0;
    static int ecount = 0;
    static int nguesses = 0;
    static char num[2] = {};
    static char gnum[2] = {};
    static char text[11] = {};
    static char etext[11] = {};
    static char dString[11]={};
    static int gccount = 0;
       switch (state) {
           case title:
               // start page
               Application_titlesScreenText(&(hal_p->gfxContext));
               titlec++;
               if (titlec != 0){
                   state = askCipher; // next state

                   Application_askCipherScreen(&(hal_p->gfxContext), hal_p, num);


               }


            break;

            case askCipher:
               if(txchar == 'N' && ccount < 2){
                   // fist number
                   num[ccount] = rxchar;
                   ccount++; //add to count to regulate digits


               }
               if(ccount > 0 ){
                   //go to confirm so can press button or add more
                   state = confirm;

               }
               Application_askCipherScreen(&(hal_p->gfxContext), hal_p, num);



            break;

            case confirm:
                if(txchar == 'N' &&  ccount < 2){
                    num[ccount] = rxchar;
                    ccount++;
                }
                Application_askCipherScreen(&(hal_p->gfxContext), hal_p, num);



            break;
            case askEncrypt:
               if(ecount < 11){
                   // first char inputed
                   text[ecount] = rxchar;
                   ecount++;//add to count to regulate digits
               }
               if(ecount > 0){
                   //go to confirm so can press button or add more
                   state = confirm2;
               }
               Application_askEncryptScreen(&(hal_p->gfxContext), text);

            break;
            case confirm2:
                if(ecount < 11){
                    text[ecount] = rxchar;
                    ecount++;
                }



                Application_askEncryptScreen(&(hal_p->gfxContext), text);
            break;

            case guessCipher:
                if(txchar == 'N' && gccount < 2){
                    // first char inputed
                    gnum[gccount] = rxchar;
                    gccount++; //add to count to regulate digits
                }
                if(gccount > 0){
                    //go to confirm so can press button or add more
                    state = confirmGC;


                }

                Application_guessCipherScreen(&(hal_p->gfxContext), hal_p, gnum, nguesses, etext, dString, num, text);
            break;

            case confirmGC:
                if(txchar == 'N' &&  gccount < 2){
                    gnum[gccount] = rxchar;
                    gccount++;
                }
                Application_guessCipherScreen(&(hal_p->gfxContext), hal_p, gnum, nguesses, etext, dString, num, text);


            break;

            case checkAnswer:
                // if the if the guess and actual match go to win screen :)
                if(num[0] == gnum[0] && num[1] == gnum[1]){
                    nguesses++;
                    Application_wonScreen(&(hal_p->gfxContext), hal_p, gnum, nguesses, etext, text);
                    //changes the color be off
                    static int i;
                    static int g;
                    sscanf(num, "%d", &i); // converting
                    sscanf(gnum, "%d", &g);
                    signed int check = g - i;
                    checkColors(states, hal_p, check);

                }
                else{
                    nguesses++;
                    static int j;
                    static int i;
                    static int g;
                    for(j = 0; text[j] != NULL; j++){
                        sscanf(num, "%d", &i); // converting
                        sscanf(gnum, "%d", &g);
                        dString[j] = text[j] + ((int)i) - g;
                        // regulating so its in ASCII
                        if(text[j] + ((int)i) - g > 127){
                            dString[j] =  dString[j] - 128;
                        }
                        else if(text[j] + ((int)i) - g < 0){

                            dString[j] =  dString[j] + 128;
                        }
                    }
                    signed int check = g - i;
                    //changes the color to correspond how far off the guess is
                    checkColors(states, hal_p, check);
                    // reset the array
                    gnum[0] = NULL;
                    gnum[1] = NULL;
                    gccount = 0;
                    state = guessCipher;
                    Application_guessCipherScreen(&(hal_p->gfxContext), hal_p, gnum, nguesses, etext, dString, num, text);
                }
            break;


       }
       app_p->color = states;

       return state;

}
void checkColors(Colors states, HAL* hal_p, int check) {
    // if statement regulating what state(Color) turns on when
    if(check == 0){
        states = OFF;
    }
    else if(check < -20 && check != 0){
        states = RED;
    }
    else if(-20 <= check && check <= -6){
        states = MAGENTA;
    }
    else if(-5 <= check && check <= 5 && check != 0){
        states = WHITE;

    }
    else if(6 <= check && check <= 20){

        states = CYAN;
    }
    else{

        states = GREEN;
    }


    // switchs for selecting the LED
    switch(states) {
    case OFF:
        LED_turnOff(&hal_p->boosterpackRed);
        LED_turnOff(&hal_p->boosterpackGreen);
        LED_turnOff(&hal_p->boosterpackBlue);

    break;
    case RED:
        LED_turnOn(&hal_p->boosterpackRed);
        LED_turnOff(&hal_p->boosterpackGreen);
        LED_turnOff(&hal_p->boosterpackBlue);

    break;
    case MAGENTA:
        LED_turnOn(&hal_p->boosterpackRed);
        LED_turnOff(&hal_p->boosterpackGreen);
        LED_turnOn(&hal_p->boosterpackBlue);

    break;
    case WHITE:
        LED_turnOn(&hal_p->boosterpackRed);
        LED_turnOn(&hal_p->boosterpackGreen);
        LED_turnOn(&hal_p->boosterpackBlue);

    break;
    case CYAN:
        LED_turnOff(&hal_p->boosterpackRed);
        LED_turnOn(&hal_p->boosterpackGreen);
        LED_turnOn(&hal_p->boosterpackBlue);

    break;
    case GREEN:
        LED_turnOff(&hal_p->boosterpackRed);
        LED_turnOn(&hal_p->boosterpackGreen);
        LED_turnOff(&hal_p->boosterpackBlue);


    break;
    }
}

