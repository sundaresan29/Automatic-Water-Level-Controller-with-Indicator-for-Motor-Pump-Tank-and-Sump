/* NAME       : Sundaresan S
   CODE       : Automatic Water Level Controller with Indicator for Motor Pump Tank and Sump
   Descreption: this code is for controle water tank and sump on both with and without server here esp 32 act as a local server in server mode 
*/
// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "NSIC-TSC Motor Server";
const char* password = "XXXXXXXXXXXXXXX";

const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";
//sensors
int s1;
int s2;
int s3;
int s4;
//sensors

int rsw;//reset button
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Motor Server Control Mode</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>Motor Server Control Mode</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  xhr.send();
}
</script>
<b><br></b>
<small>Turn ON and OFF the Reset Button Onse then Turn OFF/Change the Wifi Connection Immediately To Exit from the Server.</small>
<b><br></b>
<b>NOTE :<br><small> Do not Turn Off / Change Wifi Connection Directly To Exit from the Server</small></b>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons += "<h4>Output - Tank Motor</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(2) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Output - Sump Motor</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"4\" " + outputState(4) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Output - Reset Button</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"15\" " + outputState(15) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

String outputState(int output){
  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  pinMode(18, OUTPUT);
  digitalWrite(18, LOW);
  pinMode(19, OUTPUT);
  digitalWrite(19, LOW);
  pinMode(21, OUTPUT);
  digitalWrite(21, LOW);
  pinMode(22, OUTPUT);
  digitalWrite(22, LOW);
  pinMode(23, OUTPUT);
  digitalWrite(15, LOW); 
  pinMode(15, OUTPUT);
  digitalWrite(23, LOW);   
  pinMode(34,INPUT);
  pinMode(35,INPUT);
  pinMode(32,INPUT);
  pinMode(33,INPUT);
  pinMode(14, INPUT);
  
  // Connect to Wi-Fi
    
  WiFi.softAP(ssid, password);
    
//  WiFi.softAP(ssid, password);
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(1000);
//    Serial.println("Connecting to WiFi..");
//  }

  // Print ESP Local IP Address
//  Serial.println(WiFi.localIP());
 while(WiFi.softAPgetStationNum()<=0)
    {
      motorcontrole_without_server();// tank and sump motor controle without server 
      Tank_motor_LEDS(); //led indicators for tank
      Sump_motor_LEDS();//led indicators for sump
      resetbutton();
    }
    motorcontrole_with_server();// tank and sump motor controle over the server 
 



//Start server
  server.begin();
}

void loop() {
resetbutton();// reset and exit from the server
  if(WiFi.softAPgetStationNum()>=1)// wifi connection status
  {
   digitalWrite(21, LOW);
   digitalWrite(23, LOW);
   digitalWrite(22, LOW);
   digitalWrite(19, LOW);
   digitalWrite(15,HIGH);
   delay(500);
   digitalWrite(15,LOW);
   delay(500); 
  }
  else
  {
   digitalWrite(15,LOW);
   ESP.restart();  //exit from the server if no wifi connection
  }
}
void motorcontrole_without_server()
{
   s1=digitalRead(34);//sensor 1(float switch 1)
   s2=digitalRead(35);//sensor 2(float switch 2)
   s3=digitalRead(32);//sensor 3(float switch 3)
   s4=digitalRead(33);//sensor 4(float switch 4)
  
if((s1==HIGH&&s2==HIGH)&&(s3==LOW||s4==LOW))// Tank motor control
  {
    digitalWrite(2,HIGH);//Tank motor on
    
    
  }
   
  else if((s1==LOW&&s2==LOW)/*&&(s3==LOW&&s4==LOW))*/)// Tank motor control
  {
    digitalWrite(2,LOW);
    
  }
  if(s3==HIGH&&s4==HIGH)// Sump motor control
  {
    digitalWrite(4,HIGH);
    digitalWrite(2,LOW);
    Serial.println("Hi");
    
  }
     else if(s3==LOW&&s4==LOW)// Sump motor control
  {
    digitalWrite(4,LOW);
    Serial.println("H0");
  }
  if((s1==LOW||s2==LOW))
  {
    digitalWrite(19,HIGH);
  }
  else{
    digitalWrite(19,LOW);
  }

  
}
void motorcontrole_with_server()
{
   
   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
if((inputMessage1.toInt()== 15)&&inputMessage2.toInt()==1)//reset from the server
{

Serial.println("Restarting in 30 seconds");
 
  delay(30000);
 
  ESP.restart(); //reset from the server
}
  
    Serial.print("GPIO: ");
    Serial.print(inputMessage1);
    Serial.print(" - Set to: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });
}
void Tank_motor_LEDS()
{ 
  if (s1==HIGH&&s2==HIGH)// indicates tank water level is low
  {
    digitalWrite(21, HIGH);
    digitalWrite(19, LOW );
  }
  else if(s1==HIGH || s2==HIGH) // indicates tank water level 1 status and motor filling status by blinking
  {
     digitalWrite(19, HIGH);
     delay(1000);
     digitalWrite(19, LOW);
     delay(1000);
  }
  else if(s1==LOW&&s2==LOW)// indicates tank water level 2 an 1 is full
  {
    digitalWrite(21, LOW);
    digitalWrite(19, HIGH);
  }
  
 
}
void Sump_motor_LEDS()
{
    if (s3==HIGH&&s4==HIGH)// indicates Sump water level is low
  {
    digitalWrite(23, HIGH);
    digitalWrite(22, LOW );
  }
  else if(s3==HIGH || s4==HIGH) // indicates Sump water level 1 status and motor filling status by blinking
  {
     digitalWrite(22, HIGH);
     delay(1000);
     digitalWrite(22, LOW);
     delay(1000);
  }
  else if(s3==LOW&&s4==LOW)// indicates Sump water level 2 an 1 is full
  {
    digitalWrite(23, LOW);
    digitalWrite(22, HIGH);
  }
}
void resetbutton()
{
 rsw=digitalRead(14);// reset switch 
 if (rsw == HIGH)// reset the ESP 32 boart or exit from the server 
 {
    digitalWrite(15,HIGH);//indicates board reset execution
    Serial.println("Restarting in 10 seconds");
    delay(10000);
    ESP.restart();//reset function
    
 }
 else
 {
  digitalWrite(15,LOW);//indicates board reset compleet
 }
}
