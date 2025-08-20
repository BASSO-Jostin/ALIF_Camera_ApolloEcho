HardwareSerial Serial2(PA3, PA2);  //Serial for AT Commands to send to the Satellite through the module EchoStar
HardwareSerial Serial1(PB7, PB6);

String inputBuffer = "";
String SaveBuffer = "";
int packetNumber;

void setup() {
  // put your setup code here, to run once:

  Serial1.begin(115200);
  Serial2.begin(115200);

  Serial2.println("Initialisation");
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial1.available()) {
    char c = Serial1.read();
    Serial2.write(c);

    inputBuffer += c;

    // Line by line checking
    if (c == '\n') {
      //Serial2.println(inputBuffer);
      inputBuffer.trim();

      // Check if the line is the success
      if (inputBuffer.startsWith("--> Sending SSDV packet")) {-->
        SaveBuffer = inputBuffer;

        int hashIndex = inputBuffer.indexOf('#');
        if (hashIndex > 0) {
          packetNumber = inputBuffer.substring(hashIndex + 1).toInt();

          Serial2.println("-----------------------------------------------------------------------------------------------------------------");
          Serial2.println(packetNumber);
          Serial2.println("-----------------------------------------------------------------------------------------------------------------");
        }

        // Vérifie si la ligne indique un échec
        /*if (inputBuffer.indexOf("NOT_SENT:1") != -1 || inputBuffer.indexOf("ERROR") != -1) {

        return;
      }*/

        inputBuffer = "";  // Reinitialize for the next line
        SaveBuffer = "";
      }
    }
  }
}