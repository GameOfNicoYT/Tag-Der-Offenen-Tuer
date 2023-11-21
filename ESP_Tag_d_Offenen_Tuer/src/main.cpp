#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebSocketsServer.h>

// Replace with your network credentials
const char *ssid = "ESP_Sensor_Board";
const char *password = "HTL_TDOT_2023!";

// Set web server port number to 80
WebSocketsServer webSocket = WebSocketsServer(81);
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Creating Network: ");
  Serial.println(ssid);

  WiFi.softAP(ssid, password);

  Serial.println("Zugangspunkt gestartet");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.softAPIP());

  server.begin();
}


void loop()
{
  WiFiClient client = server.available(); // Listen for incoming clients

  if (Serial.available())
  {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, Serial);

    if (!error)
    {
      const char *sensor = doc["sensor"]; // GETSTRING
      long time = doc["time"];            // GETNUMBER
    }
  }

  if (client)
  { // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    { // loop while the client's connected
      currentTime = millis();
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        header += c;
        if (c == '\n')
        { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=UTF-8");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /formatedData") >= 0)
            {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
            }
            else if (header.indexOf("GET /rawData") >= 0)
            {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            }
            else if (header.indexOf("GET /snakeGame") >= 0)
            {
              // SNAKE GAME

              client.println("<style>");
              client.println("body {");
              client.println("overflow: hidden;");
              client.println("display: flex;");
              client.println("justify-content: center;");
              client.println("align-items: center;");
              client.println("flex-direction: column;");
              client.println("height: 100vh;");
              client.println("background-color: #f0f0f0;");
              client.println("}");
              client.println("");
              client.println("canvas {");
              client.println("border: 1px solid black;");
              client.println("}");
              client.println("</style>");
              client.println("");
              client.println("<body>");
              client.println("<p id=\"currentScore\"></p>");
              client.println("<p id=\"highScore\"></p>");
              client.println("<canvas id=\"gameCanvas\" width=\"400\" height=\"400\"></canvas>");
              client.println("");
              client.println("<script>");
              client.println("let currentScore = document.getElementById(\"currentScore\");");
              client.println("let highscore = document.getElementById(\"highScore\");");
              client.println("let snakeParts = [];");
              client.println("let tailLength = 0; // Startlänge der Schlange");
              client.println("const canvas = document.getElementById(\"gameCanvas\");");
              client.println("const ctx = canvas.getContext(\"2d\");");
              client.println("");
              client.println("let speed = 7;");
              client.println("let tileCount = 20;");
              client.println("let tileSize = canvas.width / tileCount - 2;");
              client.println("let headX = 4;");
              client.println("let headY = 10;");
              client.println("let appleX = 10;");
              client.println("let appleY = 10;");
              client.println("let xVelocity = 1;");
              client.println("let yVelocity = 0;");
              client.println("");
              client.println("function drawGame() {");
              client.println("currentScore.innerHTML = \"Score: \" + tailLength;");
              client.println("if (localStorage.getItem(\"Score\") != 0) {");
              client.println("highscore.innerHTML = \"Highscore: \" + localStorage.getItem(\"Score\");");
              client.println("} else {");
              client.println("highscore.innerHTML = \"Highscore: Noch nicht aufgestellt!\";");
              client.println("}");
              client.println("changeSnakePosition();");
              client.println("let result = isGameOver();");
              client.println("if (result) {");
              client.println("return;");
              client.println("}");
              client.println("");
              client.println("clearScreen();");
              client.println("checkAppleCollision();");
              client.println("drawApple();");
              client.println("drawSnake();");
              client.println("setTimeout(drawGame, 1000 / speed);");
              client.println("}");
              client.println("");
              client.println("function isGameOver() {");
              client.println("// Spiellogik für Game Over");
              client.println("if (");
              client.println("headX < 0 ||");
              client.println("headX === tileCount ||");
              client.println("headY < 0 ||");
              client.println("headY === tileCount");
              client.println(") {");
              client.println("restartGame();");
              client.println("return true;");
              client.println("}");
              client.println("for (let part of snakeParts) {");
              client.println("if (part.x === headX && part.y === headY) {");
              client.println("restartGame();");
              client.println("return true;");
              client.println("}");
              client.println("}");
              client.println("return false;");
              client.println("}");
              client.println("");
              client.println("function clearScreen() {");
              client.println("ctx.fillStyle = \"white\";");
              client.println("ctx.fillRect(0, 0, canvas.width, canvas.height);");
              client.println("}");
              client.println("");
              client.println("function drawSnake() {");
              client.println("ctx.fillStyle = \"green\";");
              client.println("for (let i = 0; i < snakeParts.length; i++) {");
              client.println("let part = snakeParts[i];");
              client.println("ctx.fillRect(");
              client.println("part.x * tileCount,");
              client.println("part.y * tileCount,");
              client.println("tileSize,");
              client.println("tileSize");
              client.println(");");
              client.println("}");
              client.println("");
              client.println("snakeParts.push({ x: headX, y: headY });");
              client.println("while (snakeParts.length > tailLength) {");
              client.println("snakeParts.shift();");
              client.println("}");
              client.println("");
              client.println("// Zeichne den Kopf der Schlange");
              client.println("ctx.fillStyle = \"green\";");
              client.println("ctx.fillRect(headX * tileCount, headY * tileCount, tileSize, tileSize);");
              client.println("}");
              client.println("");
              client.println("function changeSnakePosition() {");
              client.println("headX += xVelocity;");
              client.println("headY += yVelocity;");
              client.println("}");
              client.println("");
              client.println("function drawApple() {");
              client.println("ctx.fillStyle = \"red\";");
              client.println("ctx.fillRect(appleX * tileCount, appleY * tileCount, tileSize, tileSize);");
              client.println("}");
              client.println("");
              client.println("function checkAppleCollision() {");
              client.println("if (appleX === headX && appleY === headY) {");
              client.println("appleX = Math.floor(Math.random() * tileCount);");
              client.println("appleY = Math.floor(Math.random() * tileCount);");
              client.println("tailLength++;");
              client.println("}");
              client.println("}");
              client.println("");
              client.println("document.body.addEventListener(\"keydown\", keyDown);");
              client.println("");
              client.println("function keyDown(event) {");
              client.println("if (event.keyCode == 38 && yVelocity != 1) {");
              client.println("yVelocity = -1;");
              client.println("xVelocity = 0;");
              client.println("}");
              client.println("");
              client.println("if (event.keyCode == 40 && yVelocity != -1) {");
              client.println("yVelocity = 1;");
              client.println("xVelocity = 0;");
              client.println("}");
              client.println("");
              client.println("if (event.keyCode == 37 && xVelocity != 1) {");
              client.println("xVelocity = -1;");
              client.println("yVelocity = 0;");
              client.println("}");
              client.println("");
              client.println("if (event.keyCode == 39 && xVelocity != -1) {");
              client.println("xVelocity = 1;");
              client.println("yVelocity = 0;");
              client.println("}");
              client.println("}");
              client.println("");
              client.println("function restartGame() {");
              client.println("if (parseInt(localStorage.getItem(\"Score\")) <= tailLength) {");
              client.println("localStorage.setItem(\"Score\", tailLength);");
              client.println("}");
              client.println("");
              client.println("snakeParts = [];");
              client.println("tailLength = 0;");
              client.println("speed = 7;");
              client.println("tileCount = 20;");
              client.println("tileSize = canvas.width / tileCount - 2;");
              client.println("headX = 4;");
              client.println("headY = 10;");
              client.println("appleX = 10;");
              client.println("appleY = 10;");
              client.println("xVelocity = 1;");
              client.println("yVelocity = 0;");
              client.println("");
              client.println("drawGame(); // Spiel neu starten");
              client.println("}");
              client.println("");
              client.println("drawGame();");
              client.println("</script>");
              client.println("</body>");

              // END
            }
            else
            {

              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<meta charset=\"UTF-8\">");
              // CSS to style the on/off buttons
              // Feel free to change the background-color and font-size attributes to fit your preferences

              // Web Page Heading
              client.println("<style>");
              client.println("body {");
              client.println("width: 100vw;");
              client.println("height: 100vh;");
              client.println("overflow: hidden;");
              client.println("display: flex;");
              client.println("justify-content: center;");
              client.println("align-items: center;");
              client.println("flex-direction: column;");
              client.println("}");
              client.println("</style>");
              client.println("");
              client.println("<body>");
              client.println("<h1>ESP32 - Sensor Projekt</h1>");
              client.println("<h2>Tag der Offenen Tür 2023</h2>");
              client.println("");
              client.println("<div class=\"buttonContainer\">");
              client.println("<a href=\"/snakeGame\"><button>Snake Spiel</button></a>");
              client.println("<a href=\"/rawData\"><button>Rohdaten</button></a>");
              client.println("<a href=\"/formatedData\"><button>Formatierten Daten</button></a>");
              client.println("</div>");
              client.println("</body>");
            }

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          }
          else
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
