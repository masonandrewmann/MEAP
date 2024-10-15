/*
 * mOperator.h
 *
 */
#ifndef MOPERATOR_H_
#define MOPERATOR_H_

/**
mOperator implements an FM operator using an oscillator (mOscil) and envelope (ADSR). An mOperator can stand on its
own or be tied to other mOperators to create complex FM sounds. Always returns 16-bit output.

Template:
tparam NUM_CELLS This is defined in the table ".h" file the Oscil will be
using.

@tparam T This is the bit-rate of the wavetable being used. It can be found as datatype of the
data array of the table your are using or often in the name of the ".h" file. Typically int16_t or int8_t.
 */
template <uint32_t NUM_CELLS, class T = int8_t>
class mOperator
{
public:
    /** Constructor.
    Declare an mOperator without specifying a wavetable to use
    */
    mOperator()
    {
        init(NULL);
    };

    /** Constructor.
    @param TABLE_NAME the name of the array the mOscil will be using. This
    can be found in the table ".h" file if you are using a table made for
    Mozzi by the int8_t2mozzi.py python script in Mozzi's python
    folder.*/
    mOperator(const T *TABLE_NAME)
    {
        init(TABLE_NAME);
    };

    /** init.
     * Initialized the mOperator object. Called automatically by constructor. You should only need to use this in cases
     * where the non-default constructor can't be called.
    @param TABLE_NAME the name of the array the mOscil will be using. This
    can be found in the table ".h" file if you are using a table made for
    Mozzi by the int8_t2mozzi.py python script in Mozzi's python
    folder.*/
    void init(const T *TABLE_NAME)
    {
        osc_.setTable(TABLE_NAME);
        osc_.setFreq(220);
        sustain_time_ = 4294967295;
        env_.setTimes(0, 0, sustain_time_, 0);
        sustain_level_ = 255;
        env_.setADLevels(255, sustain_level_);
        env_val_ = 0;
        osc_freq_ratio_ = 1.0;
        gain_ = 255;

        if (sizeof(T) == sizeof(int16_t))
        {
            shift_val_ = 16;
        }
        else
        {
            shift_val_ = 8;
        }

        looping_on_ = false;
        phase_sync_ = true;
        drone_ = false;
    }
    /** noteOn.
     * Begins a note. Sets frequency of oscillator and triggers envelope.
    @param note midi note number (0-127)
    @param velocity note amplitude (0-127)
    */
    void noteOn(uint8_t note, uint8_t velocity)
    {
        osc_.setFreq(mtof(note) * osc_freq_ratio_);
        if (phase_sync_)
        {
            osc_.setPhase(0);
        }
        env_.setADLevels(velocity << 1, (velocity * sustain_level_ >> 7));
        env_.noteOn();
        note_active_ = true;
    }
    /** noteOff.
     * Turns currently playing note off. Moves envelope to decay phase
     */
    void noteOff()
    {
        env_.noteOff();
        note_active_ = false;
    }

    /** setTable.
     * Sets the wavetable your oscillator uses. Must be the same size as NUM_CELLS
    @param TABLE_NAME the name of the array the mOscil will be using.
    */
    void setTable(const T *TABLE_NAME)
    {
        osc_.setTable(TABLE_NAME);
    }

    /** setFreq.
     * Sets frequency of oscillator without triggering the envelope
    @param f_ frequency in Hz
    */
    void setFreq(float f_)
    {
        osc_.setFreq(f_ * osc_freq_ratio_);
    }

    /** setFreqRatio.
     * Sets ratio of the frequency the operator will actually play in relation to
     * the frequency it is told to play. Useful when you are using multiple operators to
     * build a single voice. For example you may want one operator to play an octave higher than the fundamental.
    @param f_ frequency ratio
    */
    void setFreqRatio(float r_)
    {
        osc_freq_ratio_ = r_;
    }

    /** getFreqRatio.
    @return the frequency ratio
    */
    float getFreqRatio()
    {
        return osc_freq_ratio_;
    }

    /** setLoopingOn
     * Sets envelope to continuously loop through its phases. Has the side effect of setting the sustain time to zero.
     */
    void setLoopingOn()
    {
        looping_on_ = true;
        env_.setSustainTime(0);
    }

    /** setLoopingOff
     * Sets envelope to stop once it reaches the end of decay phase
     */
    void setLoopingOff()
    {
        looping_on_ = false;
        env_.setSustainTime(sustain_time_);
    }

