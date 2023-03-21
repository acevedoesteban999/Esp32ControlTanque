#include <ESP32Servo.h>
#include <WiFi.h>
#include <PID_v1.h>
//========================VARS========================

//=====WIFI=====
WiFiClient client;
const char* ssid     = "ESP32-Tanque";
const char* password = "123456789";
WiFiServer server(80);
String header;

//=====PI=====
double Setpoint, Input, Output;
double Kp=360.2611, Ki=16.0453, Kd=94.0629;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, REVERSE);

//=====SERVO=====
Servo myServo;
const int servoPin = 18;

//=====Sensor=====
const int sensorPin = 39;
const float  maxVoltage = 2.42;
const float  minVoltage = 1.7;
float sensorVoltage=0;
double sensorNivel=50;
double sensorNivelSetPoint;

//=====BOMBA=====
const int bombaPin = 14;

//=====VAR=====
int Mode = 2;

//=====WEB=====
String HeadPage =
  "<!DOCTYPE html>"
"  <html lang=\"es\">"
"  <head>"
"   <meta charset=\"UTF-8\">"
"   <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"  <link rel=\"icon\" href=\"data:,\">"
"<style>"
"        body {"
"            font-family: Helvetica;"
"            display: flex;"
"            justify-content:center;"
"            text-align: center;"
"        }"
"        .fieldset0{"
"            font-size:x-large;"
"            font-family:Arial, Helvetica, sans-serif;"
"            text-align: start;"
"        }"
"        .button1 {"
"            background-color: #3CAF50;"
"            border: none;"
"            color: white;"
"            padding: 10px 20px;"
"            text-decoration: none;"
"            font-size: 30px;"
"            margin: 15px;"
"            border-radius: 60px;"
"            cursor: pointer;"
"        }"
"        .continer {"
"            margin: 50px;"
"            align-items: center;"
"        }"
"        .button0 {"
"            background-color: #3CAF50;"
"            border: none;"
"            color: white;"
"            padding: 10px 20px;"
"            text-decoration: none;"
"            font-size: 30px;"
"            margin: 15px;"
"            border-radius: 60px;"
"            cursor: pointer;"
"        }"
"    </style>"
"   <title>ESP32 Tanque</title>"
"  </head>"
"  <body>";

String FooterPage =
"  </body>"
"     <script>"
"        function changeRange()"
"        {"
"           document.getElementById(\"idH2\").innerHTML=document.getElementById(\"range\").value"
"        }"
"        function AsynMode()"
"        {"
"           var a=document.getElementById(\"MdT0\").checked;"
"           var b=document.getElementById(\"MdT1\").checked;"

