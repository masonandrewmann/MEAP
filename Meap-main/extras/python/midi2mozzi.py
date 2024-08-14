import mido
from mido import MidiFile
import numpy as np
import sys, array, os, textwrap, random

if len(sys.argv) != 4:
        print ('Usage: midi2mozzi.py <infile outfile outname>')
        sys.exit(1)

[infile, outfile, outname] = sys.argv[1:]

def midi2mozzi(infile, outfile, outname):
    # SETUP ALWAYS FILL THIS OUT FOR A NEW FILE
    mid = MidiFile(infile)
    # maybe set output channel here

    name = (outname)
    # SETUP ALWAYS FILL THIS OUT FOR A NEW FILE

    message_type = []
    data1 = []
    data2 = []
    channel = []
    time = []

    ## I DON'T KNOW HOW WELL THIS DEALS WITH MULTITRACK MIDI FILES !!!!!!!!!!!!!!!

    time_accumulator = 0 # will accumulate time of unused midi messages
    # ONCE THRU TO SET SIZE AND RECORD NOTES
    for i, track in enumerate(mid.tracks):
        print('Track {}: {}'.format(i, track.name))
        for msg in track:
            msg_dict = msg.dict()
            msg_bytes = msg.bytes()
            # print(msg_bytes)
            msg_type = msg_dict['type']
            # print(msg_dict)
            if(msg_type != 'note_on' and msg_type != 'note_off' and msg_type != 'control_change' and msg_type != 'program_change'):
                print('weird message type: ' + msg_type)
                if(msg_type == 'end_of_track'):
                    message_type = message_type + [msg_bytes[0]]
                    data1 = data1 + [msg_bytes[0]]
                    data2 = data2 + [msg_bytes[0]]
                    channel = channel + [msg_bytes[0]]
                    time = time + [round(msg_dict['time']/4 + time_accumulator)]
            if(len(msg_bytes) == 3 and msg_bytes[0] != 255):
                message_type = message_type + [msg_bytes[0]]
                data1 = data1 + [msg_bytes[1]]
                data2 = data2 + [msg_bytes[2]]
                channel = channel + [msg_dict['channel']]
                time = time + [round(msg_dict['time']/4 + time_accumulator)]
                time_accumulator = 0
            else:
                print('message of more than 3 bytes (or meta event): ')
                print(msg_bytes)
                # print(msg_dict['time'])
                time_accumulator = time_accumulator + round(msg_dict['time']/4)
        
    # filename = name + '.h'
    filename = outfile
    defname = 'MEAPER_' + name + '_H_'

    out_str = 'uint8_t ' + name + '_data' + '[] = { '

    line_counter = 0
    for i in range(len(message_type)):
        out_str = out_str + str(message_type[i])
        out_str = out_str + ', '
        line_counter += 1
        if(line_counter > 30):
            out_str = out_str + '\n'
            line_counter = 0;
        out_str = out_str + str(data1[i])
        out_str = out_str + ', '
        line_counter += 1
        if(line_counter > 30):
            out_str = out_str + '\n'
            line_counter = 0;
        out_str = out_str + str(data2[i])
        out_str = out_str + ', '
        line_counter += 1
        if(line_counter > 30):
            out_str = out_str + '\n'
            line_counter = 0;
        # out_str = out_str + "pp"
        out_str = out_str + str(time[i]>>8 & 0b11111111) # top byte of time array
        out_str = out_str + ', '
        line_counter += 1
        if(line_counter > 30):
            out_str = out_str + '\n'
            line_counter = 0;
        out_str = out_str + str(time[i] & 0b11111111) # bottom byte of time array
        if i < (len(message_type) - 1):
            out_str = out_str + ', '
        line_counter += 1
        if(line_counter > 30):
            out_str = out_str + '\n'
            line_counter = 0;
        # out_str = out_str + "nn"
    out_str = out_str + ' };'
        
    with open(filename, "w") as f:
        print('#ifndef ' + defname, file=f)
        print('#define ' + defname, file=f)
        print('\n', file=f)
        print('#define ' + name + '_NUM_NOTES ' + str(len(data1)), file=f)
        print('\n', file=f)
        print (out_str, file=f)
        print('\n', file=f)
        print('#endif /* ' + defname + ' */', file=f)



midi2mozzi(infile, outfile, outname)