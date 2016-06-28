#include "NCDRelayGeneric.h"
#include "spark_wiring_usbserial.h"

//Comment line below out to turn off Serial logging
#define LOGGING

//Constructor
NCD32Relay::NCD32Relay(){
}

void NCD32Relay::setNumberOfRelays(int relays){
	numberOfRelays = relays;
	Serial.printf("numberOfRelays: %i \n",numberOfRelays);
}

//24 or 32 relays
void NCD32Relay::setAddress(int a0, int a1){
	address1 = 0x20;
	address2 = 0x21;
	if(a0 == 1){
		address1 = address1 | 2;
		address2 = address2 | 2;
	}
	if(a1 == 1){
		address1 = address1 | 4;
		address2 = address2 | 4;
	}
	//Start I2C port
	Wire.begin();
	byte writeData[2] = {0,0};
	if(!i2cWrite(address1, 0, writeData, 2)){
		initialized = false;
		Serial.println("Initialization of relay controller failed on first address");
		return;
	}else{
		readStatus(address1);
	}

	byte writeData2[2];
	if(numberOfRelays == 32){
		writeData2[0] = 0;
		writeData2[1] = 0;
	}else{
		writeData2[0] = 0;
		writeData2[1] = 255;
	}
	if(!i2cWrite(address2, 0, writeData2, 2)){
		initialized = false;
		Serial.println("Initialization of relay controller failed on second address");
		return;
	}else{
		readStatus(address2);
	}
	initialized = true;
}
//1, 2, 4, 8, or 16 relays
void NCD32Relay::setAddress(int a0, int a1, int a2){
	address1 = 0x20;
	if(a0 == 1){
		address1 = address1 | 1;
	}
	if(a1 == 1){
		address1 = address1 | 2;
	}
	if(a2 == 1){
		address1 = address1 | 4;
	}
	Wire.begin();

	if(numberOfRelays == 16 || numberOfRelays ==8){
		byte writeData[2] = {0,0};
		if(numberOfRelays == 16){
			if(!i2cWrite(address1, 0, writeData, 2)){
				Serial.println("Initialization of relay controller failed");
				return;
			}

		}else{
			if(!i2cWrite(address1, 0, writeData, 1)){
				Serial.println("Initialization of relay controller failed");
				return;
			}
		}
	}else{
		byte writeData[1];
		byte writeData2[1];
		switch(numberOfRelays){
		case 1:
			writeData[0] = oneRelayMux;
			writeData2[0] = oneRelayMux;
			break;
		case 2:
			writeData[0] = twoRelayMux;
			writeData2[0] = twoRelayMux;
			break;
		case 4:
			writeData[0] = fourRelayMux;
			writeData2[0] = fourRelayMux;
			break;
		}
		if(!i2cWrite(address1, 0x00, writeData, 1)){
			Serial.println("Initialization of relay controller failed");
			return;
		}
		if(!i2cWrite(address1, 0x06, writeData2, 1)){
			Serial.println("Initialization of relay controller failed");
			return;
		}
	}
	initialized = true;
	readStatus(address1);
}

void NCD32Relay::turnOnRelay(int relay){
	byte bankValue = 0;
	byte registerAddress = 0;
	byte addr = 0;
	if(relay <= 8){
		bankValue = bankOneStatus;
		addr = address1;
		if(numberOfRelays <=8){
			registerAddress = 0x0A;
		}else{
			registerAddress = 18;
		}
	}else{
		if(relay > 8 && relay <=16){
			relay = relay - 8;
			bankValue = bankTwoStatus;
			addr = address1;
			registerAddress = 19;
		}else{
			if(relay > 16 && relay <= 24){
				relay = relay - 16;
				bankValue = bankThreeStatus;
				addr = address2;
				registerAddress = 18;
			}else{
				if(relay > 24 && relay <=32){
					relay = relay - 24;
					bankValue = bankFourStatus;
					addr = address2;
					registerAddress = 19;
				}
			}
		}

	}
	switch(relay){
	case 1:
		bankValue = bankValue | 1;
		break;
	case 2:
		bankValue = bankValue | 2;
		break;
	case 3:
		bankValue = bankValue | 4;
		break;
	case 4:
		bankValue = bankValue | 8;
		break;
	case 5:
		bankValue = bankValue | 16;
		break;
	case 6:
		bankValue = bankValue | 32;
		break;
	case 7:
		bankValue = bankValue | 64;
		break;
	case 8:
		bankValue = bankValue | 128;
		break;
	}
	if(!i2cWrite(addr, registerAddress, bankValue)){
		initialized = false;
		Serial.println("Turn on relay failed");
		return;
	}
	initialized = true;
	readStatus(addr);
}

