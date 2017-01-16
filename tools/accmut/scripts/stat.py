

schem_file = open("timeres.sche")

#sma_eval_file = open("evaltime")

sma_file = open("timeres.dma.nodiv")

dma_file = open("timeres.dma")

total_schm = 0L

for line in schem_file.readlines():
	tid, sec, usec = line.split()
	tid = int(tid)
	sec = long(sec)
	usec = long(usec)
	total_schm = total_schm + usec + sec*1000000
	
total_sma = 0L

for line in sma_file.readlines():
	tid, sec, usec = line.split()
	tid = int(tid)
	sec = long(sec)
	usec = long(usec)
	total_sma = total_sma + usec + sec*1000000
	
total_dma = 0L

for line in dma_file.readlines():
	tid, sec, usec = line.split()
	tid = int(tid)
	sec = long(sec)
	usec = long(usec)
	total_dma = total_dma + usec + sec*1000000

tsch = float(total_schm)
tsma = float(total_sma)
tdma = float(total_dma)


print "SHE TIME %f us" % tsch


print "NO_DIV TIME %f us" % tsma

print "DMA TIME %f us" % tdma

print "-------------------------"
print "NO_DIV/SHE %f" % (tsma/tsch)
print "DMA/SHE %f" % (tdma/tsch)
print "DMA/NO_DIV %f" % (tdma/tsma)
print "-------------------------"
print "SHE/NO_DIV %f" % (tsch/tsma)
print "SHE/DMA %f" % (tsch/tdma)
print "NO_DIV/DMA %f" % (tsma/tdma)
