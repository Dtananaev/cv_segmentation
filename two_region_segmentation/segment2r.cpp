/*
 * File: segment2r.cpp
 *
 * Author: Denis Tananaev
 *
 * Date: 22.09.2016 
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
#include "CFilter.h"
#include "CMatrix.h"
#include "CVector.h"
#include "CTensor.h"
#include "CTensor4D.h"
#include "NMath.h"
#include <iostream>

//Dirichlet boundary conditions
CMatrix<float> Dirichlet_bound(CMatrix<float> aImage,int border_size){

	CMatrix<float> result(aImage.xSize()+border_size*2,aImage.ySize()+border_size*2);
	result.fill(0);
	//center matrix
	for(int x=0;x<aImage.xSize();x++)
		for(int y=0;y<aImage.ySize();y++)
		{
			result(x+border_size,y+border_size)=aImage(x,y);
		}

return result;

}

//Cut Neumann boundaries
void cut(CMatrix<float>& image,int border_size)
{ 
	CMatrix<float> realimage(image.xSize()-2*border_size,image.ySize()-2*border_size);
	for(int x=0;x<realimage.xSize();x++)
		for(int y=0; y<realimage.ySize();y++)
		{
			realimage(x,y)=image(x+border_size,y+border_size);
		}
		image=image(image.xSize()-2*border_size,image.ySize()-2*border_size);
		image=realimage;
		//return realimage;
}
// Neumann boundry conditions
CMatrix<float> Neumann_bound(CMatrix<float> aImage,int border_size)
{
	CMatrix<float> result(aImage.xSize()+border_size*2,aImage.ySize()+border_size*2);
	result.fill(0);
	//center matrix
	for(int x=0;x<aImage.xSize();x++)
		for(int y=0;y<aImage.ySize();y++)
		{
			result(x+border_size,y+border_size)=aImage(x,y);
		}
		//Top
		for(int x=0;x<aImage.xSize();x++)
			for(int y=0;y<border_size;y++)
			{
				result(x+border_size,y)=aImage(x,border_size-1-y);
			}
			//Bottom
			for(int x=0;x<aImage.xSize();x++)
				for(int y=0;y<border_size;y++)
				{
					result(x+border_size,y+aImage.ySize()+border_size)=aImage(x,aImage.ySize()-1-y);
				}
				//left side
				for(int x=0;x<border_size;x++)
					for(int y=0;y<aImage.ySize();y++)
					{
						result(x,y+border_size)=aImage(border_size-1-x,y);
					}

					//right side
					for(int x=0;x<border_size;x++)
						for(int y=0;y<aImage.ySize();y++)
						{
							result(x+aImage.xSize()+border_size,y+border_size)=aImage(aImage.xSize()-1-x,y);
						}
						//up left square
						for(int x=0;x<border_size;x++)
							for(int y=0;y<border_size;y++)
							{
								result(x,y)=aImage(0,0);
							}
							//up right square
							for(int x=aImage.xSize()-1;x<(aImage.xSize()+border_size);x++)
								for(int y=0;y<border_size;y++)
								{
									result(x+border_size,y)=aImage(aImage.xSize()-1,0);
								}
								//down left square
								for(int x=0;x<border_size;x++)
									for(int y=aImage.ySize()-1;y<(aImage.ySize()+border_size);y++)
									{
										result(x,y+border_size)=aImage(0,aImage.ySize()-1);
									}
									//down right square
									for(int x=aImage.xSize()-1;x<(aImage.xSize()+border_size);x++)
										for(int y=aImage.ySize()-1;y<(aImage.ySize()+border_size);y++)
										{
											result(x+border_size,y+border_size)=aImage(aImage.xSize()-1,aImage.ySize()-1);
										}		
										return result;
}

//  diff -0,5 0 0,5
void diffXY(CMatrix<float> Image,  CMatrix<float> &dx, CMatrix<float> &dy){
       int w =Image.xSize();
       int h =Image.ySize();
       Image=Neumann_bound(Image,1);
    dx.fill(0);
    dy.fill(0);
    for(int x=0; x<w; x++)
    		for(int y=0; y<h; y++) {
            int a=x+1;
            int b=y+1;
            dx(x,y)=-0.5*Image(a-1,b)+0.5*Image(a+1,b);
            dy(x,y)=-0.5*Image(a,b-1)+0.5*Image(a,b+1);     
 
    }

}

void clamp(CMatrix<float> &u){

     for(int x=0;x<u.xSize();x++)
		for(int y=0;y<u.ySize();y++)
		{
            if(u(x,y)>= 0.5){

                u(x,y)=255;
            }else{
                
                u(x,y)=0;
            }


        }
}
//  diff -1 1
void difForwXY(CMatrix<float> Image,  CMatrix<float> &dx, CMatrix<float> &dy){
       int w =Image.xSize();
       int h =Image.ySize();
       Image=Neumann_bound(Image,1);
    dx.fill(0);
    dy.fill(0);
    for(int x=0; x<w; x++)
    		for(int y=0; y<h; y++) {
            int a=x+1;
            int b=y+1;
            dx(x,y)=-1*Image(a,b)+1*Image(a+1,b);
            dy(x,y)=-1*Image(a,b)+1*Image(a,b+1);     
 
    }

}
CMatrix<float> segmentation2r(CMatrix<float> image, float mean1, float mean2 ){
    float w=1;
    float v=700;
    float e=0.001;
    int border=3;
	float treshold=0.0000001;

    int number_of_pixels=image.xSize()*image.ySize();
    //init label matrix
  //  CMatrix<float> u (image);
  //  u.normalize(0,1);
    CMatrix<float> u (image.xSize(),image.ySize(),0);
    u.fillRect(1,image.xSize()/4 ,image.ySize()/4, 3*image.xSize()/4, 3*image.ySize()/4);
    CMatrix<float> u_new(image.xSize(),image.ySize(),0);
    CMatrix<float> dx(image.xSize(),image.ySize(),0);
    CMatrix<float> dy(image.xSize(),image.ySize(),0);   
    diffXY(image, dx,dy);
      u=Neumann_bound(u,border);
      image=Neumann_bound(image,border);
      u_new=Neumann_bound(u_new,border);
      dx=Neumann_bound(dx,border);
      dy=Neumann_bound(dy,border);   
   
    float diff_u;

    int counter=0;

    do{

        diff_u=0;
        

    for(int x=border;x<u.xSize()-border;x++)
		for(int y=border;y<u.ySize()-border;y++)
		{

         
        double g_1,g_2,g_3,g_4;

/*
        g_1 = 0.5* (A + B )
        g_2 = 0.5* (A + C )
        g_3 = 0.5* (A + D )   
        g_4 = 0.5* (A + E ) 
*/
   //Calculating A  = g(x,y)    

    
        int a = x;
        int b = y;

       double gradU =  0.5* (  u(a+1,b)-u(a-1,b)  + u(a,b+1)-u(a,b-1) );


       double A = 1/sqrt(   gradU*gradU  +e*e  );


        //Calculating B = g(x+1,y)  
         a = x+1;  b = y;

         gradU =  0.5* (  u(a+1,b)-u(a-1,b)  + u(a,b+1)-u(a,b-1) );
  
        
       double B = 1/sqrt(    gradU*gradU  +e*e  );

        //Calculating C = g(x-1,y)  
         a =x-1; b = y;
 
          gradU =  0.5* (  u(a+1,b)-u(a-1,b)  + u(a,b+1)-u(a,b-1) );


       double C = 1/sqrt(   gradU*gradU  +e*e  );

        //Calculating D = g(x,y+1)  
         a =x; b = y+1;
    
         gradU =  0.5* (  u(a+1,b)-u(a-1,b)  + u(a,b+1)-u(a,b-1) );


        
       double D = 1/sqrt(    gradU*gradU  +e*e  );

        //Calculating E = g(x,y-1)  
         a =x; b =y-1;
 
          gradU =  0.5* (  u(a+1,b)-u(a-1,b)  + u(a,b+1)-u(a,b-1) );

        
        
       double E = 1/sqrt(    gradU*gradU   +e*e  );


    //Calculating our functions for solver and keep them constant for one iteration
       double weight=sqrt(dx(x,y)* dx(x,y) +dy(x,y)*dy(x,y));

       g_1 = 0.5* (A + B )* weight;
       g_2 = 0.5* (A + C )*weight;
       g_3 = 0.5* (A + D )*weight;   
       g_4 = 0.5* (A + E )*weight;

       u_new(x,y)=(1-w)*u(x,y)+ w*(g_2*u_new(x-1,y)+g_4*u_new(x,y-1)  + g_1*u(x+1,y)+g_3*u(x,y+1) +(pow((image(x,y)-mean2),2) -pow((image(x,y)-mean1),2) )/v  )/(g_1+g_2+g_3+g_4);



        if(u_new(x,y)>1){
          u_new(x,y)=1;
        }else if(u_new(x,y)<0){
          u_new(x,y)=0;
        }
             diff_u+= (u_new(x,y)-u(x,y))*(u_new(x,y)-u(x,y));
     
		}    
        diff_u=  diff_u/ number_of_pixels;
         u=u_new;
         cut(u,border);
         u=Neumann_bound(u,border);
         counter+=1;
        if(counter==150){break;}
}while(diff_u >treshold  );
         cut(u_new,border);
clamp( u_new);
    return u_new;   
}



int main(int argc, char** argv) {

	std::string fileNameInput;
	int k=5;
	int iter=2;

	if (argc==2){
		fileNameInput=argv[1];
	
	}else{
		std::cout<<"!!!WRONG INPUT!!!"<<"\n";
		std::cout<<"Usage: 2 region segmentation inputfile" <<"\n";
		std::cout<<"The command should contain at least input file name. The default k=5 , iter=2."<<"\n";
		return 0;    
	}

	std::cout<<"file name is "<<fileNameInput.c_str()<<"\n";


    CMatrix<float> image;
    CMatrix<float> result;
    image.readFromPGM((fileNameInput+".pgm").c_str());
    
    result=segmentation2r(image, 255, 0);

result.writeToPGM((fileNameInput+"_2r.pgm").c_str());    



	return 0;
}

