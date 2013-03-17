#include <iostream>
#include "stdio.h"
unsigned char* siftKeys;
unsigned char* vertexKeys;
float* vertexinfo;
int* colorinfo;
float* vertexquality;

int main(int argc, char* argv[])
{
	
        printf("Reading Model\n");

        FILE* fp = fopen(argv[1],"r");
	
	int num_points, desc_size;
        fscanf(fp,"%d %d",&num_points, &desc_size);

        int num;
        siftKeys = new unsigned char[128*num_points];
        vertexinfo = new float[3*num_points];
        colorinfo = new int[3*num_points];
        vertexquality = new float[num_points];

      //  vertexKeys = new unsigned char[3*num_points];

	float x,y,scale,orient;
        for(int i=0;i<num_points;i++)
        {

                fscanf(fp,"%f %f %f %f\n",&x,&y,&scale,&orient);


                for(int j=0;j<128;j++)
                {
                        fscanf(fp,"%d", &num);
                        siftKeys[128*i+j]= (char)(num);
		//	printf("%d ",num);
                }
	//	printf("\n");

        }



  FILE* pFile = fopen(argv[2],"w");
  fwrite (siftKeys , 1 , num_points*128 , pFile );
  fclose (pFile);
	


}

