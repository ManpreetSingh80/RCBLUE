#include <SoftwareSerial.h>
const int rxpin = 11; // pin used to receive (not used in this version) 
const int txpin = 4; // pin used to send to LCD
SoftwareSerial blue(rxpin, txpin); // new serial port on pins 2 and 3

int MotorAspeed, MotorBspeed, MotorSlack,moveState=0,d_speed,d_dir;
float prevA,prevB;

float yaw,input,out,setpoint,originalSetpoint,Buffer[3];
float yinput,yout;int ysetpoint, yoriginalSetpoint=0;

float bal_kp=34.45,bal_ki=45.44,bal_kd=45.23,rot_kp=1.234,rot_ki=2.234,rot_kd=3.234;
int addressFloat=0;
String content = "",di="D";

void setup() 
{
Serial.begin(9600); // start serial communication at 9600bps
blue.begin(9600);
blue.setTimeout(10);
}
void loop() 
{
bluetooth();
if(Serial.available()>0)        //for printing to serial monitor
{
  char outByte=Serial.read();
  blue.print(outByte);
  Serial.println(outByte);
}
  
}
void bluetooth()
{
if(blue.available())
  {
   content=blue.readString();
     if(content[0]=='F')            //forward
     {setpoint = originalSetpoint - d_speed;Serial.println(setpoint);}
     else if(content[0]=='B')      //backward
     {setpoint = originalSetpoint + d_speed;Serial.println(setpoint);}
     else if(content[0]=='L')      //left
     {ysetpoint = constrain((ysetpoint + yoriginalSetpoint - d_dir),-180,180);Serial.println(ysetpoint);}
     else if(content[0]=='R')        //right
     {ysetpoint = constrain(ysetpoint + yoriginalSetpoint + d_dir,-180,180);Serial.println(ysetpoint);}
     else if(content[0]=='S')        //speed slider
     {d_speed = (content.substring(2)).toInt();Serial.println(d_speed);}
     else if(content[0]=='D')        //direction slider
     {d_dir = content.substring(2).toInt();Serial.println(d_dir);}
    else if(content[0]=='P')
    {
     if(content[1]=='S')
    {
     if(content[2]=='B')
     save_pid(1);      //save balance pid value
     else
     save_pid(0);      //save rotation pid value
    } else if(content[1]=='C')
    {
     if(content[2]=='B')
    {
      change_pid(1);      //change balance pid value
    }
    else
    change_pid(0);        //change rotation pid value
    }
    }
    if(content=="updateb")
    return_pid(1);        //return balance pid values
    else if(content=="updater")
    return_pid(0);        //return rotation pid values
/*if(ysetpoint!=0)
{
  di="D";di.concat(ysetpoint);di.concat("e");blue.print(di);
Serial.println(di);}*/
  }
}
void return_pid(bool b)
{
  char charVal[10];
  String sent = "";
  if(b)                                                            //Balance PID value is returned to App
  {
  sent.concat("OP");dtostrf(bal_kp, 5, 3, charVal);sent.concat(charVal);
  sent.concat("OI");dtostrf(bal_ki, 5, 3, charVal);sent.concat(charVal);
  sent.concat("OD");dtostrf(bal_kd, 5, 3, charVal);sent.concat(charVal);
  sent.concat("e");
  Serial.println(sent);
  }
  else                                                                          //Rotation PID value is returned to app
  {
    sent.concat("OP");dtostrf(rot_kp, 5, 3, charVal);sent.concat(charVal);
  sent.concat("OI");dtostrf(rot_ki, 5, 3, charVal);sent.concat(charVal);
  sent.concat("OD");dtostrf(rot_kd, 5, 3, charVal);sent.concat(charVal);
  sent.concat("e");Serial.println(sent);
  }
 blue.print(sent);
}
void change_pid(bool b)
{
  blue.print("O");
  while(!blue.available());
  for(int i=0;i<3;i++)
  {
    Buffer[i]=blue.parseFloat();
  }
  if(b)                                                      //Balance PID changed
  {
    bal_kp=Buffer[0];bal_ki=Buffer[1],bal_kd=Buffer[2];
    Serial.print("Bkp");Serial.print(bal_kp);Serial.print("\t");
    Serial.print("Bki");Serial.print(bal_ki);Serial.print("\t");
    Serial.print("Bkd");Serial.print(bal_kd);Serial.println("\t");
  }
  else                                                        //Rotation PID changed
  {
   rot_kp=Buffer[0];rot_ki=Buffer[1],rot_kd=Buffer[2];
  Serial.print("Rkp");Serial.print(rot_kp);Serial.print("\t");
    Serial.print("Rki");Serial.print(rot_ki);Serial.print("\t");
    Serial.print("Rkd");Serial.print(rot_kd);Serial.println("\t"); 
  }
}
void save_pid(bool pid)  
{
  addressFloat = 0;
 if(pid)                            //Balance PID updated
 {
   Serial.println("Bupdated");
 }
 else
 {
   Serial.println("Rupdated");        //Rotation PID updated
 }
}
