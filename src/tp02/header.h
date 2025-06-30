
// header.h

#ifndef HEADER_H
#define HEADER_H

typedef struct{
    float x, y, z;
} Coord; 

typedef struct{
    int point_count;
    float meanX, meanY, meanZ;
    float stddevX, stddevY, stddevZ;
    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;
} Analise;

/*typedef struct {
    Analise Stats;
    Coord* PointCloud1;//buffer1
    Coord* PointCloud2;//buffer2
    Coord* PointCloud3;//buffer3
    //pthread_mutex_t lock1;//args1
    //pthread_mutex_t lock2;//args2
    int file_num;
} threadArgs;*/

//Exc1
void readPointCloud(const char* filename, Analise* stats, Coord** points);
//Exc2
void preProcessPointCloud(Coord** in_points, Coord** out_points, int* point_count);//in points * porque não vamos escrever 
//Exc3
void identifyDrivableArea(Coord** in_points, Coord** out_points, int* point_count);//in points * porque não vamos escrever 
//AUX
void writeCoordToFile(const Coord* coord, const char* filename, int count);
void checkFileAccess(const char* filename);


#endif 
