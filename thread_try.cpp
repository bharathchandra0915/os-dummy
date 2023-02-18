#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>  
#include <chrono>
#include <thread>
#include <future> 

using namespace std;

void img_to_blur( vector<vector<vector<int>>> image, vector<vector<vector<int>>> &blur_image )
{
    int height  = image.size();
    int width = image[0].size() ;
    
    blur_image.resize(height-2, vector<vector<int>>(width-2, vector<int>(3)));
    cout << blur_image.size();
    // vector<vector<vector<int>>> blur_image(height - 2, vector<vector<int>>(width - 2, vector<int>(3)));
   
    /* taking a kernel of radius 1 i.e matrix of 3*3 is formed

        1 1 1
        1 1 1
        1 1 1 
    */

    for(int i = 1; i< height - 1; i++) // i = 1 and i = height -1 are neglected since they doesn't have neighbours
    {
        for (int j = 1; j< width - 1; j++)
        {
            // cout <<i <<" " <<j<< endl;
            int r = (image[i-1][j-1][0] + image[i-1][j][0] + image[i-1][j+1][0] +
                     image[i][j-1][0] + image[i][j][0] + image[i][j+1][0] +
                     image[i+1][j-1][0] + image[i+1][j][0] + image[i+1][j+1][0])/9;

            int g = (image[i-1][j-1][1] + image[i-1][j][1] + image[i-1][j+1][1] +
                     image[i][j-1][1] + image[i][j][1] + image[i][j+1][1] +
                     image[i+1][j-1][1] + image[i+1][j][1] + image[i+1][j+1][1])/9;
            
            int b = (image[i-1][j-1][2] + image[i-1][j][2] + image[i-1][j+1][2] +
                     image[i][j-1][2] + image[i][j][2] + image[i][j+1][2] +
                     image[i+1][j-1][2] + image[i+1][j][2] + image[i+1][j+1][2])/9;

            blur_image[i-1][j-1][0] = r;
            blur_image[i-1][j-1][1] = g;
            blur_image[i-1][j-1][2] = b;

            // cout << r <<" " <<g <<" " <<b << endl;
            
        }
        
    } 
    // return blur_image;
}


void img_to_gray( vector<vector<vector<int>>> image, vector<vector<vector<int>>> &gray_image )
{
    
    int height  = image.size() ;
    int width = image[0].size() ;
    
    gray_image.resize(height, vector<vector<int>>(width, vector<int>(3)));

    // vector<vector<vector<int>>> gray_image(height, vector<vector<int>>(width, vector<int>(3)));
   
    // string filename = "outfile.txt";
    // ifstream file(filename);
    // if (!file.is_open()) {
    //     cerr << "Error opening file" << endl;
    //     exit(1);
    // }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int r, g, b;
            // file >> r >> g >> b;
            r = image[i][j][0] ;
            g =  image[i][j][1] ;
            b =  image[i][j][2];
            int num = (r+g+b)/3;
            gray_image[i][j][0] = num ;
            gray_image[i][j][1] = num;
            gray_image[i][j][2] = num ;


        }
    }
    // return gray_image ; 


}

void print_to_file(vector<vector<vector<int>>> image, char *file )
{
        
    int height  = image.size();
    int width = image[0].size() ;
    
    ofstream Myfile(file) ;

    Myfile << "P3\n";
    Myfile << to_string(width)+" ";
    Myfile << to_string(height)+"\n";
    Myfile << "255\n";
    
    for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
            string str = to_string(image[i][j][0]) + " " + to_string(image[i][j][1]) + " " + to_string(image[i][j][2]) + "\t";
            Myfile << str;
            // cout << bgr_to_blur_image[i][j][0] << " " << bgr_to_blur_image[i][j][1] << " " << bgr_to_blur_image[i][j][2] << "\t";
        }
        Myfile << "\n";
    }
    Myfile.close();

}


int main(int argc, char** argv)
{
    // string filename = "images/bill.ppm";
    string filename = argv[1];
    int height = 0, width = 0;
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        exit(1);
    }
    
    // Read the header information
    string magic_number;
    file >> magic_number; // reading the first line; 
    
    // cout << magic_number ;
    if (magic_number != "P3") {
        cerr << magic_number <<" Invalid file format" << endl;
        exit(1);
    }
    file >> width;
    file >> height;
    // printf("Height %d Width %d \n",height, width);
    int max_val;
    file >> max_val;
    if (max_val != 255) {
        cerr << "Unsupported color format" << endl;
        exit(1);
    }

    vector<vector<vector<int>>> image(height, vector<vector<int>>(width, vector<int>(3)));
    vector<vector<vector<int>>> gray_image;
    vector<vector<vector<int>>> blur_image;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int r, g, b;
            file >> r >> g >> b;
            image[i][j][0] = r;
            image[i][j][1] = g;
            image[i][j][2] = b;
        }
    }
    file.close();
   
    // printf("Height %d Width %d \n",height, width);

    //// Transformations begin here, so start the time here
    auto start = chrono::high_resolution_clock::now();

    // vector<vector<vector<int>>> blur_image = img_to_blur(image);
    // vector<vector<vector<int>>> gray_image = img_to_gray(blur_image);

    thread t1(img_to_blur, image, ref(blur_image));
    thread t2(img_to_gray, blur_image, ref(gray_image));

    t1.join();
    t2.join();

    // end the time and find the difference
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast< chrono::milliseconds>(end - start);
    // cout << "Time taken: " << duration.count() << " milliseconds." <<endl;

    //// printing needs not to be calculated in time taken
    print_to_file(gray_image, argv[2]);
    
}
