/*
	Example code to read two I2C AHT20 sensors connected to different pins
	and print their temperature to the serial terminal using the SoftWire
	software I2C library.
	
	Based on ChatGPT generated code, but modified to allow two sensors.
	
	Tested with ESP32-C3 board and two AHT20 breakouts from AliExpress.
*/

#include <SoftWire.h>

// sensor 1 pins
#define SENSOR1_SDA_PIN 4
#define SENSOR1_SCL_PIN 3

// sensor 2 pins
#define SENSOR2_SDA_PIN 6
#define SENSOR2_SCL_PIN 5

// AHT20 address. should be 0x38 normally
#define AHT20_ADDR 0x38


SoftWire i2c_1(SENSOR1_SDA_PIN, SENSOR1_SCL_PIN);
SoftWire i2c_2(SENSOR2_SDA_PIN, SENSOR2_SCL_PIN);

// Required by SoftWire high-level API
uint8_t txBuffer_1[16];
uint8_t rxBuffer_1[16];
uint8_t txBuffer_2[16];
uint8_t rxBuffer_2[16];

bool aht20Init();
bool aht20ReadTemperature(float &tempC);

void setup() {
    Serial.begin(115200);

    i2c_1.setTxBuffer(txBuffer_1, sizeof(txBuffer_1));
    i2c_1.setRxBuffer(rxBuffer_1, sizeof(rxBuffer_1));
	i2c_2.setTxBuffer(txBuffer_2, sizeof(txBuffer_2));
    i2c_2.setRxBuffer(rxBuffer_2, sizeof(rxBuffer_2));

    i2c_1.begin();
	i2c_2.begin();

    delay(100);

    if (!aht20Init(i2c_1)) {
        Serial.println("1 AHT20 init failed");
        while (1);
    }
	
	if (!aht20Init(i2c_2)) {
        Serial.println("2 AHT20 init failed");
        while (1);
    }

    Serial.println("AHT20 ready");
}

void loop() {
    float tempC;

    if (aht20ReadTemperature(i2c_1, tempC)) {
        Serial.print("1 Temperature: ");
        Serial.print(tempC, 2);
        Serial.println(" C");
    } else {
        Serial.println("1 Read failed");
    }
	
	if (aht20ReadTemperature(i2c_2, tempC)) {
        Serial.print("2 Temperature: ");
        Serial.print(tempC, 2);
        Serial.println(" C");
    } else {
        Serial.println("2 Read failed");
    }

    delay(2000);
}

bool aht20Init(SoftWire &i2c) {
    // Optional soft reset
    i2c.beginTransmission(AHT20_ADDR);
    i2c.write(0xBA);

    if (i2c.endTransmission() != 0)
        return false;

    delay(50);

    // Calibration / initialization command
    i2c.beginTransmission(AHT20_ADDR);
    i2c.write(0xBE);
    i2c.write(0x08);
    i2c.write(0x00);

    if (i2c.endTransmission() != 0)
        return false;

    delay(20);

    // Read status byte
    uint8_t count = i2c.requestFrom(AHT20_ADDR, (uint8_t)1);

    if (count != 1)
        return false;

    uint8_t status = i2c.read();

    // Bit 3 = calibrated
    if (!(status & 0x08)) {
        Serial.print("Status = 0x");
        Serial.println(status, HEX);
        return false;
    }

    return true;
}

bool aht20ReadTemperature(SoftWire &i2c, float &tempC) {
    // Trigger measurement
    i2c.beginTransmission(AHT20_ADDR);
    i2c.write(0xAC);
    i2c.write(0x33);
    i2c.write(0x00);

    if (i2c.endTransmission() != 0)
        return false;

    delay(100);

    uint8_t data[7];

    uint8_t count = i2c.requestFrom(AHT20_ADDR, (uint8_t)7);

    if (count != 7)
        return false;

    for (uint8_t i = 0; i < 7; i++) {
        if (!i2c.available())
            return false;

        data[i] = i2c.read();
    }

    uint8_t status = data[0];

    // Busy bit
    if (status & 0x80)
        return false;

    uint32_t rawTemp =
        ((uint32_t)(data[3] & 0x0F) << 16) |
        ((uint32_t)data[4] << 8) |
        ((uint32_t)data[5]);

    tempC =
        ((float)rawTemp * 200.0f / 1048576.0f) - 50.0f;

    return true;
}