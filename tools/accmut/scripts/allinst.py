from sys import argv

script, f_name, fid_name  = argv

#FID ==>> FNAME
profile_dict = {}

#FNAME ==>> INST_NUM
instnum_dict = {}

init_func_list = ['__accmut__init', '__accmut__sepcific_timer', '__accmut__set_sig_handlers', '__accmut__load_all_muts', '__accmut__oracal_bufinit']

io_func_list = ['__accmut__fopen', '__accmut__fclose', '__accmut__fseek', '__accmut__ferror', '__accmut__fileno', '__accmut__feof', '__accmut__freopen', 
				'__accmut__unlink', '__accmut__fgets', '__accmut__getc', '__accmut__fread', '__accmut__ungetc', '__accmut__fputc', '__accmut__fputs', 
				'__accmut__puts', '__accmut__fprintf', '__accmut__printf', '__accmut__fwrite']

io_uitl_func_list = ['__accmut__checkoutput', '__accmut__exit_check_output', '__accmut__oracal_bufinit', '__accmut__setout', '__accmut__oracledump',
					 '__accmut__filedump']

exit_func_list = ['__accmut__SIGSEGV__handler', '__accmut__SIGABRT__handler', '__accmut__timeout_handler']

#str_func_list = ['__accmut__strlen', '__accmut__strcat', '__accmut__itoa']

map_file = open(fid_name);
for line in map_file.readlines():
	item = line.strip().split(':')
	#print item[0], item[1]
	profile_dict[item[1]] = item[0]
	instnum_dict[item[0]] = 0


ints_file = open(f_name)

total = 0

for line in ints_file.readlines():
	fname = profile_dict[line.strip()]
	instnum_dict[fname] += 1
	total += 1

ori_inst = 0
try_inst = 0
acc_inst = 0
filt_mut = 0
filt_var = 0
try_inst = 0
devide_inst = 0

for key, value in instnum_dict.items():
	if int(value) != 0:
		print key, value
	if key.startswith('__accmut__'):
		acc_inst += int(value)
	else:
		ori_inst += int(value)
	if key == '__accmut__filter__mutants':
		filt_mut += int(value)
	if key == '__accmut__filter__variant':
		filt_var += int(value)
	if key.startswith('__accmut__cal_') or key.startswith('__accmut__prepare'):
		try_inst += int(value)
	if key.startswith('__accmut__divide__eqclass'):
		devide_inst += int(value)

print "================= SUMARIZE ================"
print "TOTAL: ", total
print "ORI INST: ", ori_inst, ori_inst/float(total)
print "ACC INST: ", acc_inst, acc_inst/float(total)
print "FILT_MUT INST: ", filt_mut, "  FILT_VAR INST: ", filt_var
print "TRY INST: ", try_inst

#########################################################
print "================= REMOVE INIT, EXIT AND IO================"

init_inst = 0
for item in init_func_list:
	init_inst += int(instnum_dict[item])

exit_inst = 0
for item in exit_func_list:
	exit_inst += int(instnum_dict[item])

io_inst = 0
for item in io_func_list:
	io_inst += int(instnum_dict[item])

'''str_inst = 0
for item in str_func_list:
	str_inst += int(instnum_dict[item])
'''

print "CHECK: ", init_inst, exit_inst, io_inst

total = total - init_inst - exit_inst - io_inst 
acc_inst = acc_inst - init_inst - exit_inst - io_inst 
print "TOTAL: ", total, " ORI: ", ori_inst, " ACC: ", acc_inst
print "ORI/TOTAL: ", ori_inst/float(total)
print "ACC/TOTAL: ", acc_inst/float(total)
print "FILT_MUT/TOTAL: ", filt_mut/float(total)
print "FILT_VAR/TOTAL: ", filt_var/float(total)
print "TRY/TOTAL: ", try_inst/float(total)
print "DIV/TOTAL: ", devide_inst/float(total)


