#ifndef SAMPLEPATTER_H_
#define SAMPLEPATTER_H_


#endif /* SAMPLEPATTER_H_ */


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
  uint16_t *amplitude;
  uint16_t num_steps;
  uint16_t num_repeats;
  uint16_t curr_step;
  uint16_t remaining_repeats;
  uint16_t sample_num;
  uint32_t next_step_time;
  uint32_t note_end_time;

  /**
   * @brief Constructor
   *
   * @param num_steps how many steps in the pattern
   * @param num_repeats how many times to repeat the pattern per trigger
   * @param sample_num num of the sample to play
   */
  SamplePattern(uint16_t num_steps, uint16_t num_repeats, uint16_t sample_num) {
    // Constructor
    midi_notes = new uint8_t[num_steps];
    step_duration = new uint16_t[num_steps];
    note_sustain = new uint16_t[num_steps];
    amplitude = new uint16_t[num_steps];

    curr_step = 0;
    num_repeats = 0;
    next_step_time = 0;
    note_end_time = 0;
  }

  /**
   * @brief Checks time to see if pattern needs to move on and executes appropriate methods
   */
  void cycle() {
    uint32_t curr_time = millis();
    if (curr_time > next_step_time) {  // do we need to trigger a new step
    }
  }

private:
};