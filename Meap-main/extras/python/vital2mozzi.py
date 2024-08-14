import sys, array, os, textwrap, random


if len(sys.argv) != 7:
        print ('Usage: vital2mozzi.py <infile outfile tablename bit_depth table_size step_size>')
        sys.exit(1)

[infile, outfile, tablename, bit_depth, table_size, step_size] = sys.argv[1:]

def vital2mozzi(infile, outfile, tablename, bit_depth, table_size, step_size):
	fin = open(os.path.expanduser(infile), "rb")
	print ("opened " + infile)


	if bit_depth == '16':
		# getsize returns size in bytes so we divide by 2 to get number of 16-bit samples in output
		int16_tstoread = os.path.getsize(os.path.expanduser(infile))//2 
		valuesfromfile = array.array('h') # array of signed int16_t ints
		try:
			valuesfromfile.fromfile(fin, int16_tstoread)
		finally:
			fin.close()
	elif bit_depth == '8':
		int8_tstoread = os.path.getsize(os.path.expanduser(infile))
		valuesfromfile = array.array('b') # array of signed int8_t ints
		try:
			valuesfromfile.fromfile(fin, int8_tstoread)
		finally:
			fin.close()
	else:
		print("Bit depth must be 8 or 16!") # make a proper error here


	values=valuesfromfile.tolist()

	# ---------- PRUNING TABLES TO CORRECT SIZE ---------
	if(table_size == '2048'):
		pruned_values = values
		# print("proceed as normal")
	elif(table_size == '1024'):
		pruned_values = values[::2]
		# print("keep every other sample")
	elif(table_size == '512'):
		pruned_values = values[::4]
		# print("keep every 4th sample")
	elif(table_size == '256'):
		pruned_values = values[::8]
		# print("keep every 8th sample")

	step_size_i = int(step_size)
	table_size_i = int(table_size)

	# ---------- ONLY GRABBING EVERY N TABLES ----------
	num_output_tables = 256 // step_size_i

	input_start_index = 0
	output_start_index = 0

	stepped_values = []

	for i in range(num_output_tables):
		stepped_values[output_start_index:output_start_index + table_size_i] = pruned_values[input_start_index:input_start_index + table_size_i]
		output_start_index = output_start_index + table_size_i
		input_start_index = input_start_index + (table_size_i * step_size_i)

	# ---------- WRITING OUTPUT FILE ----------

	fout = open(os.path.expanduser(outfile), "w")
	fout.write('#ifndef ' + tablename + '_H_' + '\n')
	fout.write('#define ' + tablename + '_H_' + '\n \n')
	fout.write('#if ARDUINO >= 100'+'\n')
	fout.write('#include "Arduino.h"'+'\n')
	fout.write('#else'+'\n')
	fout.write('#include "WProgram.h"'+'\n')
	fout.write('#endif'+'\n')
	fout.write('#include "mozzi_pgmspace.h"'+'\n \n')
	fout.write('#define ' + tablename + '_NUM_CELLS '+ str(len(stepped_values))+'\n')

	# print("out_length: " + str(len(stepped_values)))

	fout.write('#define ' + tablename + '_FRAME_SIZE ' + table_size +'\n')
	fout.write('#define ' + tablename + '_NUM_FRAMES ' + str(num_output_tables) + '\n \n')

	outstring = 'CONSTTABLE_STORAGE(int' + bit_depth + '_t) ' + tablename + '_DATA [] = {'
	try:
		for i in range(len(stepped_values)):
			## mega2560 boards won't upload if there is 33, 33, 33 in the array, so dither the 3rd 33 if there is one
			if (stepped_values[i] == 33) and (stepped_values[i+1] == 33) and (stepped_values[i+2] == 33):
				stepped_values[i+2] = random.choice([32, 34])
			outstring += str(stepped_values[i]) + ", "
	finally:
		outstring +=  "};"
		outstring = textwrap.fill(outstring, 80)
		fout.write(outstring)
		fout.write('\n\n#endif /* ' + tablename + '_H_ */\n')
		fout.close()
		print ("wrote " + outfile)

vital2mozzi(infile, outfile, tablename, bit_depth, table_size, step_size)