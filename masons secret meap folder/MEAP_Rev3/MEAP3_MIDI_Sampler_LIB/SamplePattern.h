#ifndef SAMPLEPATTERN_H_
#define SAMPLEPATTERN_H_


/*
  on creation: choose number of steps, number of repeats, sample number
  array:
    midi note number
    step duration
    note sustain
    amplitude
*/
class SamplePattern {
public:
  uint8_t *midi_notes;
  uint16_t *step_duration;
  uint16_t *note_sustain;
  uint16_t *amplitude;  // not implemented yet
  int16_t num_steps;
  int16_t num_repeats;
  int16_t curr_step;
  int16_t remaining_repeats;
  int16_t sample_num;
  uint32_t next_step_time;
  int8_t channel_num;  // note implemented yet
  uint16_t sclock = 0;
  bool active;
  bool step_flag;
  bool start_flag;

  /**
   * @brief Constructor
   *
   * @param num_steps how many steps in the pattern
   * @param num_repeats how many times to repeat the pattern per trigger
   * @param sample_num num of the sample to play
   */
  SamplePattern(uint16_t _num_steps, uint16_t _num_repeats, int16_t _sample_num) {
    // Constructor
    // midi_notes = new uint8_t[num_steps];
    // step_duration = new uint16_t[num_steps];
    // note_sustain = new uint16_t[num_steps];
    // amplitude = new uint16_t[num_steps];

    curr_step = 0;
    next_step_time = 0;
    channel_num = -1;  // not implemented yet but for midi output
    active = false;
    step_flag = false;
    start_flag = false;
    sample_num = _sample_num;
    num_steps = _num_steps;
    num_repeats = _num_repeats - 1;  // eg if _num_repeats is 2, we want pattern to play 2 total times, not repeat twice after first play
    remaining_repeats = num_repeats;
  }

  /**
   * @brief Checks time to see if pattern needs to move on and executes appropriate methods
   */
  void cycle() {
    // check if we need to move to a new step
    if (active && !start_flag) {
      if (sclock >= next_step_time) {
        step_flag = true;
      }
    }
    // uint32_t curr_time = millis();
    // if (active) {
    //   if (curr_time > next_step_time) {  // do we need to trigger a new step
    //     Serial.println("stepflag true");
    //     step_flag = true;
    //   }
    // }
  }

  /**
   * @brief Begins playing this pattern, from the begining
   */
  void start() {
    curr_step = 0;
    next_step_time = 0;
    remaining_repeats = num_repeats;
    active = true;
    start_flag = true;
  }

  /**
   * @brief Stops playing this pattern
   */
  void stop() {
    active = false;
  }

  /**
   * @brief Moves to next step. To be called after all calculations for current step are done
   */
  void incrementStep() {

    curr_step = curr_step + 1;
    if (curr_step >= num_steps) {  // finished cycling through pattern
      curr_step = 0;
      Serial.println(num_repeats);
      if (num_repeats >= -1) {
        remaining_repeats--;
        Serial.println(num_repeats);
        Serial.println(remaining_repeats);
        if (remaining_repeats < 0) {
          active = false;
        }
      }
    }
  }

  /**
   * @brief Moves to next sixteenth note. To be called whenever a midi 16th note is received
   */
  void incrementClock(uint16_t my_midi_step_num) {
    if (start_flag) {
      if (my_midi_step_num % 24 == 0) {
        start_flag = false;
      }
    } else {
      sclock++;
    }
  }

private:
};

#endif /* SAMPLEPATTERN_H_ */