"           if (a)"
"                a=0;"
"           else if (b)"
"                a=1;"
"           else"
"                a=2;"
"           fetch"
"           ("
"               '/',"
"               {"
"                   method: 'POST',"
"                   headers: "
"                   {"
"                       'Accept': 'application/json',"
"                       'Content-Type': 'application/json'"
"                   },"
"                   body: JSON.stringify(\"AsynMode\"+ a)"
"               }"
"           )"
"           .then(response => response.json()"
"           )"
"           .then((data) =>"
"           {"
"                if(a==0)"
"                 {"
"                    document.getElementById(\"idDivValvulaBomba\").removeAttribute(\"style\");"
"                    document.getElementById(\"idDivValvula\").removeAttribute(\"style\");"
"                    document.getElementById(\"idDivBomba\").setAttribute(\"style\",\"display: none;\");  "
"                    document.getElementById(\"idDivRange\").setAttribute(\"style\",\"display: none;\");  "
"                    document.getElementById(\"idDivSetpoint\").setAttribute(\"style\",\"display: none;\");  "
"                 }"
"                else if(a==1)"
"                 {"
"                    document.getElementById(\"idDivSetpoint\").removeAttribute(\"style\");  "
"                    document.getElementById(\"idDivRange\").removeAttribute(\"style\");  "
"                    document.getElementById(\"idDivValvulaBomba\").setAttribute(\"style\",\"display: none;\");"
"                    document.getElementById(\"idDivBomba\").setAttribute(\"style\",\"display: none;\");  "
"                    document.getElementById(\"idDivValvula\").setAttribute(\"style\",\"display: none;\"); "
"                 }"
"                else"
"                 {"
"                    document.getElementById(\"idDivValvulaBomba\").removeAttribute(\"style\");"
"                    document.getElementById(\"idDivBomba\").removeAttribute(\"style\");  "
"                    document.getElementById(\"idDivValvula\").removeAttribute(\"style\");"
"                    document.getElementById(\"idDivSetpoint\").setAttribute(\"style\",\"display: none;\");  "
"                    document.getElementById(\"idDivRange\").setAttribute(\"style\",\"display: none;\");  "
"                 }"
"           }"
"           )"
"       }"
"        function AsynManual()"
"       {"
"           var a=document.getElementById(\"VS0\").checked;"
"           var a1=document.getElementById(\"VS1\").checked;"
"           var b=document.getElementById(\"B0\").checked;"
"           var c=document.getElementById(\"idInputServo\").value;"
"           if (a)"
"                a=1;"
"           else if (a1)"
"                a=0;"
"           else"
"                a=2;"
"            if (b)"
"                b=1;"
"           else"
"                b=0;"
"           fetch"
"           ("
"               '/',"
"               {"
"                   method: 'POST',"
"                   headers: "
"                   {"
"                       'Accept': 'application/json',"
"                       'Content-Type': 'application/json'"
"                   },"
"                   body: JSON.stringify(\"AsynValvula\"+a+\"AsynBomba\"+b + \"AsynServo\"+c)"
"               }"
"           )"
"      }"
"      function AsynRange()"
"          {"
"              var a=document.getElementById(\"range\").value;"
"              fetch"
"                ("
"                    '/',"
"                    {"
"                        method: 'POST',"
"                        headers: "
"                        {"
"                            'Accept': 'application/json',"
"                            'Content-Type': 'application/json'"
"                        },"
"                    body: JSON.stringify(\"AsynRange\"+a)"
"                    }"
"                )"
"          }"
"        function AsynUpdate()"
"       {"
"           fetch"
"           ("
"               '/',"
"               {"
"                   method: 'POST',"
"                   headers: "
"                   {"
"                       'Accept': 'application/json',"
"                       'Content-Type': 'application/json'"
"                   },"
"               body: JSON.stringify(\"AsynUpdate\")"
"               }"
"           )"
"           .then(response => response.json()"
"           )"
"           .then((data) =>"
"           {"
"              if(data.Modo==0)"
"                   document.getElementById(\"idSpan\").innerHTML=\"ON OFF\";"
"              else if(data.Modo==1)"
"                   document.getElementById(\"idSpan\").innerHTML=\"PID\";"
"              else"
"                   document.getElementById(\"idSpan\").innerHTML=\"Manual\";"
"              document.getElementById(\"idValvula\").innerHTML=data.Valvula+\"%\";"
"              document.getElementById(\"idNivel\").innerHTML=data.Nivel+\"%\";"      
"              document.getElementById(\"idSetpoint\").innerHTML=data.SetpointNivel+\"%\";"           
"            }"
"           )"
"       }"
"        setInterval('AsynUpdate()',2500);"
"     </script>"
  "</html>";

