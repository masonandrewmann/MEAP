#ifndef MEAP_BITCRUSHER_H_
#define MEAP_BITCRUSHER_H_

class mBitcrusher
{
public:
    //! Class constructor.
    mBitcrusher(void) {
    };

    void bits(uint8_t b)
    {
        if (b > 16)
            b = 16;
        else if (b == 0)
            b = 1;
        crushBits = b;
    }

    void sampleRate(float hz)
    {
        int n = (AUDIO_SAMPLE_RATE_EXACT / hz) + 0.5f;
        if (n < 1)
            n = 1;
        else if (n > 64)
            n = 64;
        sampleStep = n;
    }

    void next()
    {
        audio_block_t *block;
        uint32_t i;
        uint32_t sampleSquidge, sampleSqueeze; // squidge is bitdepth, squeeze is for samplerate

        if (crushBits == 16 && sampleStep <= 1)
        {
            // nothing to do. Output is sent through clean, then exit the function
            block = receiveReadOnly();
            if (!block)
                return;
            transmit(block); // send block after calculating it
            release(block);  // free the sent block
            return;
        }
        // start of processing functions. Could be more elegant based on external
        // functions but left like this to enable code optimisation later.
        block = receiveWritable(); // check if we need to write a block
        if (!block)
            return;

        if (sampleStep <= 1)
        { // no sample rate mods, just crush the bitdepth.
            for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
            {
                // shift bits right to cut off fine detail sampleSquidge is a
                // uint32 so sign extension will not occur, fills with zeroes.
                sampleSquidge = block->data[i] >> (16 - crushBits);
                // shift bits left again to regain the volume level.
                // fills with zeroes.
                block->data[i] = sampleSquidge << (16 - crushBits);
            }
        }
        else if (crushBits == 16)
        { // bitcrusher not being used, samplerate mods only.
            i = 0;
            while (i < AUDIO_BLOCK_SAMPLES)
            {
                // save the root sample. this will pick up a root
                // sample every _sampleStep_ samples.
                sampleSqueeze = block->data[i];
                for (int j = 0; j < sampleStep && i < AUDIO_BLOCK_SAMPLES; j++)
                {
                    // for each repeated sample, paste in the current
                    // root sample, then move onto the next step.
                    block->data[i] = sampleSqueeze;
                    i++;
                }
            }
        }
        else
        { // both being used. crush those bits and mash those samples.
            i = 0;
            while (i < AUDIO_BLOCK_SAMPLES)
            {
                // save the root sample. this will pick up a root sample
                // every _sampleStep_ samples.
                sampleSqueeze = block->data[i];
                for (int j = 0; j < sampleStep && i < AUDIO_BLOCK_SAMPLES; j++)
                {
                    // shift bits right to cut off fine detail sampleSquidge
                    // is a uint32 so sign extension will not occur, fills
                    // with zeroes.
                    sampleSquidge = sampleSqueeze >> (16 - crushBits);
                    // shift bits left again to regain the volume level.
                    // fills with zeroes. paste into buffer sample +
                    // sampleStep offset.
                    block->data[i] = sampleSquidge << (16 - crushBits);
                    i++;
                }
            }
        }
        transmit(block);
        release(block);
    }

protected:
    uint8_t crushBits;  // 16 = off
    uint8_t sampleStep; // the number of samples to double up. This simple technique only allows a few stepped positions.
};

#endif // MEAP_BITCRUSHER_H_
