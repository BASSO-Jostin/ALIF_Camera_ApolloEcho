HardwareSerial Serial2(PA3, PA2);  //Serial for AT Commands to send to the Satellite through the module EchoStar
HardwareSerial Serial1(PB7, PB6);

String inputBuffer = "";
String Buffer = "";
int packetNumber;

char Savebuffer[128];
char packet[255];
int indexBuffer = 0;

void setup() {
  // put your setup code here, to run once:

  Serial1.begin(115200);
  Serial2.begin(115200);

  Serial2.println("AT+JOIN\r\n");

  delay(5000);

  Serial1.println("Initialisation");



  // Serial2.println("AT+JOIN\r\n");
  // delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial1.available()) {
    char c = Serial1.read();
    //Serial1.write(c);

    inputBuffer += c;

    // Line by line checking
    if (c == '\n') {
      //inputBuffer.trim();
      Buffer += inputBuffer;

      if (inputBuffer.startsWith("test_ssdv thread: begin ") || inputBuffer.indexOf("test_ssdv thread: begin ") != -1) {
        packetNumber = 0;
        Serial1.println("-------------------------");
        Serial1.println("Début");
        Serial1.println("-------------------------");
      }

      // Check if the line is the success
      if (inputBuffer.startsWith("5566")) {
        //Serial2.println(packetNumber);
        //Serial2.println(Buffer);

        memset(packet, 0, 255);

        sprintf(packet, "AT+SEND=1,0,11,0,%s\r\n"); //Buffer.c_str());

        Serial2.println(packet);

        Serial1.println(packet);

        Serial1.println("-------------------------");

        packetNumber += 1;
      }

      if (inputBuffer.startsWith("test_ssdv thread: end ") || inputBuffer.indexOf("test_ssdv thread: end ") != -1) {
        packetNumber = 0;
        Serial1.println("-------------------------");
        Serial1.println("Fin");
        Serial1.println("-------------------------");
      }

      inputBuffer = "";  // Reinitialize for the next line
      Buffer = "";
    }
  }
}
