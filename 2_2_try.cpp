#include <bits/stdc++.h>
#include <chrono>
#include <cmath>
#include <thread>
#include <vector>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>
#include <fcntl.h>


using namespace std;

#define semaphore_name "/binary_semaphore"
#define INITIAL_VALUE 1
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
string s;
struct pixel{
    int r, g, b;
};

void img_to_gray( key_t endOfT2, key_t key, int pid, int height, int width)
{
    if(pid > 0) return;
    int shmid = shmget(key, sizeof(struct pixel) * height * width, 0666 | IPC_CREAT);
    struct pixel *image;
    image = (struct pixel *)shmat(shmid, NULL, 0);

    sem_t *binary_semaphore = sem_open(semaphore_name, O_RDWR);
    int * go_back;
    int shmid_1 = shmget(endOfT2, sizeof(int) * 2, 0666 | IPC_CREAT);
    go_back = (int *)shmat(shmid_1, NULL, 0);

   

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
           sem_wait(binary_semaphore);
           struct pixel img;
           img = image[i*width + j];
            // file >> r >> g >> b;
            int r1,g1, b1;
            r1 = img.r;
            g1 =  img.g;
            b1 =  img.b;

            int num = (r1+g1+b1)/3;
            img.r = num ;
            img.g = num;
            img.b = num ;

            image[i*width + j] = img;

            go_back[0] = i + 1;
            go_back[1] = j + 1; // Assignment of goBack to signal signal lock

         sem_post(binary_semaphore);
        }
    }
    // return gray_image ; 
    exit(0);


}


void img_to_blur( key_t endOfT2, key_t key, int pid, int height, int width)
{
    
    if(pid !=0) return;
    int shmid = shmget(key, sizeof(struct pixel) * height * width, 0666 | IPC_CREAT);
    struct pixel *image;
    image = (struct pixel *)shmat(shmid, NULL, 0);
    struct pixel img;
    
   
    vector<vector<pixel>> new_image (height, vector<pixel>(width));

    sem_t *binary_semaphore = sem_open(semaphore_name, O_RDWR);

    int *go_back;
    int shmid_1 = shmget(endOfT2, sizeof(int) * 2, 0666 | IPC_CREAT);
    go_back = (int *)shmat(shmid_1, NULL, 0);

    for(int i = 1; i< height - 1; ) // i = 1 and i = height -1 are neglected since they doesn't have neighbours
    {
        for (int j = 1; j< width - 1; )
        {
                sem_wait(binary_semaphore);
                 if(j + 1 <= go_back[1] || i + 1 <= go_back[0])
                {

                     new_image[i][j].r = (image[(i-1)*width+j-1].r + image[(i-1)*width+j].r + image[(i-1)*width+j+1].r +image[(i)*width+j-1].r + image[(i)*width+j].r + image[(i)*width+j+1].r +image[(i+1)*width+j-1].r + image[(i+1)*width+j].r + image[(i+1)*width+j+1].r)/9;

                    new_image[i][j].g = (image[(i-1)*width+j-1].g + image[(i-1)*width+j].g + image[(i-1)*width+j+1].g +image[(i)*width+j-1].g + image[(i)*width+j].g + image[(i)*width+j+1].g +image[(i+1)*width+j-1].g + image[(i+1)*width+j].g + image[(i+1)*width+j+1].g)/9;
            
                    new_image[i][j].b = (image[(i-1)*width+j-1].b + image[(i-1)*width+j].b + image[(i-1)*width+j+1].b +image[(i)*width+j-1].b + image[(i)*width+j].b + image[(i)*width+j+1].b +image[(i+1)*width+j-1].b + image[(i+1)*width+j].b + image[(i+1)*width+j+1].b)/9;

                   i++;j++;

                }
                
                sem_post(binary_semaphore);
                
            // cout <<i <<" " <<j<< endl;
           

            // cout << r <<" " <<g <<" " <<b << endl;
            
        }
       
        
        
        
    } 
      for(int i = height - 1; i>0 ; i--) // i = 1 and i = height -1 are neglected since they doesn't have neighbours
    {
        for (int j = 1; j<= width - 1; j++)
        {
            image[i*width + j] = new_image[i][j];

        }}

               
    // return blur_image; 
    exit(0);
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
    struct pixel *image;

    key_t key = 0x1234;
    int shmid = shmget(key, sizeof(struct pixel) * (height) * (width) , 0666 | IPC_CREAT);
    image = (struct pixel *)shmat(shmid, NULL, 0);

    struct pixel img;
  

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int r, g, b;
            file >> r >> g >> b;
            img.r = r;
            img.g = g ;
            img.b = b;
            image[(i*width) + j] = img;


        }
    }
    file.close();
   
    // printf("Height %d Width %d \n",height, width);

    //// Transformations begin here, so start the time here

    key_t endOfT2  = 0x1235;
    int *go_back;
    int shmid_1 = shmget(endOfT2, sizeof(int) *2, 0666 | IPC_CREAT);
    go_back = (int*) shmat(shmid_1, NULL, 0);

    auto start = chrono::high_resolution_clock::now();

    sem_t *binary_semaphore = sem_open(semaphore_name, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);

    img_to_blur(endOfT2, key, fork(), height, width);
    img_to_gray(endOfT2, key, fork(), height, width);

     wait(NULL); 
     wait(NULL);
    // end the time and find the difference
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast< chrono::milliseconds>(end - start);
    cout << "Time taken: " << duration.count() << " milliseconds." <<endl;

    //// printing needs not to be calculated in time taken
    
    string s = argv[2];
    ofstream Myfile(s) ;

    Myfile << "P3\n";
    Myfile << to_string(width)+" ";
    Myfile << to_string(height)+"\n";
    Myfile << "255\n";
    for (int i = height - 1; i >= 0; i--){
        for (int j = 0; j <= width - 1; j++){
            img = image[(i * width) + j];
            Myfile << to_string(img.r)+" ";
            Myfile << to_string(img.g)+" ";
            Myfile << to_string(img.b)+" ";
            
        }
     Myfile << "\n";
    }
    
    Myfile.close();
    return 0;
    
}
