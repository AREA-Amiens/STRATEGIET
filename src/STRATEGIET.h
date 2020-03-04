// je charge la bibliothèque qui nous permet de communiquer
// avec les composants utilisant le protocole I2C
#include <Wire.h>



#define RESET_G   15
#define SLEEP_G   13
#define STEP_G    0
#define DIR_G     1

#define RESET_D 9
#define SLEEP_D 10
#define STEP_D 11
#define DIR_D 12


// Définition des adresses logiques des esclaves I2C.
#define I2C_SLAVE_LED_ADDRESS  1
#define I2C_SLAVE_ENGINE_ADDRESS  2



void readRegisterAndSendValue();