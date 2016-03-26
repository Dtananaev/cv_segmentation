/*
* kmeans.cpp
*
*  Created on: march 26, 2015
*      Author: Denis Tananaev
*/

#include "CMatrix.h"
#include <stdlib.h> 
#include <ctime>
#include <cmath>
#include <string>
#include "CTensor.h"
#include <vector>
#include <algorithm> 
#include <string>



typedef unsigned char uchar;
typedef struct {
	uchar x;
	uchar y;
	uchar z;
} uchar4;

typedef uchar4 Color;

typedef struct {
	float x;
	float y;
	float z;
} pixel;





void CTensorToColorCMatrix(CMatrix<Color>& out, const CTensor<unsigned char>& in){
	out.setSize(in.xSize(), in.ySize());
	for( int y = 0; y < out.ySize(); ++y )
		for( int x = 0; x < out.xSize(); ++x )
		{
			out(x,y).x = in(x,y,0); // R
			out(x,y).y = in(x,y,1); // G
			out(x,y).z = in(x,y,2); // B
		}
}


void ColorCMatrixToCTensor(CTensor<unsigned char>& out, const CMatrix<Color>& in){

	out.setSize(in.xSize(), in.ySize(), 3);
	for( int y = 0; y < out.ySize(); ++y )
		for( int x = 0; x < out.xSize(); ++x )
		{
			out(x,y,0) = in(x,y).x; // R
			out(x,y,1) = in(x,y).y; // G
			out(x,y,2) = in(x,y).z; // B
		}
}




//Initialization of k-mean on C++
std::vector<pixel> init_centroids(CMatrix<Color> image, int k){//here k -the number of the centroids
	std::vector<pixel> centroids;
	for (int i=0; i<k; i++){

		int x=rand() %image.xSize();
		int y=rand() %image.ySize(); 

		pixel temp;
		temp.x=image(x,y).x;  
		temp.y=image(x,y).y;  
		temp.z=image(x,y).z;  

		centroids.push_back(temp);   

	}
	return centroids;
}


//calculate distance to each centroid and output the matrix with indecis of the pixels with shortest distace to centroid
void distance(CMatrix<Color> image, std::vector<pixel>& centroids, CMatrix<int>& index_matrix){

	std::vector<float> distance_to_centroids;// vector with distances to each centroid

	for(int x=0;x<image.xSize();x++)
		for(int y=0;y<image.ySize();y++)//go through whole image
		{

			distance_to_centroids.clear();

			for(int i=0; i< centroids.size(); i++){
				float distx=image(x,y).x-centroids[i].x;
				float disty=image(x,y).y-centroids[i].y;
				float distz=image(x,y).z-centroids[i].z;
				float dist=distx*distx+disty*disty+distz*distz;

				distance_to_centroids.push_back(dist);

			}  
			index_matrix(x,y)= std::min_element( distance_to_centroids.begin(),  distance_to_centroids.end()) -  distance_to_centroids.begin();//find the index of the minimum distance and write this index to the index matrix (index-related to the centroid number)

		}



}



void update_centroids(CMatrix<Color> image, CMatrix<int> index_matrix,  std::vector<pixel>& centroids, int k){

	//make centroids zero
	pixel temp={0};  //temp.x=0;  temp.y=0;  temp.z=0;   
	std::fill(centroids.begin(), centroids.end(),temp);

	//create counter
	std::vector<int> counter(k, 0);

	for(int x=0;x<image.xSize();x++)
		for(int y=0;y<image.ySize();y++)//go through whole image
		{
			for(int i =0; i<k; i++){
				centroids[i].x+=(index_matrix(x,y)==i)*image(x,y).x;
				centroids[i].y+=(index_matrix(x,y)==i)*image(x,y).y;  
				centroids[i].z+=(index_matrix(x,y)==i)*image(x,y).z;  

				counter[i]+=(index_matrix(x,y)==i)*1;  
			}  
		}

		//normalize centroids
		for(int i =0; i<k; i++){
			centroids[i].x= centroids[i].x/ counter[i];
			centroids[i].y=centroids[i].y/ counter[i];  
			centroids[i].z=centroids[i].z/ counter[i]; 

		}

}



void visualise_clusters(CMatrix<int> index_matrix, CMatrix<Color>& image,std::vector<pixel> centroids){



	for(int x=0;x<image.xSize();x++)
		for(int y=0;y<image.ySize();y++)//go through whole image
		{
			image(x,y).x=centroids[index_matrix(x,y)].x;
			image(x,y).y=centroids[index_matrix(x,y)].y;
			image(x,y).z=centroids[index_matrix(x,y)].z;      
		}        


}






int main(int argc, char** argv) {

	std::string fileNameInput;
	int k=5;
	int iter=2;

	if (argc==2){
		fileNameInput=argv[1];
	}else if (argc==3){
		fileNameInput=argv[1];
		k=atoi(argv[2]);
	}else if (argc==4){
		fileNameInput=argv[1];
		k=atoi(argv[2]);
		iter=atoi(argv[3]);
	}else{
		std::cout<<"!!!WRONG INPUT!!!"<<"\n";
		std::cout<<"Usage: kmeans inputfile <number of clusters k>  <number of iterations iter>"<<"\n";
		std::cout<<"The command should contain at least input file name. The default k=5 , iter=2."<<"\n";
		return 0;    
	}

	std::cout<<"file name is "<<fileNameInput.c_str()<<"\n";

	// Read the image from disk R G B
	CTensor<unsigned char> tmp;
	tmp.readFromPPM((fileNameInput+".ppm").c_str());

	// Store the image in an appropriate format
	CMatrix<Color> img;
	CTensorToColorCMatrix(img, tmp);
	CMatrix<int> index_matrix(img.xSize(),img.ySize());

	//k-means

	//init centroids
	std::vector<pixel> centroids=init_centroids(img, k);

	//start iterations
	for(int i=0; i<iter; i++){
		distance(img, centroids,index_matrix);
		update_centroids(img,index_matrix, centroids, k); 
	}

	visualise_clusters(index_matrix, img,centroids);

	// Write the inverted inverted image to the disk
	ColorCMatrixToCTensor(tmp, img);
	tmp.writeToPPM((fileNameInput+"_clustered.ppm").c_str());

	return 0;
}




