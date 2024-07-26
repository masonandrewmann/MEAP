#ifndef MEAP_ADSR_H
#define MEAP_ADSR_H

/***************************************************/
/*! \class stk::ADSR
    \brief STK ADSR envelope class.

    This class implements a traditional ADSR (Attack, Decay, Sustain,
    Release) envelope.  It responds to simple keyOn and keyOff
    messages, keeping track of its state.  The \e state = ADSR::IDLE
    before being triggered and after the envelope value reaches 0.0 in
    the ADSR::RELEASE state.  All rate, target and level settings must
    be non-negative.  All time settings are in seconds and must be
    positive.

    by Perry R. Cook and Gary P. Scavone, 1995--2019.
*/
/***************************************************/

class MEAP_ADSR
{
public:
    //! ADSR envelope states.
    enum
    {
        ATTACK,  /*!< Attack */
        DECAY,   /*!< Decay */
        SUSTAIN, /*!< Sustain */
        RELEASE, /*!< Release */
        IDLE     /*!< Before attack / after release */
    };

    //! Default constructor.
    MEAP_ADSR(void)
    {
        target_ = 0.0;
        value_ = 0.0;
        attackRate_ = 0.001;
        decayRate_ = 0.001;
        releaseRate_ = 0.005;
        releaseTime_ = -1.0;
        sustainLevel_ = 0.5;
        state_ = IDLE;
    };

    //! Class destructor.
    ~MEAP_ADSR(void){};

    //! Set target = 1, state = \e ADSR::ATTACK.
    void keyOn(void)
    {
        if (target_ <= 0.0f)
            target_ = 1.0f;
        state_ = ATTACK;
    };

    //! Set target = 0, state = \e ADSR::RELEASE.
    void keyOff(void)
    {
        target_ = 0.0f;
        state_ = RELEASE;

        // FIXED October 2010 - Nick Donaldson
        // Need to make release rate relative to current value!!
        // Only update if we have set a TIME rather than a RATE,
        // in which case releaseTime_ will be -1
        if (releaseTime_ > 0.0f)
            releaseRate_ = value_ / (releaseTime_ * (float)AUDIO_RATE);
    };

    //! Set the attack rate (gain / sample). (rate greater than 0)
    void setAttackRate(float rate)
    {
        attackRate_ = rate;
    };

    //! Set the target value for the attack (default = 1.0). (target greater than 0)
    void setAttackTarget(float target)
    {
        target_ = target;
    };

    //! Set the decay rate (gain / sample). (rate greater than 0)
    void setDecayRate(float rate)
    {
        decayRate_ = rate;
    };

    //! Set the sustain level. (sustain greater than or equal to 0)
    void setSustainLevel(float level)
    {
        sustainLevel_ = level;
    };

    //! Set the release rate (gain / sample). (rate greater than 0)
    void setReleaseRate(float rate)
    {
        releaseRate_ = rate;

        // Set to negative value so we don't update the release rate on keyOff()
        releaseTime_ = -1.0;
    };

    //! Set the attack rate based on a time duration (seconds).(time greater than 0)
    void setAttackTime(float time)
    {
        attackRate_ = 1.0f / (time * (float)AUDIO_RATE);
    };

    //! Set the decay rate based on a time duration (seconds). (time greater than 0)
    void setDecayTime(float time)
    {
        decayRate_ = (1.0f - sustainLevel_) / (time * (float)AUDIO_RATE);
    };

    //! Set the release rate based on a time duration (seconds). (time greater than 0)
    void setReleaseTime(float time)
    {
        releaseRate_ = sustainLevel_ / (time * (float)AUDIO_RATE);
        releaseTime_ = time;
    };

    //! Set sustain level and attack, decay, and release time durations (seconds).
    void setAllTimes(float aTime, float dTime, float sLevel, float rTime)
    {
        this->setAttackTime(aTime);
        this->setSustainLevel(sLevel);
        this->setDecayTime(dTime);
        this->setReleaseTime(rTime);
    };

    //! Set a sustain target value and attack or decay from current value to target. (target greater than 0)
    void setTarget(float target)
    {
        target_ = target;

        this->setSustainLevel(target_);
        if (value_ < target_)
            state_ = ATTACK;
        if (value_ > target_)
            state_ = DECAY;
    };

    //! Return the current envelope \e state (ATTACK, DECAY, SUSTAIN, RELEASE, IDLE).
    int getState(void) const { return state_; };

    //! Set to state = ADSR::SUSTAIN with current and target values of \e value.
    void setValue(float value)
    {
        state_ = SUSTAIN;
        target_ = value;
        value_ = value;
        this->setSustainLevel(value);
        last_output_ = value;
    };
    ;

    //! Return the last computed output value.
    float lastOut(void) const { return last_output_; };

    //! Compute and return one output sample.
    float next(void)
    {
        switch (state_)
        {

        case ATTACK:
            value_ += attackRate_;
            if (value_ >= target_)
            {
                value_ = target_;
                target_ = sustainLevel_;
                state_ = DECAY;
            }
            last_output_ = value_;
            break;

        case DECAY:
            if (value_ > sustainLevel_)
            {
                value_ -= decayRate_;
                if (value_ <= sustainLevel_)
                {
                    value_ = sustainLevel_;
                    state_ = SUSTAIN;
                }
            }
            else
            {
                value_ += decayRate_; // attack target < sustain level
                if (value_ >= sustainLevel_)
                {
                    value_ = sustainLevel_;
                    state_ = SUSTAIN;
                }
            }
            last_output_ = value_;
            break;

        case RELEASE:
            value_ -= releaseRate_;
            if (value_ <= 0.0f)
            {
                value_ = 0.0f;
                state_ = IDLE;
            }
            last_output_ = value_;
        }

        return value_;
    };

protected:
    int state_;
    float value_;
    float target_;
    float attackRate_;
    float decayRate_;
    float releaseRate_;
    float releaseTime_;
    float sustainLevel_;

    float last_output_;
};

#endif // MEAP_ADSR_H