String BodyPage =
"<div class=\"continer\">"
"        <h2> Modo <span id=\"idSpan\">Conectando...</span> </h2>"
"        <hr>"
"        <fieldset class=\"fieldset0\">"
"            <legend>Modo de Trabajo</legend>"
"            <div> <input type=\"radio\" name=\"MdT\" id=\"MdT0\" value=\"0\"> <label for=\"MdT0\">Control ON-OFF</label> </div>"
"            <div> <input type=\"radio\" name=\"MdT\" id=\"MdT1\" value=\"1\"> <label for=\"MdT1\">Control PID</label> </div>"
"            <div> <input type=\"radio\" name=\"MdT\" id=\"MdT2\" value=\"2\" checked> <label for=\"MdT2\">Manual</label> </div>"
"        </fieldset> "
"        <button class=\"button1\" onclick=\"AsynMode()\">Aplicar</button>"
"        <hr>"
"        <h2> Válvula: </h2>"
"        <h3 id=\"idValvula\"> % </h3>"
"        <div id=\"idDivSetpoint\" style=\"display: none;\">"
"          <h2> Estabilizar: </h2>"
"          <h3  id=\"idSetpoint\"> % </h3>"
"        </div>"
"        <h2> Nivel: </h2>"
"        <h3 id=\"idNivel\"> % </h3>"
"        <hr>"
"        <div id=\"idDivValvulaBomba\">"
"           <div id=\"idDivValvula\">"
"               <fieldset class=\"fieldset0\">"
"                 <legend>Válvula de Salida</legend>"
"                 <div> "
"                     <input type=\"radio\"  name=\"VS\" id=\"VS0\"> <label for=\"VS0\">Abierta</label> "
"                 </div>"
"                 <div>"
"                     <input type=\"radio\" name=\"VS\" id=\"VS1\" checked > <label for=\"VS1\">Cerrada</label>"
"                     </div>"
"                 <div>"
"                     <input type=\"radio\" name=\"VS\" id=\"VS2\"> <label for=\"VS2\">Valor</label> "
"                 </div>"
"               <input type=\"number\" value=\"0\" id=\"idInputServo\">"
"             </fieldset>"
"           </div>"
"           <div id=\"idDivBomba\">"
"               <fieldset class=\"fieldset0\">"
"                  <legend>Bomba</legend>"
"                   <div> <input type=\"radio\" name=\"B\" id=\"B0\" > <label for=\"B0\">Encendida</label> </div>"
"                   <div> <input type=\"radio\" name=\"B\" id=\"B1\" checked> <label for=\"B1\">Apagada</label> </div>"
"               <hr>"
"               </fieldset>"
"           </div>"
"           <button class=\"button0\" onclick=\"AsynManual()\">Aplicar</button>"
"         </div>"
"        <div id=\"idDivRange\" style=\"display: none;\">"
"            <h2 >Valor a estabilizar:</h2><br />"
"            <input type=\"range\" ontouchmove=\"changeRange()\" onmousemove=\"changeRange()\" id=\"range\" list=\"markers\" value=\"0\" />"
"            <datalist id=\"markers\">"
"            <option value=\"0\"></option>"
"            <option value=\"25\"></option>"
"            <option value=\"50\"></option>"
"            <option value=\"75\"></option>"
"            <option value=\"100\"></option>"
"            </datalist>"
"           <h2 id=\"idH2\">0</h2>"
"           <button class=\"button0\" onclick=\"AsynRange()\">Aplicar</button>"
"        </div>"
"    </div>";

