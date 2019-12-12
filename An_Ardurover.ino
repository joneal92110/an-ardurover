/*
Project: an-arduover
12/12/2019
*/
//Uses the Automation library at: https://github.com/tinkerspy/Automaton/wiki/The-command-machine
//http://tinyurl.com/ptj1212b
#include <Automaton.h>

char cmd_buffer[80];
Atm_command cmd;

enum {CMD_MOTOR, CMD_MSLEEP, CMD_RUN, CMD_STOP, CMD_DEL, CMD_VERSION, CMD_HELP};
const char cmdlist[] = 
  "motor msleep run stop del version help";

static String pubstrCmds[30];
static int pubargumennt1[30];
static int pubargumennt2[30];
static int pubrunFlag = 0;

void robotCommand()
{
    char tempb[20];
    int i = 0;
    while (pubstrCmds[i] != "RUN")
    {
      if(pubstrCmds[i] == "MOTOR")
      {
          if(pubargumennt1[i] == 1)
          {
            if(pubargumennt2[i] > 0)
            {
                 sprintf(tempb, "Motor 1 forward %i.", pubargumennt2[i]);
                 digitalWrite(2,HIGH);
                 digitalWrite(4,LOW);
                 analogWrite(3,pubargumennt2[i]+80);
            }
            else if(pubargumennt2[i] == 0)
            {
                 sprintf(tempb, "Motor 1 stopped.");
                 digitalWrite(2,LOW);
                 digitalWrite(4,LOW);
            }
            else if(pubargumennt2[i] < 0)
            {
                 sprintf(tempb, "Motor 1 backwards %i.", pubargumennt2[i]);
                 digitalWrite(2,LOW);
                 digitalWrite(4,HIGH);
                 analogWrite(3,pubargumennt2[i]+80);              
            }
            else
            sprintf(tempb, "You did not set a speed for motor $i.", pubargumennt1[i]);
            Serial.println(tempb);            
          }
          else if(pubargumennt1[i] == 2)
          {
            if(pubargumennt2[i] > 0)
            {
                 sprintf(tempb, "Motor 2 forward %i.", pubargumennt2[i]);
                 digitalWrite(7,HIGH);
                 digitalWrite(8,LOW);
                 analogWrite(5,pubargumennt2[i]+80);
            }
            else if(pubargumennt2[i] == 0)
            {
                 sprintf(tempb, "Motor 2 stopped");
                 digitalWrite(7,LOW);
                 digitalWrite(8,LOW);
            }
            else if(pubargumennt2[i] < 0)
            {
                 sprintf(tempb, "Motor 2 backwards %i.", pubargumennt2[i]);
                 digitalWrite(7,LOW);
                 digitalWrite(8,HIGH);
                 analogWrite(5,pubargumennt2[i]+80);              
            }
            else
            sprintf(tempb, "You did not set a speed for motor %i.", pubargumennt1[i]);
            Serial.println(tempb);                
          }
          else
          {
              Serial.println("Error: You did not select motor 1 or motor 2");
          }
      }
      
    
      if(pubstrCmds[i] == "MSLEEP")
      {
          sprintf(tempb, "Robot will sleep for %i ms...", pubargumennt1[i], pubargumennt2[i]);
          Serial.print(tempb);
          delay(pubargumennt1[i]);
          Serial.println("...");
          analogWrite(5,0);              
          analogWrite(3,0);
          delay(250);              
          
      }
      i++;
    }
    delay(600);
    pinMode(12, OUTPUT);
    digitalWrite(12, LOW);
}

void setCommand(char cmdName[30] = "null", int arg1 = 0, int arg2 = 0)
{
      if(pubrunFlag >= 25)
        Serial.println("Running low on memory... (28 lines MAX!)\n\r");

      if(pubrunFlag >= 29)
      {
        Serial.println("Max lines reached.\n\rRunning...");
        setCommand("RUN", 0, 0);
        pubrunFlag = -1;
        return;
      }
      pubstrCmds[pubrunFlag] = cmdName;
      pubargumennt1[pubrunFlag] = arg1;
      pubargumennt2[pubrunFlag] = arg2; 
      char tempb[30];      
      if(cmdName == "MOTOR")
      {
          sprintf(tempb, "Line %i: motor %i %i", pubrunFlag, arg1, arg2);
      }
      else if(cmdName == "MSLEEP")
      {
           sprintf(tempb, "Line %i: msleep %i", pubrunFlag, arg1);
      }
      Serial.println(tempb);

      pubrunFlag++;
      return;
}
void cmd_callback( int idx, int v, int up ) {

  Serial.write("\r\n>> ");
  char tempb[20];
  int ag2 = atoi( cmd.arg( 1 ) );
  int ag3 = atoi( cmd.arg( 2 ) );

  switch ( v ) {

    case CMD_MOTOR:
      setCommand("MOTOR", ag2, ag3);
      break;
    
    case CMD_MSLEEP:
      if(ag2 > 10000 || ag2 < 0)
      {
          Serial.write("msleep is too large a number. Needs to be less than 10,000 ms.\r\n");
      }
      if(ag2 < 10000)// || ag2 > 0)
      {      
      setCommand("MSLEEP", ag2, ag3);
      }
      break;
           
    case CMD_RUN:
      Serial.write("> Running commands...\n\r");
      setCommand("RUN", 0, 0);
      pubrunFlag = -1;
      break;
            
    case CMD_STOP:
      Serial.write("> Stopping robot\n\r");
      delay(600);
      pinMode(12, OUTPUT);
      digitalWrite(12, LOW);
      break;
      
    case CMD_DEL:
    sprintf(tempb, "Line %i deleted.", ag2);
    Serial.print(tempb);
    pubstrCmds[ag2] = "";
    pubargumennt1[ag2] = 0;
    pubargumennt2[ag2] = 0;
    break;

    case CMD_VERSION:
    Serial.write("Your robot needs an update.\n\r");
    delay (600);
    break;

    case CMD_HELP:
    Serial.write("\n\r\tView the full manual online at: www.github.com/joneal92110/an-ardurover/wiki/manual\n\r");
    delay (600);
    break;

    default:
    Serial.write("Error\n\r");
    break;
  }
}

void setup() {
  pinMode(12, INPUT);
  digitalWrite(12, LOW);
  Serial.begin( 9600 );
  delay(300);
  Serial.write("AT+C012\n\r"); //channel 3
  delay(300);
  Serial.write("AT+P2\n\r"); //TX power 3
  delay(300);
  Serial.write("Rosa Parks Robotics Team\n\r\t-- An Ardurover --\n\r");
  Serial.write("Pam Pham - Tim Jepson - Jeremiah O'Neal\n\r");
  Serial.write("- Kevin Siemens - Ryan Chafe -\n\r");
  Serial.write("Source code: http://tinyurl.com/ptj1212b \n\r\r\n");
  Serial.write("Clearing commands. Please wait...\r\n");
  for(int i = 0; i <= 30; i++)
  {
    Serial.write(".");
    pubstrCmds[i] = "";
    pubargumennt1[i] = 0;
    pubargumennt2[i] = 0;
  }    
  Serial.println("\n\rType help for help.\r\nReady.\n\r");
  cmd.begin( Serial, cmd_buffer, sizeof( cmd_buffer ) )
    .list( cmdlist )
    .onCommand( cmd_callback );
}

void loop() {
  if(pubrunFlag == -1)
  {    
    robotCommand();  
  }
  if(pubrunFlag > -1)
  {
    automaton.run();
  }
}
