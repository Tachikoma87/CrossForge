#!/usr/bin/python

import math 
import sys
#import os

def matmul(m,n,l,x = [],y = []):
	nmat=[]
	i=0


	while i<m:
		k=0
		while k <l:

			res=0
			c=0
			while c<n:
			
				res=res+x[i*m+c]*y[k+l*c]

				
				c+=1
			nmat.append(res)
		
			k+=1
	
		i+=1
			


	#nmat=[x[0]*y[0]+x[1]*y[4]+x[2]*y[8]+x[3]*y[12],x[0]*y[1]+x[1]*y[5]+x[2]*y[9]+x[3]*y[13],x[0]*y[2]+x[1]*y[6]+x[2]*y[10]+x[3]*y[14],x[0]*y[3]+x[1]*y[7]+x[2]*y[11]+x[3]*y[15],
	#x[4]*y[0]+x[5]*y[4]+x[6]*y[8]+x[7]*y[12],x[4]*y[1]+x[5]*y[5]+x[6]*y[9]+x[7]*y[13],x[4]*y[2]+x[5]*y[6]+x[6]*y[10]+x[7]*y[14],x[4]*y[3]+x[5]*y[7]+x[6]*y[11]+x[7]*y[15],
	#x[8]*y[0]+x[9]*y[4]+x[10]*y[8]+x[11]*y[12],x[8]*y[1]+x[9]*y[5]+x[10]*y[9]+x[11]*y[13],x[8]*y[2]+x[9]*y[6]+x[10]*y[10]+x[11]*y[14],x[8]*y[3]+x[9]*y[7]+x[10]*y[11]+x[11]*y[15],
	#x[12]*y[0]+x[13]*y[4]+x[14]*y[8]+x[15]*y[12],x[12]*y[1]+x[13]*y[5]+x[14]*y[9]+x[15]*y[13],x[12]*y[2]+x[13]*y[6]+x[14]*y[10]+x[15]*y[14],x[12]*y[3]+x[13]*y[7]+x[14]*y[11]+x[15]*y[15]]
	
	return nmat


class Element:
	def __init__(self,name,parent,offset,channel,endsite):
		self.parent =parent
		self.offset=offset
		self.channel=channel
		self.endsite=endsite
		self.name=name
		self.transmatrix=[0,0,0,0,0,0,0,0,0]
		self.position=[0,0,0]
		#self.rotation=[0,0,0]


	def transmat(self,b,a,c):

		amat=[1,0,0,
		0,math.cos(a),-math.sin(a),
		0,math.sin(a),math.cos(a)]

		bmat=[math.cos(b),0,math.sin(b),
		0,1,0,
		-math.sin(b),0,math.cos(b)]

		
		cmat=[math.cos(c),-math.sin(c),0,
		math.sin(c),math.cos(c),0,
		0,0,1 ]
		bamat=matmul(3,3,3,bmat,amat)
		abcmat=matmul(3,3,3,bamat,cmat)
		self.transmatrix=abcmat

		#[abcmat[0],abcmat[1],abcmat[2],self.offset[0],
		#abcmat[3],abcmat[4],abcmat[5],self.offset[1],
		#abcmat[6],abcmat[7],abcmat[8],self.offset[2],
		#0,0,0,1]

		#[math.cos(b)*math.cos(c)+math.sin(a)*math.sin(b)*math.sin(c),math.cos(c)*math.sin(a)*math.sin(b)-math.cos(b)*math.sin(c),math.cos(a)*math.sin(b),
		#math.cos(a)*math.sin(c),math.cos(a)*math.cos(c),-math.sin(a),
		#-math.cos(c)*math.sin(b)+math.cos(b)*math.sin(a)*math.sin(c),math.cos(b)*math.cos(c)*math.cos(a)+math.sin(b)*math.sin(c),math.cos(a)*math.cos(b)]




		
		
		#outfile.write(str(self.transmatrix))
		#outfile.write(str('||'+str(self.transmatrix)+'||'))
		#[math.cos(b)*math.cos(c)-math.sin(a)*math.sin(b)*math.sin(c),math.cos(a)*math.cos(b)*math.sin(c)+math.sin(a)*math.cos(c),-math.cos(a)*math.sin(b),float(self.offset[0]),
		#-math.sin(a)*math.cos(b)*math.cos(z)-math.cos(a)*math.sin(c),-math.sin(a)*math.cos(b)*math.sin(c)+math.cos(a)*math.cos(c),math.sin(a)*math.sin(b),float(self.offset[1]),
		#math.sin(b)*math.cos(c),math.sin(b)*math.sin(c),math.cos(b),float(self.offset[2]),
		#0,0,0,1]

