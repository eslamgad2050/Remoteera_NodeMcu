void manual_control(){
 Serial.println("manual control");
    //Manual Switch Control
    if (digitalRead(SwitchPin1) == LOW){
      delay(200);
      if(toggleState_1 == 1){
        digitalWrite(RelayPin1, LOW); // turn on relay 1
        toggleState_1 = 0;
        client.publish(pub1, "0");
        Serial.println("Device1 ON");
        }
      else{
        digitalWrite(RelayPin1, HIGH); // turn off relay 1
        toggleState_1 = 1;
        client.publish(pub1, "1");
        Serial.println("Device1 OFF");
        }
     }
    else if (digitalRead(SwitchPin2) == LOW){
      delay(200);
      if(toggleState_2 == 1){
        digitalWrite(RelayPin2, LOW); // turn on relay 2
        toggleState_2 = 0;
        client.publish(pub2, "0");
        Serial.println("Device2 ON");
        }
      else{
        digitalWrite(RelayPin2, HIGH); // turn off relay 2
        toggleState_2 = 1;
        client.publish(pub2, "1");
        Serial.println("Device2 OFF");
        }
    }
    else if (digitalRead(SwitchPin3) == LOW){
      delay(200);
      if(toggleState_3 == 1){
        digitalWrite(RelayPin3, LOW); // turn on relay 3
        toggleState_3 = 0;
        client.publish(pub3, "0");
        Serial.println("Device3 ON");
        }
      else{
        digitalWrite(RelayPin3, HIGH); // turn off relay 3
        toggleState_3 = 1;
        client.publish(pub3, "1");
        Serial.println("Device3 OFF");
        }
    }
    else if (digitalRead(SwitchPin4) == LOW){
      delay(200);
      if(toggleState_4 == 1){
        digitalWrite(RelayPin4, LOW); // turn on relay 4
        toggleState_4 = 0;
        client.publish(pub4, "0");
        Serial.println("Device4 ON");
        }
      else{
        digitalWrite(RelayPin4, HIGH); // turn off relay 4
        toggleState_4 = 1;
        client.publish(pub4, "1");
        Serial.println("Device4 OFF");
        }
    }
    else if (digitalRead(SwitchPin5) == LOW){
      delay(200);
      if(toggleState_5 == 1){
        digitalWrite(RelayPin5, LOW); // turn on relay 4
        toggleState_5 = 0;
        client.publish(pub5, "0");
        Serial.println("Device5 ON");
        }
      else{
        digitalWrite(RelayPin5, HIGH); // turn off relay 4
        toggleState_5 = 1;
        client.publish(pub5, "1");
        Serial.println("Device5 OFF");
        }
    }
    delay(100);
}
