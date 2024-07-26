#include <soc/sens_reg.h>
#include <soc/rtc_cntl_reg.h>
// touch readings are controlled by the touch FSM (finite state machine)
// touch fsm can be triggerred by software or a dedicated hardware timer to conduct a measurement on a particular pin

//measurement involves the following steps

// 0. probably some setup
// 1. Touch FSM selects the touch sensor to be read
// 2. Touch FSM drives start signal, internally starts touch counter timer
// 3. Pulse counter in touch FSM will increment on each pulse received from sensor
// 4. When pulse counter reaches count threshold set in RTC_CNTL_TOUCH_MEAS_NUM,
//    the measurement is complete.

// output can be read from SENS_TOUCH_PADn_DATA

// SENS_TOUCH_DATA_SEL can set filtering or smth on output data

// if pulse counter does not reach threshold after timeout (RTC_CNTRL_TOUCH_TIMEOUT_NUM)
//  TOUCH_TIME_OUT_INT timeout interrupt will be triggered, indicating a circuit exception


// **** BEGIN HOW TO ACTUALLY DO THINGS *****

// TO configure start signal to be triggered by software
//    - set RTC_CNTL_TOUCH_START_FORCE
//    - once configured, setting RTC_CNTL_TOUCH_START_EN will start a measurement
int touch_pin = 2;

uint64_t out_value = 0;

void setup() {
  Serial.begin(115200);

  REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_FORCE, 1);  // software will start readings
  REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_EN, 0);     // clear reading start register
  REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_DATA_SEL, 0);          // raw data (no smoothing or benchmark)

  // REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_STATUS_CLR, 1); //clears all touch active status?

  // REG_SET_FIELD(SENS_SAR_TOUCH_CHN_ST_REG, SENS_TOUCH_CHANNEL_CLR, 1); // clears channel

  touchRead(1);
  touchRead(2);
  touchRead(3);
  touchRead(4);
  touchRead(5);
  touchRead(6);
  touchRead(7);
  touchRead(8);
  REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL1_REG, RTC_CNTL_TOUCH_MEAS_NUM, 1);  // only one reading
  REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL1_REG, RTC_CNTL_TOUCH_MEAS_NUM, 1);  // only one reading
  // REG_SET_FIELD(RTC_CNTL_TOUCH_TIMEOUT_CTRL_REG, RTC_CNTL_TOUCH_TIMEOUT_EN, 1);
  // REG_SET_FIELD(RTC_CNTL_TOUCH_TIMEOUT_CTRL_REG, RTC_CNTL_TOUCH_TIMEOUT_NUM, 3000);



  // REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_OUTEN, 1<<2);  // enables touch controller output i think this chooses channel? nope i dont think it does :~(
  // REG_SET_FIELD(RTC_CNTL_TOUCH_SCAN_CTRL_REG, RTC_CNTL_TOUCH_SCAN_PAD_MAP, 1<<3); // choose channel 2
}

void loop() {
  // REG_SET_FIELD(RTC_CNTL_TOUCH_SCAN_CTRL_REG, RTC_CNTL_TOUCH_SCAN_PAD_MAP, 1<<2); // choose channel 2

  REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_OUTEN, 1 << 2);




  REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_EN, 0);  // clear reading start register
  // REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_RESET, 0);  // reset the fsm
  REG_SET_FIELD(SENS_SAR_TOUCH_CHN_ST_REG, SENS_TOUCH_CHANNEL_CLR, 1);

  REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_EN, 1);  // begin a reading

  while (!REG_GET_FIELD(SENS_SAR_TOUCH_CHN_ST_REG, SENS_TOUCH_MEAS_DONE)) {
  }
  // delay(300);
  // if (REG_GET_FIELD(SENS_SAR_TOUCH_CHN_ST_REG, SENS_TOUCH_MEAS_DONE)) {
  out_value = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS2_REG, SENS_TOUCH_PAD2_DATA);
  Serial.println(out_value);

  // REG_SET_FIELD(RTC_CNTL_TOUCH_SCAN_CTRL_REG, RTC_CNTL_TOUCH_SCAN_PAD_MAP, 0); // clear!

  REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_OUTEN, 0);
  // } else {
  // Serial.println("n");
  // }
  delay(100);









  REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_OUTEN, 1 << 1);




  REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_EN, 0);  // clear reading start register
  REG_SET_FIELD(SENS_SAR_TOUCH_CHN_ST_REG, SENS_TOUCH_CHANNEL_CLR, 1);

  REG_SET_FIELD(RTC_CNTL_TOUCH_CTRL2_REG, RTC_CNTL_TOUCH_START_EN, 1);  // begin a reading

  while (!REG_GET_FIELD(SENS_SAR_TOUCH_CHN_ST_REG, SENS_TOUCH_MEAS_DONE)) {
  }
  // delay(300);
  // if (REG_GET_FIELD(SENS_SAR_TOUCH_CHN_ST_REG, SENS_TOUCH_MEAS_DONE)) {
  out_value = REG_GET_FIELD(SENS_SAR_TOUCH_STATUS1_REG, SENS_TOUCH_PAD1_DATA);
  Serial.println(out_value);

  // REG_SET_FIELD(RTC_CNTL_TOUCH_SCAN_CTRL_REG, RTC_CNTL_TOUCH_SCAN_PAD_MAP, 0); // clear!

  REG_SET_FIELD(SENS_SAR_TOUCH_CONF_REG, SENS_TOUCH_OUTEN, 0);
  // } else {
  // Serial.println("n");
  // }
  delay(100);
}
