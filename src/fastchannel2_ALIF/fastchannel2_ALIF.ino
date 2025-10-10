#include <STM32LowPower.h>

#define TEST_PWR_MAX 20
#define TEST_PWR_MIN 0
#define TEST_PWR_STEP 5
#define NUM_PKT_EACH_CONFIG 1000

#define LED_USER_1_G D12  // Red on my board
#define LED_USER_2_R P45  // White on my board
#define BTN_USER_1 P36
#define BTN_USER_2 P31

HardwareSerial Serial2(PA3, PA2);  //Serial for AT Commands to send to the Satellite through the module EchoStar
HardwareSerial Serial1(PB7, PB6);

uint32_t counter = 0;
int8_t tx_power = TEST_PWR_MAX;

String inputBuffer = "";
String Buffer = "";
int packetNumber;

#define PACKET_BUFFER_SIZE 512 // In bytes
char packet[PACKET_BUFFER_SIZE];
int indexBuffer = 0;


void increase_counter(void) {
	counter++;

	if (counter >= NUM_PKT_EACH_CONFIG) {
		counter = 0;

		tx_power -= TEST_PWR_STEP;
		if (tx_power < TEST_PWR_MIN) {
			tx_power = TEST_PWR_MAX;
		}

		Serial2.print("AT+TXPMSS=");
		Serial2.print(tx_power);
		Serial2.print("\r\n");
		delay(5000);
	}
}

void led_blink(int num_of_blink = 2) {
	while (num_of_blink-- > 0) {
		digitalWrite(LED_USER_1_G, HIGH);
		delay(50);
		digitalWrite(LED_USER_1_G, LOW);
		delay(150);
	}
}

void setup() {
	counter = 0;
	tx_power = TEST_PWR_MAX;

	// LEDs
	pinMode(LED_USER_1_G, OUTPUT);
	digitalWrite(LED_USER_1_G, HIGH);

	LowPower.begin();

	Serial1.begin(115200);

	Serial2.begin(115200);

	// if (api.system.sleep.registerWakeupCallback(WakeupCallback) == false) {
	// 	Serial1.println("Create Wakeup Callback failed.");
	// }

	Serial2.print("AT+TXPMSS=");
	Serial2.print(tx_power);
	Serial2.print("\r\n");
	delay(5000);

	Serial1.println("Starting!");
	digitalWrite(LED_USER_1_G, LOW);

	Serial2.println("AT+JOIN\r\n");
	delay(5000);
}

void loop() {
	// while (!Serial2.available())
	// 	;

	// while (Serial2.available()) {
	// 	Serial2.write(Serial2.read());
	// }

	// delay(10000);

	// while (!Serial2.available()) {
	//increase_counter();



	while (Serial1.available()) {
		char c = Serial1.read();
		//Serial2.write(c);

		//Serial1.write(c);

		inputBuffer += c;

		// Line by line checking
		if (c == '\n') {
			//inputBuffer.trim();
			Buffer += inputBuffer;

			if (inputBuffer.startsWith("test_ssdv thread: begin ") || inputBuffer.indexOf("test_ssdv thread: begin ") != -1) {
				packetNumber = 0;

				// NOT SENDING, IT COULD MAKE THE CHANNEL BUSY WHEN THE FIST SEGMENT COME
				// if (isJoined()) {
				// 	Serial2.print("AT+SEND=1,7,10,0,-------------------------");
				// 	Serial2.print("Beginning");
				// 	Serial2.print("-------------------------\r\n");
				// }
				Serial1.print("-------------------------");
				Serial1.print("Beginning");
				Serial1.print("-------------------------");
			}
			// Check if the line is the success
			if (inputBuffer.startsWith("5566")) {
				//Serial2.println(packetNumber);
				//Serial2.println(Buffer);

				memset(packet, 0, PACKET_BUFFER_SIZE);

				snprintf(packet, PACKET_BUFFER_SIZE, "AT+SENDB=1,8,21,0,%.4x%s\r\n", packetNumber, Buffer.c_str());
				

				if (isJoined()) {
					Serial2.println(packet);
				}
				Serial1.println(packet);


				packetNumber += 1;
			}

			if (inputBuffer.startsWith("test_ssdv thread: end ") || inputBuffer.indexOf("test_ssdv thread: end ") != -1) {
				packetNumber = 0;

				// Delay before sending the end message
				delay(8000);
				if (isJoined()) {
					Serial2.print("AT+SENDB=1,8,22,0,-------------------------");
					Serial2.print("End");
					Serial2.print("-------------------------\r\n");
				}
				Serial1.print("-------------------------");
				Serial1.print("End");
				Serial1.print("-------------------------");
			}

			inputBuffer = "";  // Reinitialize for the next line
			Buffer = "";
		}
	}
}

bool isJoined(void) {
	char c;
	String StatusBuffer = "";
	StatusBuffer.reserve(32);
	bool join = false;
	Serial2.print("AT+NJS?\r\n");

	uint32_t timeout = millis() + 5000;

	while (timeout > millis()) {
		if (Serial2.available()) {
			c = Serial2.read();

			StatusBuffer += c;

			if (c == '\n') {
				if (StatusBuffer.indexOf("NJS:1") != -1) {
					join = true;
					break;
				} else if (StatusBuffer.indexOf("NJS:0") != -1) {
					join = false;
					break;
				}

				StatusBuffer = "";
			}
		}
	}

	return join;


	// String readBuffer;
	// readBuffer.reserve(32);

	// Serial2.print("AT+NJS?\r\n");
	// Serial2.readStringUntil("\n");
}