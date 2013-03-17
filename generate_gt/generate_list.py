import sys

# db_or_not cam_array.txt
db_list = [];
f = open(sys.argv[1]);

for i in f.readlines():
	db_list.append(int(i.strip('\n')));


f.close();

f = open(sys.argv[2]);

count=0;
for i in f.readlines():
	if(db_list[count]==0):
		print count,
		res = i.split(' ');
		
		cnt =0;
		for j in range(0,len(res)):
			if j%2==0 and db_list[int(res[j])] == 1 and cnt <10:
				print res[j],
				cnt = cnt + 1;
			if cnt == 10:
				break;
		print '\n',
			
		

	count=count+1;

