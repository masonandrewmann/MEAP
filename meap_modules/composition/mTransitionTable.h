#ifndef M_TRANSITION_TABLE_H_
#define M_TRANSITION_TABLE_H_

template <int NUM_STATES>
class mTransitionTable
{
public:
    mTransitionTable(float transition_table[NUM_STATES][NUM_STATES])
    {
        updateTable(transition_table);
        current_state = 0;
    }

    void updateTable(float transition_table[NUM_STATES][NUM_STATES])
    {
        float sums[NUM_STATES];

        // copy input table and calculate sum of each row
        for (int row = 0; row < NUM_STATES; row++)
        {
            sums[row] = 0;
            for (int col = 0; col < NUM_STATES; col++)
            {
                cumulative_table[row][col] = 0;
                table[row][col] = transition_table[row][col];

                sums[row] += table[row][col];
            }
            sums[row] = 1.f / sums[row]; // pre-inverting to save divides in the next step
        }

        // normalizing probablity density function
        for (int row = 0; row < NUM_STATES; row++)
        {
            for (int col = 0; col < NUM_STATES; col++)
            {
                table[row][col] *= sums[row];
            }
        }

        // calculate cumulative probability density function
        for (int row = 0; row < NUM_STATES; row++)
        {
            for (int col = 0; col < NUM_STATES; col++)
            {
                for (int i = 0; i <= col; i++)
                {
                    cumulative_table[row][col] += table[row][i];
                }
            }
        }
    }

    int step()
    {
        float R = Meap<MEAP_DEFAULT_VERSION>::frand();

        for (int i = 0; i < NUM_STATES; i++)
        {
            if (cumulative_table[current_state][i] >= R)
            {
                current_state = i;
                break;
            }
        }
        return current_state;
    }

    void setState(int state)
    {
        current_state = state;
    }

    int getState()
    {
        return current_state;
    }

    int current_state;
    float table[NUM_STATES][NUM_STATES];
    float cumulative_table[NUM_STATES][NUM_STATES];
};

#endif // M_TRANSITION_TABLE_H_