void NCD32Relay::turnOffRelay(int relay){
	byte bankValue = 0;
	byte registerAddress = 0;
	byte addr = 0;
	if(relay <= 8){
		bankValue = bankOneStatus;
		addr = address1;
		if(numberOfRelays <=8){
			registerAddress = 0x0A;
		}else{
			registerAddress = 18;
		}
	}else{
		if(relay > 8 && relay <=16){
			relay = relay - 8;
			bankValue = bankTwoStatus;
			addr = address1;
			registerAddress = 19;
		}else{
			if(relay > 16 && relay <= 24){
				relay = relay - 16;
				bankValue = bankThreeStatus;
				addr = address2;
				registerAddress = 18;
			}else{
				if(relay > 24 && relay <=32){
					relay = relay - 24;
					bankValue = bankFourStatus;
					addr = address2;
					registerAddress = 19;
				}
			}
		}

	}
	switch(relay){
	case 1:
		bankValue = bankValue & ~1;
		break;
	case 2:
		bankValue = bankValue & ~2;
		break;
	case 3:
		bankValue = bankValue & ~4;
		break;
	case 4:
		bankValue = bankValue & ~8;
		break;
	case 5:
		bankValue = bankValue & ~16;
		break;
	case 6:
		bankValue = bankValue & ~32;
		break;
	case 7:
		bankValue = bankValue & ~64;
		break;
	case 8:
		bankValue = bankValue & ~128;
		break;
	}
	if(!i2cWrite(addr, registerAddress, bankValue)){
		initialized = false;
		Serial.println("Turn on relay failed");
		return;
	}
	initialized = true;
	readStatus(addr);
}

void NCD32Relay::turnOnAllRelays(){
	if(numberOfRelays <= 8){
		if(!i2cWrite(address1, 0x0A, 255)){
			Serial.println("Turn On all relays failed");
			initialized = false;
		}
		initialized = true;
		readStatus(address1);
		return;
	}
	byte writeData[2] = {255, 255};
	if(!i2cWrite(address1, 18, writeData, 2)){
		Serial.println("Turn On all relays failed");
		initialized = false;
	}
	readStatus(address1);
	if(!i2cWrite(address2, 18, writeData, 2)){
		Serial.println("Turn On all relays failed");
		initialized = false;
	}
	readStatus(address2);
	initialized = true;
}

void NCD32Relay::turnOffAllRelays(){
	if(numberOfRelays <= 8){
		if(!i2cWrite(address1, 0x0A, 0)){
			Serial.println("Turn Off all relays failed");
			initialized = false;
		}
		initialized = true;
		readStatus(address1);
		return;
	}
	byte writeData[2] = {0, 0};
	if(!i2cWrite(address1, 18, writeData, 2)){
		Serial.println("Turn Off all relays failed");
		initialized = false;
	}
	readStatus(address1);
	if(!i2cWrite(address2, 18, writeData, 2)){
		Serial.println("Turn Off all relays failed");
		initialized = false;
	}
	readStatus(address2);
	initialized = true;
}

