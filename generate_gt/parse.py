
import sys

#list.orig.txt
f=open(sys.argv[1]);

for i in f.readlines():
	val=i.split('/')[0];
	if val=='db':
		print 1
	else: 
		print 0

