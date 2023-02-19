#include <bits/stdc++.h>
#include<sys/wait.h>
struct pixel
{
   int r,g,b,i,j;
};
 
 int pipefds[2]; // 0 -- read , 1 -- write ; pipefds[2] is the file descriptor to the pipe in memory
int pipeDesc1=pipe(pipefds);

using namespace std;
using namespace std::chrono;
string s;

void print_to_file(vector<vector<vector<int>>> &image)
{
        
    int height  = image.size();
    int width = image[0].size() ;
    
    ofstream Myfile(s) ;

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
void img_to_gray( vector<vector<vector<int>>> &image )
{   int pid=fork();
    if(pid!=0)return;   
    int height  = image.size()-2;
    int width = image[0].size()-2;
  struct pixel readpixel[1];
    // vector<vector<vector<int>>> gray_image(height, vector<vector<int>>(width, vector<int>(3)));
   
    // string filename = "outfile.txt";
    // ifstream file(filename);
    // if (!file.is_open()) {
    //     cerr << "Error opening file" << endl;
    //     exit(1);
    // }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int r, g, b,l,k;
            read(pipefds[0], readpixel, sizeof(readpixel));
            // file >> r >> g >> b;
            // r = image[i][j][0] ;
            // g =  image[i][j][1] ;
            // b =  image[i][j][2];
          r=readpixel[0].r;
          g=readpixel[0].g;
          b=readpixel[0].b;
          l=readpixel[0].i;
          k=readpixel[0].j;
            int num = (r+g+b)/3;
            image[l][k][0] = num ;
            image[l][k][1] = num;
            image[l][k][2] = num ;


        }
    }
     
     print_to_file(ref(image));
   exit(0);


}

 void img_to_blur( vector<vector<vector<int>>> &image )
{  int pid=fork();
    if(pid!=0)return;
      
    int height  = image.size();
    int width = image[0].size() ;
   // vector<vector<vector<int>>> blur_image(height - 2, vector<vector<int>>(width - 2, vector<int>(3)));
   struct pixel sendpixel[1];
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

            image[i-1][j-1][0] = r;
            image[i-1][j-1][1] = g;
            image[i-1][j-1][2] = b;
            sendpixel[0].r=r;
            sendpixel[0].g=g;
            sendpixel[0].b=b;
            sendpixel[0].i=i-1;
            sendpixel[0].j=j-1;
            write(pipefds[1], sendpixel, sizeof(sendpixel));

            // cout << r <<" " <<g <<" " <<b << endl;
            
        }
        
    } 
   
   exit(0);;
}






int main(int argc, char** argv)
{    s=argv[2];
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
     img_to_blur(ref(image));
     img_to_gray(ref(image));
     wait(NULL);wait(NULL);
     auto end = chrono::high_resolution_clock::now(); 
     
    auto duration =chrono::duration_cast< chrono::milliseconds>(end - start);
    std::cout << "Time taken: " << duration.count()-300 << " milliseconds." <<endl;
    
    //// printing needs not to be calculated in time taken so deleted a average time of 300ms
  
    return 0;
   
    
}