void NCD32Relay::turnOnAllRelays(int bank){

	if(numberOfRelays <= 8){
		if(!i2cWrite(address1, 0x0A, 255)){
			Serial.println("Turn On all relays in bank failed");
			initialized = false;
		}
		initialized = true;
		readStatus(address1);
		return;
	}

	byte addr = 0;
	if(bank == 1 || bank == 2){
		addr = address1;
	}else{
		if(bank == 3 || bank == 4){
			addr = address2;
		}else{
			//Bad bank value
			return;
		}
	}

	bank = 17+bank;

	if(!i2cWrite(addr, bank, 255)){
		Serial.println("Turn On all relays in bank failed");
		initialized = false;
	}
	initialized = true;
	readStatus(addr);
	return;
}

void NCD32Relay::turnOffAllRelays(int bank){
	if(numberOfRelays <= 8){
		if(!i2cWrite(address1, 0x0A, 0)){
			Serial.println("Turn Off all relays in bank failed");
			initialized = false;
		}
		initialized = true;
		readStatus(address1);
		return;
	}

	byte addr = 0;
	if(bank == 1 || bank == 2){
		addr = address1;
	}else{
		if(bank == 3 || bank == 4){
			addr = address2;
		}else{
			//Bad bank value
			return;
		}
	}

	bank = 17+bank;

	if(!i2cWrite(addr, bank, 0)){
		Serial.println("Turn Off all relays in bank failed");
		initialized = false;
	}
	initialized = true;
	readStatus(addr);
	return;
}

void NCD32Relay::toggleRelay(int relay){
	byte bankValue = 0;
	byte registerAddress = 0;
	byte addr = 0;
	if(relay <= 8){
		bankValue = bankOneStatus;
		addr = address1;
		if(numberOfRelays <=8){
			registerAddress = 0x0A;
		}else{
			registerAddress = 18;
		}
	}else{
		if(relay > 8 && relay <=16){
			relay = relay - 8;
			bankValue = bankTwoStatus;
			addr = address1;
			registerAddress = 19;
		}else{
			if(relay > 16 && relay <= 24){
				relay = relay - 16;
				bankValue = bankThreeStatus;
				addr = address2;
				registerAddress = 18;
			}else{
				if(relay > 24 && relay <=32){
					relay = relay - 24;
					bankValue = bankFourStatus;
					addr = address2;
					registerAddress = 19;
				}
			}
		}

	}
	switch(relay){
	case 1:
		bankValue = bankValue ^ 1;
		break;
	case 2:
		bankValue = bankValue ^ 2;
		break;
	case 3:
		bankValue = bankValue ^ 4;
		break;
	case 4:
		bankValue = bankValue ^ 8;
		break;
	case 5:
		bankValue = bankValue ^ 16;
		break;
	case 6:
		bankValue = bankValue ^ 32;
		break;
	case 7:
		bankValue = bankValue ^ 64;
		break;
	case 8:
		bankValue = bankValue ^ 128;
		break;
	}
	if(!i2cWrite(addr, registerAddress, bankValue)){
		initialized = false;
		Serial.println("Toggle on relay failed");
		return;
	}
	initialized = true;
	readStatus(addr);
}

void NCD32Relay::setBankStatus(int status, int bank){
	byte addr;
	if(bank == 1 || bank == 2){
		addr = address1;
	}else{
		if(bank == 3 || bank == 4){
			addr = address2;
		}else{
			//Bad bank value
			return;
		}
	}
	setBankStatusRetry:
	Wire.beginTransmission(addr);
	if(numberOfRelays <= 8){
		bank = 0x0A;
	}else{
		bank = 17+bank;
	}

	if(!i2cWrite(addr, bank, status)){
		initialized = false;
		Serial.println("Set Bank Status failed");
		return;
	}
	initialized = true;
	readStatus(addr);
}

void NCD32Relay::setAllRelayStatus(int bank1, int bank2, int bank3, int bank4){
	if(numberOfRelays <=8){
		setBankStatus(bank1, 1);
		return;
	}
	byte writeData1[2] = {bank1, bank2};
	if(!i2cWrite(address1, 18, writeData1, 2)){
		Serial.println("Set All Relay Status failed");
		initialized = false;
		return;
	}
	readStatus(address1);

	byte writeData2[2] = {bank3, bank4};
	if(!i2cWrite(address2, 18, writeData2, 2)){
		Serial.println("Set All Relay Status failed");
		initialized = false;
		return;
	}
	readStatus(address2);
	initialized = true;
}