for filename in sys.argv[1:]:
	#original_stdout = sys.stdout
	#os.rename(filename,filename)
	hierarchy=[]
	parcount=-1; #count to determine which is the correct parent
	infile = open(filename, 'r')

	nfilename=filename.replace('.bvh','.trc')
	outfile = open(nfilename, 'w') 
	
	#data =infile.read()
	
	motion='false'
	frames=1
	frametime=1
	cframe=0
	count=0
	lines = infile.readlines()
	parentlist=[]
	factor=0.0
	itm=0
	factorlist=['Hips','Chest','Neck','Head','RightHip','RightKnee','RightAnkle','RightToe','RightToeEnd']


	for line in lines:
		if motion=='false':
			
			if 'OFFSET' in line:
				if len(hierarchy)==1:
					hierarchy[-1].parent='null'
					hierarchy[-1].offset=line.replace('OFFSET ','').split()
					cf=0
					while cf<3:
						hierarchy[-1].offset[cf]=float(hierarchy[-1].offset[cf])
						cf+=1	
				else:
					hierarchy[-1].parent=parentlist[-2]
					hierarchy[-1].offset=line.replace('OFFSET ','').split()
					cf=0
					while cf<3:
						hierarchy[-1].offset[cf]=float(hierarchy[-1].offset[cf])
						cf+=1
						
					for item in factorlist:
						if item in hierarchy[-1].name:
							if hierarchy[-1].offset[1]<0:
								factor=factor-hierarchy[-1].offset[1]
								break
							else:
								factor=factor+hierarchy[-1].offset[1]
								break
					
					
			elif 'CHANNELS' in line:
				spln=line.split()
				hierarchy[-1].channel=int(spln[1])
			elif '{' in line:
				parcount+=1
				parentlist.append(parcount)
				
			elif '}' in line:
				parentlist.pop()
				
			elif 'Frames:' in line:
				frames=int(line.replace('Frames:',''))
			elif 'Time:' in line:
				frametime=float(line.replace('Frame Time:','')) 
				#HEADER FOR TRC FILE after all Header information gathered
				nfn =	nfilename.split('\\')
				outfile.write('PathFileType\t4\t(X,Y,Z)\t'+nfn[-1]+'\n'  )
				outfile.write('DataRate\tCameraRate\tNumFrames\tNumMarkers\tUnits\tOrigDataRate\tOrigDataStartFrame\tOrigNumFrames\n')
			
				datarate=round(1/frametime)

				outfile.write(str(datarate)+'\t'+str(datarate)+'\t'+str(frames)+'\t'+str(len(hierarchy))+'\tmm\t'+str(datarate)+'\t1\t'+str(frames)+'\n') 
				outfile.write('Frame#\tTime\t')

				for items in hierarchy:
					outfile.write(items.name+'\t\t\t')

				#V.Sacral\t\t\tSternum\t\t\tTop.Head\t\t\tR.Acromium\t\t\tR.Elbow\t\t\tR.Wrist.Med\t\t\tL.Acromium\t\t\tL.Elbow\t\t\tL.Wrist.Med\t\t\tR.ASIS\t\t\tR.Knee.Lat\t\t\tR.Ankle.Med\t\t\tR.Toe.Med\t\t\tR.Toe.Tip\t\t\tL.ASIS\t\t\tL.Knee.Lat\t\t\tL.Ankle.Med\t\t\tL.Toe.Med\t\t\tL.Toe.Tip\n') #TODO: How to name the Markers and if they should named dynamically
				outfile.write('\n\t')	
				k=1
				for i in range(len(hierarchy)):
					
					outfile.write('\tX'+str(k)+'\tY'+str(k)+'\tZ'+str(k))
					k+=1
				outfile.write('\n\n')
				#Factor to scale the model for OpenSim
				factor=1760/factor
		
				motion='true'
			elif 'JOINT' in line:
				current=line.replace('JOINT ','')
				current=current.replace('\n','')
				hierarchy.append(Element(current,'null',[0,0,0],3,'false'))
				
			elif 'End Site' in line:
				hierarchy.append(Element(current+'End','null',[0,0,0],0,'true'))
			elif 'ROOT' in line:
					current=line.replace('ROOT ','')
					current=current.replace('\n','')
					hierarchy.append(Element(current,'null',[0,0,0],6,'false'))

				
				
				
		else:
			#Lines for Data of each frame
			if cframe==0:
				for item in hierarchy:
					item.offset[0]=factor*item.offset[0]
					item.offset[1]=factor*item.offset[1]
					item.offset[2]=factor*item.offset[2]
			cframe+=1
			outfile.write(str(cframe)+'\t'+str((cframe-1)*frametime))  #cframe or cframe-1
			#Coordinate Calculation for each marker at the Current Frame
			sline=line.split()
			cmarker=0
			testcount=1
			itm=0
			for marker in hierarchy:
				itm+=1
				
				#Calculation of marker Data
				if marker.channel==6:
					ca=	cmarker
					cb=	cmarker+1
					cc= cmarker+2
					marker.transmat(float(sline[ca+3])*math.pi/180,float(sline[cb+3])*math.pi/180,float(sline[cc+3])*math.pi/180)
					#marker.rotation=[float(sline[ca+3]),float(sline[cb+3]),float(sline[cc+3])]
					#outfile.write(str(marker.transmatrix))
					marker.position=[factor*float(sline[ca]),factor*float(sline[cb]),factor*float(sline[cc])]
					#marker.transmatrix[12]=marker.position[0]
					#marker.transmatrix[13]=marker.position[1]
					#marker.transmatrix[14]=marker.position[2]
					#if not (itm==2 or  itm==3 or  itm==5 or  itm==6 or  itm==7 or  itm==9 or  itm==13 or  itm==14 or  itm==18):
					outfile.write('\t'+str(marker.position[0])+'\t'+str(marker.position[1])+'\t'+str(marker.position[2]))
				else:
					if marker.endsite=='true':
						marker.transmat(0,0,0)

					else:
						#marker.rotation=[float(sline[cmarker]),float(sline[cmarker+1]),float(sline[cmarker+2])]
						#marker.transmat(hierarchy[marker.parent].rotation[0],hierarchy[marker.parent].rotation[1],hierarchy[marker.parent].rotation[2])
						marker.transmat(float(sline[cmarker])*math.pi/180,float(sline[cmarker+1])*math.pi/180,float(sline[cmarker+2])*math.pi/180)
						#outfile.write('CHILD('+str(marker.transmatrix)+')')
						#outfile.write('PARENT['+str(hierarchy[marker.parent].transmatrix)+']')
					marker.transmatrix=matmul(3,3,3,hierarchy[marker.parent].transmatrix,marker.transmatrix)
					
					
					posend=matmul(3,3,1,hierarchy[marker.parent].transmatrix,marker.offset)

					ac=0
					while ac<3:
						marker.position[ac]=hierarchy[marker.parent].position[ac]+posend[ac]#-posadd[ac]			
						ac+=1
							
					if 'Ankle' in marker.name:
						tr=matmul(3,3,1,hierarchy[marker.parent].transmatrix,[0,0,0])#[0,factor*-7.939528,0])
						outfile.write('\t'+str(marker.position[0]+tr[0])+'\t'+str(marker.position[1]+tr[1])+'\t'+str(marker.position[2]+tr[2]))
					else:
						outfile.write('\t'+str(marker.position[0])+'\t'+str(marker.position[1])+'\t'+str(marker.position[2]))
				cmarker=cmarker+marker.channel
			outfile.write('\n')	
	infile.close()
	outfile.close()
