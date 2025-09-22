#ifndef M_TRANSITION_TABLE_H_
#define M_TRANSITION_TABLE_H_

template <int NUM_ENTRIES>
class mTransitionTable
{
public:
    mTransitionTable(float transition_table[NUM_ENTRIES][NUM_ENTRIES])
    {
        updateTable(transition_table);
        current_entry = 0;
    }

    void updateTable(float transition_table[NUM_ENTRIES][NUM_ENTRIES])
    {
        float sums[NUM_ENTRIES];

        // copy input table and calculate sum of each row
        for (int row = 0; row < NUM_ENTRIES; row++)
        {
            sums[row] = 0;
            for (int col = 0; col < NUM_ENTRIES; col++)
            {
                cumulative_table[row][col] = 0;
                table[row][col] = transition_table[row][col];

                sums[row] += table[row][col];
            }
            // su.ms[row] = 1.f / sums[row]; // pre-inverting to save divides in the next step
        }

        // normalizing probablity density function
        for (int row = 0; row < NUM_ENTRIES; row++)
        {
            for (int col = 0; col < NUM_ENTRIES; col++)
            {
                table[row][col] /= sums[row];
            }
        }

        // calculate cumulative probability density function
        for (int row = 0; row < NUM_ENTRIES; row++)
        {
            for (int col = 0; col < NUM_ENTRIES; col++)
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

        for (int i = 0; i < NUM_ENTRIES; i++)
        {
            if (cumulative_table[current_entry][i] >= R)
            {
                current_entry = i;
                break;
            }
        }
        return current_entry;
    }

    void setCurrentEntry(int entry)
    {
        current_entry = entry;
    }

    int getCurrentEntry()
    {
        return current_entry;
    }

    int print()
    {
        Serial.println("table");
        for (int row = 0; row < NUM_ENTRIES; row++)
        {
            for (int col = 0; col < NUM_ENTRIES; col++)
            {
                Serial.print(cumulative_table[row][col]);
                Serial.print(" ");
            }
            Serial.println(" ");
        }
        Serial.println("done");
    }

    int current_entry;
    float table[NUM_ENTRIES][NUM_ENTRIES];
    float cumulative_table[NUM_ENTRIES][NUM_ENTRIES];
};

#endif // M_TRANSITION_TABLE_H_