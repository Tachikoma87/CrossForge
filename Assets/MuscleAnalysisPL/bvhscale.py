#!/usr/bin/python

import math 
import sys







for filename in sys.argv[1:]:

	infile = open(filename, 'r')

	nfilename=filename.replace('.bvh','_scaled.bvh')
	outfile = open(nfilename, 'w') 
	
	#data =infile.read()
	
	motion='false'
	
	joint=1
	lines = infile.readlines()
	size=0.0


	for line in lines:
			
			if 'OFFSET' in line:
					if joint==1:
						offset=line.replace('OFFSET ','').split()
						offset[1]=float(offset[1])
						if offset[1]<0:
							size=size-offset[1]
						else:
							size=size+offset[1]

			elif 'JOINT' in line:
				name=line.replace('JOINT ','')
				name=name.replace('\n','')
				if 'Collar' in name:
					joint=0
				elif 'Left' in name:
					joint=0
				elif 'Hip' in name:
					joint=1
			elif 'MOTION' in line:
				break

	factor=1.83/size
	motion=0
	spaces=0
	for line in lines:

		if 	motion==0:
			if 'OFFSET' in line:
				offset=line.replace('OFFSET ','').split()
				outfile.write(' '*spaces)
				outfile.write('OFFSET '+str(format(float(offset[0])*factor,'.6f' ))+' '+str(format(float(offset[1])*factor,'.6f' ))+' '+str(format(float(offset[2])*factor,'.6f' ))+'\n')

			elif '{' in line:
				outfile.write(line)
				spaces+=1
			elif '}' in line:
				outfile.write(line)
				spaces-=1
			elif 'Time:' in line:
				motion=1
				outfile.write(line)
			else:
				outfile.write(line)
		else:
			nline=line.split()
			b=0
			while b<3:
				nline[b]=float(nline[b])*factor
				b+=1
			for item in nline:
				outfile.write(str(format(float(item),'.6f'))+' ')
			outfile.write('\n')

	
	infile.close()
	outfile.close()