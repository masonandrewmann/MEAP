#include <soc/sens_reg.h>

int input_pin = 9; // gpio #9 = SAR ADC1 channel #8

int channel_num = 8;

int channels[] = {8, 9};
int current_channel = 0;


//confused that some fields in this register refer to SAR ADC and some to RTC ADC
// rtc definitely refers to single conversion mode

void setup() {
  Serial.begin(115200);
  // REG_SET_FIELD(GPIO_FUNC36_OUT_SEL_CFG_REG, GPIO_FUNC36_OUT_SEL, 0x100); // specifies output to be controlled by output register
  
  // analogRead(9);
  REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_FORCE, 1); // adc controlled by software
  REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_SAR1_EN_PAD_FORCE, 1); // channel select controlled by software

  REG_WRITE(SENS_SAR_ATTEN1_REG, 0xFFFFFFFF);
}

void loop() {
  REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_SAR1_EN_PAD, 1<<channels[current_channel]); // select channel
  REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_SAR, 0); // reset conversion register
  REG_SET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_SAR, 1); // start a conversion
  delay(20); // wait
  if(REG_GET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_DONE_SAR) == 1){ // is the conversion done
    Serial.print(channels[current_channel]+1);
    Serial.print(" ");
    Serial.println(REG_GET_FIELD(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_DATA_SAR)); // data from adc1!
  }
  delay(20);

  if(current_channel == 0){
    current_channel = 1;
  } else {
    current_channel = 0;
  }
}
