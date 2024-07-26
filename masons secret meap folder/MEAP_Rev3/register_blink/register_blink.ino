int led_pin = 36; // but what gpio is it? also 36 eek i think!

void setup() {
  REG_SET_FIELD(GPIO_FUNC36_OUT_SEL_CFG_REG, GPIO_FUNC36_OUT_SEL, 0x100); // specifies output to be controlled by output register
  REG_WRITE(GPIO_ENABLE1_W1TS_REG, 1 << 4); // enables the output
}

void loop() {
REG_WRITE(GPIO_OUT1_W1TC_REG, 1 << 4); // clears register
delay(500);
REG_WRITE(GPIO_OUT1_W1TS_REG, 1 << 4); // sets register
delay(500);

}
