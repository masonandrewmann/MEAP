/**
* Keeps track of a single note turned on by a midi message
*/
class ActiveMidiNote {
public:
  uint8_t note_num; /**< MIDI note number */
  uint8_t velocity; /**< MIDI note velocity*/
  uint8_t channel;  /**< MIDI note channel */
  uint8_t program;
  uint16_t voice_num;     /**< Voice allocated to play this note */
  float frequency;        /**< Frequency of note, used for portamento */
  bool port_done;         /**< True when portamento is done, False while portamento is sliding to new note */
  uint32_t port_end_time; /**< End time of portamento slide */
  uint32_t note_end_time; /**< Time to automatically end note (mostly used for computer triggered notes)*/

  void newNote(uint8_t _note_num, uint8_t _velocity, uint8_t _channel, float _frequency, uint16_t _voice_num) {
    note_num = _note_num;
    velocity = _velocity;
    channel = _channel;
    frequency = _frequency;
    voice_num = _voice_num;
    note_end_time = 0;
  };

  void setEndTime(uint32_t _note_end_time) {
    note_end_time = _note_end_time;
  }

  void portSet(bool _port_done, uint32_t _port_end_time) {
    port_done = _port_done;
    port_end_time = _port_end_time;
  }

  bool finished() {
    if (millis() > note_end_time) {
      return true;
    } else {
      return false;
    }
  }
};