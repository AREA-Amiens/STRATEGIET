#include <Wire.h>      //liaison I2C pour les cartes
#include <MsTimer2.h>  // Timer
#include <logo-area.h>


#include <U8g2lib.h>  //gestion de l'ecran lcd grafique

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>      //liaison SPI pour ecran
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


#define RESET_G   15
#define SLEEP_G   13
#define STEP_G    0
#define DIR_G     1

#define RESET_D 9
#define SLEEP_D 10
#define STEP_D 11
#define DIR_D 12


// Définition des adresses logiques des esclaves I2C.
#define I2C_SLAVE_DETECTION_ADDRESS  1
#define I2C_SLAVE_DEPLACEMENT_ADDRESS  2

#define coteJaune 1
#define coteBleu 0

void readRegisterAndSendValue();

void drawLogoArea();

void algoMenu1();

void menu1(int blanc);

void menu2();

void lectureEncoder();

void IntrerrupTimer();
