#include "spark_wiring_i2c.h"
#include "spark_wiring_usbserial.h"
#include "spark_wiring_constants.h"

class NCD32Relay{
public:
    //Constructor
    NCD32Relay(void);
    //Set the number of relays on the board
    void setNumberOfRelays(int relays);
    //Set Address.  Overload functions for either 2 or 3 address jumpers on board
    //1, 2, 4, 8, or 16 relays
    void setAddress(int a0, int a1, int a2);
    //24 or 32 relays
    void setAddress(int a0, int a1);
    //Turn on Relay
    void turnOnRelay(int Relay);
    //Turn off Relay
    void turnOffRelay(int Relay);
    //Turn On all Relays
    void turnOnAllRelays();
    //Turn Off All Relays
    void turnOffAllRelays();
    //Turn On All Relays in bank
    void turnOnAllRelays(int bank);
    //Turn Off All Relays in bank
    void turnOffAllRelays(int bank);
    //Toggle Relay
    void toggleRelay(int relay);
    //Set status of all relays in bank
    void setBankStatus(int status, int bank);
    //Set status of all relays on board
    void setAllRelayStatus(int bank1, int bank2, int bank3, int bank4);
    //Read status of relay. Valid return 0 for off 1 for on.  256 returned if there is an error
    int readRelayStatus(int relay);
    //Read status of all relays in bank.  0-255 valid.  256 returned on error
    int readBankStatus(int bank);
    
    //User Accessible Variables
    //Whether or not the controller is ready to accept commands
    bool initialized = false;
    
private:
    //internal use method for refreshing bank status variables
    void readStatus(int addr);
    bool i2cWrite(int addr, byte reg, byte *data, int count);
    bool i2cWrite(int addr, byte reg, byte data);
    int i2cRead(int addr, byte reg);
    //Status of relays in bank 1
    byte bankOneStatus = 0;
    //Status of relays in bank 2
    byte bankTwoStatus = 0;
    //Status of relays in bank 3
    byte bankThreeStatus = 0;
    //Status of relays in bank 4
    byte bankFourStatus = 0;

    int numberOfRelays = 0;
    int numberOfInputs = 0;

    byte bank1Reg = 0;
    byte bank2Reg = 0;
    byte bank3Reg = 0;
    byte bank4Reg = 0;

    int address1 = 0x00;
    int address2 = 0x00;

    //Pin mux
    byte oneRelayMux = 0xfe;
    byte twoRelayMux = 0xfc;
    byte fourRelayMux = 0xf0;
};
