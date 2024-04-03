
// #include "MicroBit.h"
// MicroBit uBit;
 
// int main()
// {
//     uBit.init();
//     MicroBitI2C i2c(I2C_SDA0, I2C_SCL0);
//     int address = 0x70<<1;
//     int hzFreq=400000;
//     i2c.frequency(hzFreq);
//     char data[20];
//     bool repeatedStart=false;
//     int length=2;
   
//     while(1)
//     {
//         i2c.read(address, data, 2, repeatedStart);
//     }
// }
#include "MicroBit.h"
#include "MicroBitUARTService.h"
#include "MicroBitUARTService2.h"
#include "MicroBitUARTService3.h"
#include "MicroBitI2C.h"
#include "AnalogOut.h"
///#include "mbed2/299/TARGET_NRF51_MICROBIT/TARGET_NORDIC/TARGET_MCU_NRF51822/TARGET_NRF51_MICROBIT/PinNames.h"

MicroBit uBit;
MicroBitUARTService *uart;
MicroBitUARTService2 *uart2;
MicroBitUARTService3 *uart3;
MicroBitI2C i2c(I2C_SDA0, I2C_SCL0);

int counter1 = 0;
int counter2 = 0;
int counter3 = 0;
int currentUART = 0;  // Variable to keep track of the current UART to send data
Ticker ticker;
Ticker AnalogTicker;
AnalogIn analogIn(MICROBIT_PIN_P0);
uint8_t data1[20]; // 10 integers * 2 bytes each
uint8_t data2[20]; // 10 integers * 2 bytes each
uint8_t data3[20]; // 10 integers * 2 bytes each
char data1i2c[20];
char i2cdata;
int i2c_address = 0x70<<1;
int i2c_hzFreq=400000;
bool i2c_repeatedStart=false;
int i2c_length=2;
int index_tracker = 0;

void addAnalog(){
       for (int i = 0; i < 10; i++) {
    //   if (index_tracker >= 10){
    //       index_tracker = 0;
    //   }
            int analogValue = analogIn.read_u16(); //ad shift?
            data1[i * 2] = (analogValue >> 8) & 0xFF;  // Higher byte
            data1[i * 2 + 1] = analogValue & 0xFF; 
          // Lower byte
           }
}



//**have global variable to keep track of i, only read in once (remove for), and once i=10 reset to 0.
void addI2C(){
    // for(int j = 0; j < 2; j++){
    // if (index_tracker >= 20){
    //     index_tracker = 0;
    // }
      for (int i = 0; i < 20; i++) {
          i2c.read(i2c_address, &data1i2c[index_tracker], 1, i2c_repeatedStart);
          index_tracker++;
       }
    }
}

void sendData() {

    // Send data based on the current UART
    switch (currentUART) {
        case 0:
             //uart->send(data1, 20);
                uint8_t dataToSend[20];
                for (int i = 0; i < 20; i++) {
                dataToSend[i] = static_cast<uint8_t>(data1i2c[i]);
                }
            uart->send(dataToSend, 20);
            break;
        case 1:
               for (int i = 0; i < 10; i++) {
        // Add integer to buffer
        data2[i * 2] = (counter2 >> 8) & 0xFF;  // Higher byte
        data2[i * 2 + 1] = counter2 & 0xFF;     // Lower byte

        // Increment counter
        counter2++;

        // Reset counter if necessary
        if (counter2 > 10) {
            counter2 = 0;
        }
    }
            uart2->send(data2, 20);
            break;
        case 2:
            for (int i = 0; i < 10; i++) {
        // Add integer to buffer
        data3[i * 2] = (counter3 >> 8) & 0xFF;  // Higher byte
        data3[i * 2 + 1] = counter3 & 0xFF;     // Lower byte

        // Increment counter
        counter3++;

        // Reset counter if necessary
        if (counter3 > 10) {
            counter3 = 0;
        }
    }
            uart3->send(data3, 20);
            break;
    }

    // Increment currentUART and wrap around if necessary
    currentUART = (currentUART + 1) % 3;
}

int main() {
    // Initialize micro:bit runtime
    uBit.init();

    // Create UART services
    uart3 = new MicroBitUARTService3(*uBit.ble, 32, 32);
    uart2 = new MicroBitUARTService2(*uBit.ble, 32, 32);
    uart = new MicroBitUARTService(*uBit.ble, 32, 32);

    // Set up timer to send data every 0.1 seconds
    ticker.attach(sendData, 0.1);
   // AnalogTicker.attach(addI2C, 0.00666);
   AnalogTicker.attach(addAnalog, 0.03);

    // Keep the main fiber running
    release_fiber();
}