    /** setPhaseSyncOn
     * When phase sync is enabled, oscillator will reset to zero phase every time noteOn is
     * called. Useful for synchronizing multiple operators.
     */
    void setPhaseSyncOn()
    {
        phase_sync_ = true;
    }

    /** setPhaseSyncOff
     * When phase sync is disabled, oscillator will NOT reset to zero phase when noteOn is
     * called.
     */
    void setPhaseSyncOff()
    {
        phase_sync_ = false;
    }

    /** setDroneOn
     * When drone is enabled, the mOscil will bypass its envelope, playing continuously like a regular oscillator.
     */
    void setDroneOn()
    {
        drone_ = true;
    }

    /** setDroneOff
     * When drone is disabled, the mOscil will be enveloped, functioning like a regular operator.
     */
    void setDroneOff()
    {
        drone_ = false;
    }

    /** setGain
     * Sets output gain of oscillator
    @param g_ gain between 0 and 255
     */
    void setGain(uint16_t g_)
    {
        gain_ = g_;
    }

    /** setAttackTime
     * Sets attack time of envelope
    @param a_ attack time in ms
     */
    void setAttackTime(uint32_t a_)
    {
        env_.setAttackTime(a_);
    }

    /** setDecayTime
     * Sets decay time of envelope
    @param d_ decay time in ms
     */
    void setDecayTime(uint32_t d_)
    {
        env_.setDecayTime(d_);
    }

    /** setSustainTime
     * Sets sustain time of envelope
    @param s_ decay time in ms
     */
    void setSustainTime(uint32_t s_)
    {
        sustain_time_ = s_;
        env_.setSustainTime(s_);
    }

    /** setSustainLevel
     * Sets sustain level of envelope
    @param s_l sustain level between 0 and 255
     */
    void setSustainLevel(uint16_t s_l)
    {
        sustain_level_ = s_l;
    }

    /** setReleaseTime
     * Sets release time of envelope
    @param r_ release time in ms
     */
    void setReleaseTime(uint32_t r_)
    {
        env_.setReleaseTime(r_);
    }

    /** setTimes
     * Sets attack, decay, sustain and release times of envelope
    @param a_ attack time in ms
    @param d_ decay time in ms
    @param s_ decay time in ms
    @param r_ release time in ms
     */
    void setTimes(uint32_t a_, uint32_t d_, uint32_t s_, uint32_t r_)
    {
        env_.setTimes(a_, d_, s_, r_);
    }

    /** setADLevels
     * Sets attack and decay levels of envelope
    @param a_l_ level reached after attack phase (0-255)
    @param d_l_ level reached after decay phase (0-255)
     */
    void setADLevels(uint32_t a_l_, uint32_t d_l_)
    {
        sustain_level_ = d_l_;
        env_.setADLevels(a_l_, d_l_);
    }

    /** update
     * MUST be called in updateControl. Calculates envelope.
     */
    void update()
    {
        env_.update();
        if (looping_on_ && note_active_ && (env_.playing() == false))
        {
            env_.noteOn();
        }
    }

    /** update
     * MUST be called in updateAudio. Returns a 16-bit sample.
     */
    int32_t next()
    {
        if (drone_)
        {
            env_val_ = 255;
        }
        else
        {
            env_val_ = env_.next();
        }
        last_out_ = (osc_.next() * env_val_ * gain_) >> shift_val_;
        return last_out_; // returns a 16 bit sample
    }

    /** update
     * MUST be called in updateAudio. Returns a 16-bit sample given a modulation input.
     @param mod_input phase modulation value. range is 16-bits
     */
    int32_t next(int32_t mod_input)
    {
        if (drone_)
        {
            env_val_ = 255;
        }
        else
        {
            env_val_ = env_.next();
        }
        UFix<16, 16> deviation = 1.0;
        auto modulation = deviation * toSFraction((int16_t)(mod_input));
        last_out_ = osc_.phMod(modulation);
        return (last_out_ * env_val_ * gain_) >> shift_val_; // returns a 16 bit sample
    }

protected:
    mOscil<NUM_CELLS, AUDIO_RATE, T> osc_;
    float osc_freq_ratio_;

    ADSR<CONTROL_RATE, AUDIO_RATE> env_;
    uint16_t env_val_;
    uint16_t shift_val_;
    uint32_t sustain_time_;
    uint16_t sustain_level_;
    uint16_t gain_;
    int32_t last_out_;

    bool note_active_;
    bool phase_sync_;
    bool looping_on_;
    bool drone_;

    UFix<0, 16> mod_index_;
    UFix<16, 16> deviation_;
};

#endif // MOPERATOR_H_
