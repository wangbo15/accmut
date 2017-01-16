from sys import argv

script, src , tar = argv

testid = 0
def process(line):
	if line.startswith("echo"):
		return line
	words = line.split('>')
	if len(words) > 2:
		print "ERR LINE : %r " %words
		return ''
	else:
		fst = list(words[0])
		#print fst
		if '<' in fst:
			fst.remove('<')
		newfst = ''.join(fst)
		global testid
		testid += 1
		return newfst + '\t' + str(testid) + '\n'


src_f = open(src)
tar_f = open(tar, 'w')

print "Parsing source script file : %r" % src
print "Writing to target file : %r" % tar

for line in src_f.readlines():
	newline = process(line)
	tar_f.write(newline)


src_f.close()
tar_f.close()
print "Finish"