int NCD32Relay::readRelayStatus(int relay){
	byte registerAddress = 0;
	int value = 0;
	int addr = 0;
	if(relay <= 8){
		addr = address1;
		if(numberOfRelays <= 8){
			registerAddress = 0x0A;
		}else{
			registerAddress = 18;
		}

	}else{
		if(relay > 8 && relay <=16){
			relay = relay - 8;
			addr = address1;
			registerAddress = 19;
		}else{
			if(relay > 16 && relay <= 24){
				relay = relay - 16;
				addr = address2;
				registerAddress = 18;
			}else{
				if(relay > 24 && relay <=32){
					relay = relay - 24;
					addr = address2;
					registerAddress = 19;
				}
			}
		}

	}
	switch(relay){
	case 1:
		value = 1;
		break;
	case 2:
		value = 2;
		break;
	case 3:
		value = 4;
		break;
	case 4:
		value = 8;
		break;
	case 5:
		value = 16;
		break;
	case 6:
		value = 32;
		break;
	case 7:
		value = 64;
		break;
	case 8:
		value = 128;
		break;
	}
	int relayStatus = i2cRead(addr, registerAddress);
	if(relayStatus == 256){
		Serial.println("Read Relay Status failed");
		initialized = false;
		return relayStatus;
	}
	if(relayStatus & value){
		return 1;
	}else{
		return 0;
	}
}

int NCD32Relay::readBankStatus(int bank){
	int addr = 0;
	if(bank == 1 || bank == 2){
		addr = address1;
	}else{
		if(bank == 3 || bank == 4){
			addr = address2;
		}else{
			//Bad bank value
			return 256;
		}
	}
	if(numberOfRelays <= 8){
		bank = 0x0A;
	}else{
		bank = bank + 17;
	}
	int bankStatus = i2cRead(addr, bank);
	if(bankStatus == 256){
		Serial.println("Read Bank Status failed");
		initialized = false;
		return bankStatus;
	}
	initialized = true;
	return bankStatus;
}

void NCD32Relay::readStatus(int addr){
	byte reg;
	if(numberOfRelays <= 8){
		reg = 0x0A;
	}else{
		reg = 18;
	}
	int bank1 = i2cRead(addr, reg);
	if(bank1 == 256){
		Serial.println("readStatus failed");
		initialized = false;
		return;
	}
	if(addr == address1){
		bankOneStatus = bank1;
		if(numberOfRelays <= 8){
			return;
		}
	}else{
		if(addr == address2){
			bankThreeStatus = bank1;
		}
	}

	int bank2 = i2cRead(addr, 19);
	if(bank2 == 256){
		Serial.println("readStatus failed");
		initialized = false;
		return;
	}
	if(addr == address1){
		bankTwoStatus = bank2;
	}else{
		if(addr == address2){
			bankFourStatus = bank2;
		}
	}
	initialized = true;
}

bool NCD32Relay::i2cWrite(int addr, byte reg, byte *data, int count){
	Wire.beginTransmission(addr);
	Wire.write(reg);
	for(int i = 0; i < count; i++){
		Wire.write(data[i]);
	}
	byte status = Wire.endTransmission();
	if(status != 0){
		return false;
	}
	return true;
}

bool NCD32Relay::i2cWrite(int addr, byte reg, byte data){
	Wire.beginTransmission(addr);
	Wire.write(reg);
	Wire.write(data);
	byte status = Wire.endTransmission();
	if(status != 0){
		return false;
	}
	return true;
}

int NCD32Relay::i2cRead(int addr, byte reg){
	Wire.beginTransmission(addr);
	Wire.write(reg);
	int status = Wire.endTransmission();
	if(status != 0){
		return 256;
	}
	Wire.requestFrom(addr, 1);
	return Wire.read();
}