//========================SETUP========================
void setup() {
  Serial.begin(115200);
  pinMode(14, OUTPUT);
  
  myServo.attach(servoPin);
  myServo.write(30);
  
  digitalWrite(14,LOW);  
  
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  Setpoint=2.21;
  sensorNivelSetPoint=49.10;
  
  myPID.SetOutputLimits(30,110);
  myPID.SetMode(AUTOMATIC); 
  myPID.SetSampleTime(1000);
  myPID.SetTunings(Kp, Ki, Kd);

  server.begin();
}
void SendGetResponse()
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  client.println(HeadPage);
  client.println(BodyPage);
  client.println(FooterPage);
  client.println();
}
boolean isNumeric(String str) {
    unsigned int stringLength = str.length();
 
    if (stringLength == 0)
    {
        return false;
    }
 
    boolean seenDecimal = false;
 
    for(unsigned int i = 0; i < stringLength; ++i)
    {
        if (isDigit(str.charAt(i)))
        {
            continue;
        }
 
        if (str.charAt(i) == '.') 
        {
            if (seenDecimal) {
                return false;
            }
            seenDecimal = true;
            continue;
        }
        return false;
    }
    return true;
}
void WebProccess()
{
  client = server.available();
  if (client)
  {
    String currentLine = "";
    bool post = false;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        //Serial.write(c);
        header += c;
        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            if (header.indexOf("GET") >= 0)
            {
              SendGetResponse();
              break;
            }
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
      else
      {
        currentLine.replace("\"","");
        int index;
        String s;
        index = currentLine.indexOf("AsynUpdate");
        if ( index >= 0)
        {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();
          client.print("{");
          client.print("\"Valvula\":");
          float ValvulaToSend=myServo.read()*1.25 -35;
          if (ValvulaToSend > 100)
            ValvulaToSend=100;
          else if (ValvulaToSend < 0)
            ValvulaToSend=0;
          client.print(ValvulaToSend);
          client.print(",");
          client.print("\"Nivel\":");
          client.print(sensorNivel);
          client.print(",");
          client.print("\"Modo\":");
          client.print(Mode);
          client.print(",");
          client.print("\"SetpointNivel\":");
          client.print(sensorNivelSetPoint);
          client.print("}");

          client.println();
          break;
        }
        index = currentLine.indexOf("AsynRange");
        if ( index >= 0)
        {
          s = currentLine.substring(index + 9);
          if (isNumeric(s))
          {
            double PorcientoFloat=s.toFloat();
            double volTage=0.000000000038937*pow(PorcientoFloat,6) - 0.000000012597811*pow(PorcientoFloat,5) + 0.000001576926928*pow(PorcientoFloat,4) - 0.000095193713924*pow(PorcientoFloat,3) + 0.002736659129198*pow(PorcientoFloat,2) - 0.022600786338444*PorcientoFloat + 1.868594314139950;
  
            if(volTage <  minVoltage)
               Setpoint=minVoltage;
            else if(volTage > maxVoltage)
               Setpoint=maxVoltage;
            else
            {
               Setpoint=volTage;
               sensorNivelSetPoint = 66.293 * pow(volTage,4) - 357.4 * pow(volTage,3) + 693.94 * pow(volTage,2) - 513.02 * volTage + 70.24;
            }
          } 
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();
          client.print("{");
          client.print("}");
          client.println();
          break;
        }
        index = currentLine.indexOf("AsynMode");
        if ( index >= 0)
        {
          s = currentLine.substring(index + 8,index + 9);
          if (isNumeric(s))
          {
            if(s[0]=='0')
            {
              Mode=0; 
            }
            else if (s[0]=='1')
            {
              Mode=1;
            }
            else if (s[0]=='2')
            {
              Mode=2;
              myServo.write(30);
              digitalWrite(bombaPin,LOW);
            }
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.print("{");
            client.print("}");
            client.println();
            break;
          } 
        }
        index = currentLine.indexOf("AsynValvula");
        if ( index >= 0)
        {
          s = currentLine.substring(index + 11,index + 12);
          if (isNumeric(s))
          {
            if(s[0]=='0')
            {
              myServo.write(30);
            }
            else if (s[0]=='1')
            {
              myServo.write(110);
            }
            else if (s[0]=='2')
            {
              index = currentLine.indexOf("AsynServo");
              if ( index >= 0)
              {
                 s = currentLine.substring(index + 9);
                 if (isNumeric(s))
                 {
                     myServo.write(s.toInt());
                 }
              }
            }
          }
          if(Mode==2)
          {
            index = currentLine.indexOf("AsynBomba");
            if ( index >= 0)
            {
              s = currentLine.substring(index + 9,index + 10);
              if (isNumeric(s))
              {
                if(s[0]=='0')
                {
                  digitalWrite(bombaPin,LOW);
                }
                else if (s[0]=='1')
                {
                  digitalWrite(bombaPin,HIGH);
                }
              }
            }
          }  
        }
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();
        client.print("{");
        client.print("}");
        client.println();
        break;
      }
    }
    header = "";
    client.stop();
  }
}
void SensorProccess()
{
  sensorVoltage = analogRead(sensorPin) * 3.3 / 4095;
  
  if(sensorVoltage <  minVoltage)
     sensorNivel=0;
  else if(sensorVoltage > maxVoltage)
     sensorNivel=100;
  else
     sensorNivel = 66.293 * pow(sensorVoltage,4) - 357.4 * pow(sensorVoltage,3) + 693.94 * pow(sensorVoltage,2) - 513.02 * sensorVoltage + 70.24;
     
  if(Mode==0)
  { 
    if(sensorVoltage <  maxVoltage)
      digitalWrite(bombaPin,HIGH);
  }
  else if(Mode == 1)
  {
      if(sensorVoltage <  maxVoltage)
        digitalWrite(bombaPin,HIGH);
      Input=sensorVoltage;
      myPID.Compute();
      myServo.write(Output);
  }
  if(sensorVoltage >  maxVoltage)
    digitalWrite(bombaPin,LOW);
}
//========================LOOP========================
void loop()
{
  WebProccess();
  SensorProccess();
  Serial.println("==============================");
  Serial.print("Modo:");
  Serial.println(Mode);
  Serial.print("Voltage:");
  Serial.println(sensorVoltage);
  Serial.print("Nivel:");
  Serial.println(sensorNivel);
  Serial.print("Voltage Referencia PID:");
  Serial.println(Setpoint);
  Serial.print("Nivel Referencia PID:");
  Serial.println(sensorNivelSetPoint);
  Serial.print("Valvula Fisica:");
  Serial.println(myServo.read());
  Serial.print("Valvula:");
  Serial.println(myServo.read()*1.25 -35);
  Serial.print("Bomba:");
  Serial.println(digitalRead(bombaPin));
    
  delay(1000);


}
