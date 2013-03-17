#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <utility>
using namespace std;
int num_images, num_points;
vector < vector < int > > point_vec;
vector < vector < int > > cam_vec;
vector< multimap <int, int> > sorted_arr;

struct camstr
{

        double t[3];
        double R[9];
//      double *

}Camera;
struct camstr* camset;


struct vertex
{
        float m_pos[3];
        float m_color[3];
        double m_num_vis;
        int *sift;

}VertexT;
struct vertex* vertexset;


void ReadBundle(char* filename)
{
        double version;

        printf("Reading Bundle File %s\n",filename);

    printf("[ReadBundleFile] Reading file...\n");

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        printf("Error opening file %s for reading\n", filename);
        return;
    }


    char first_line[256];
    fgets(first_line, 256, f);
    if (first_line[0] == '#') {
        sscanf(first_line, "# Bundle file v%lf", &version);

        printf("[ReadBundleFile] Bundle version: %0.3f\n", version);

        fscanf(f, "%d %d\n", &num_images, &num_points);
    } else if (first_line[0] == 'v') {
        sscanf(first_line, "v%lf", &version);
        printf("[ReadBundleFile] Bundle version: %0.3f\n", version);

        fscanf(f, "%d %d\n", &num_images, &num_points);
    } else {
        version = 0.1;
        sscanf(first_line, "%d %d\n", &num_images, &num_points);
    }

    printf("[SifterApp::ReadBundleFile] Reading %d images and %d points...\n",
        num_images, num_points);

    camset = new struct camstr[num_images];

	for(int i=0;i <num_images; i++)
	{
		vector < int > temp_vec;
		for(int j=0; j<num_images; j++)
		{
			
			temp_vec.push_back(0);
			
		}
		cam_vec.push_back(temp_vec);
	}
			
    for (int i = 0; i < num_images; i++) {
        double focal_length;
        double R[9];
        double t[3];
        double k[2] = { 0.0, 0.0 };

        if (version >= 0.4) {
            char name[512];
            int w, h;
            fscanf(f, "%s %d %d\n", name, &w, &h);
        }

        /* Focal length */
        if (version > 0.1) {
            fscanf(f, "%lf %lf %lf\n", &focal_length, k+0, k+1);
        } else {
            fscanf(f, "%lf\n", &focal_length);
        }

        /* Rotation */
        fscanf(f, "%lf %lf %lf\n%lf %lf %lf\n%lf %lf %lf\n",
            &camset[i].R[0], &camset[i].R[1], &camset[i].R[2], &camset[i].R[3], &camset[i].R[4], &camset[i].R[5], &camset[i].R[6], &camset[i].R[7], &camset[i].R[8]);
        /* Translation */

       // printf("camset: %lf %lf %lf\n%lf %lf %lf\n%lf %lf %lf\n",
         //   camset[i].R[0], camset[i].R[1], camset[i].R[2], camset[i].R[3], camset[i].R[4], camset[i].R[5], camset[i].R[6], camset[i].R[7], camset[i].R[8]);
//      double t[3];
        fscanf(f, "%lf %lf %lf\n", &t[0],&t[1],&t[2]);
        t[0] = -1*t[0];
        t[1] = -1*t[1];
        t[2] = -1*t[2];



        }


        #if 1
    vertexset = new struct vertex[num_points];
    for (int i = 0; i < num_points; i++)
    {

            /* Position */
            fscanf(f, "%f %f %f\n",
                            &vertexset[i].m_pos[0], &vertexset[i].m_pos[1], &vertexset[i].m_pos[2]);

            // if (m_bundle_version < 0.3)
            //     vertexset.m_pos[2] = -vertexset.m_pos12];

            /* Color */
            fscanf(f, "%f %f %f\n",
                            &vertexset[i].m_color[0], &vertexset[i].m_color[1], &vertexset[i].m_color[2]);

                vertexset[i].m_color[0]/=255.0;
                vertexset[i].m_color[1]/=255.0;
                vertexset[i].m_color[2]/=255.0;

//      printf("%f %f %f\n",vertexset[i].m_color[0],vertexset[i].m_color[1],vertexset[i].m_color[2]);
            int num_visible;
            fscanf(f, "%d", &num_visible);
            vertexset[i].m_num_vis=num_visible;


                vector < int > temp_v;
            for (int j = 0; j < num_visible; j++) {
                    int view, key;
                    fscanf(f, "%d %d", &view, &key);

                        temp_v.push_back(view);

                    double x, y;
                    fscanf(f, "%lf %lf", &x, &y);
            }

	for(int j=0; j<temp_v.size(); j++)
	{
		for(int k=0;k<temp_v.size(); k++)
			{
				if(j!=k)
					cam_vec[temp_v[j]][temp_v[k]]++;
			}
		}
                point_vec.push_back(temp_v);
    }


#endif


    fclose(f);




}


int main(int argc, char* argv[])
{

ReadBundle(argv[1]);

	multimap < int, int > temp_arr;
	multimap < int, int >::reverse_iterator it;

for(int j=0;j<cam_vec.size(); j++)
{
	temp_arr.clear();
	for(int k=0;k<cam_vec.size();k++)
	{
		temp_arr.insert( std::pair<int, int> (cam_vec[j][k], k) );
	}


	for(it=temp_arr.rbegin();it!=temp_arr.rend(); it++)
	{
		cout << (*it).second << " " << (*it).first << " ";
	}
	cout << endl;
}


return 0;

}
