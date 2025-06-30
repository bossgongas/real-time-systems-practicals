#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>  
#include "header.h"
#include <pthread.h>
#define CELL_SIZE 1   // Assuming 1m x 1m grid cells

int num = 0;

/**************************************************************************
EXC1 - Coleta dos dados 
*/

void readPointCloud(const char* filename, Analise* stats, Coord** points) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
    }

    // Count the number of points in the file
    int new_point_count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        new_point_count++;
    }
    fclose(file);
    
    Coord* temp = realloc(*points, new_point_count * sizeof(Coord));
    if (temp == NULL) {
        perror("Erro readPointCloud -> realocação da memoria");
        return;
    } else {
        *points = temp;
        //printf("\nDentro de readPointCloud: PointCloud1->%p\n", *points);
    }

    // Re-open the file to read the points
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        free(points);
        return;
    }

    // Initialize variables for statistics
    float min_x = FLT_MAX, max_x = FLT_MIN;
    float min_y = FLT_MAX, max_y = FLT_MIN;
    float min_z = FLT_MAX, max_z = FLT_MIN;
    float sumX = 0, sumY = 0, sumZ = 0;
    float sumSqX = 0, sumSqY = 0, sumSqZ = 0;

    // Read points from file and calculate statistics
    int i = 0;
    while (fgets(line, sizeof(line), file) != NULL && i < new_point_count) {
        float x, y, z;
        if (sscanf(line, "%f %f %f", &x, &y, &z) == 3) {
            (*points)[i].x = x;
            (*points)[i].y = y;
            (*points)[i].z = z;

            // Update min/max
            if (x < min_x) min_x = x;
            if (x > max_x) max_x = x;
            if (y < min_y) min_y = y;
            if (y > max_y) max_y = y;
            if (z < min_z) min_z = z;
            if (z > max_z) max_z = z;
            
            // Update sums and sums of squares
            sumX += x;
            sumY += y;
            sumZ += z;
            sumSqX += x * x;
            sumSqY += y * y;
            sumSqZ += z * z;

            i++;
        }
    }
    fclose(file);

    // Calculate statistics
    stats->point_count = new_point_count;
    stats->meanX = sumX / new_point_count;
    stats->meanY = sumY / new_point_count;
    stats->meanZ = sumZ / new_point_count;
    stats->stddevX = sqrt(sumSqX / new_point_count - stats->meanX * stats->meanX);
    stats->stddevY = sqrt(sumSqY / new_point_count - stats->meanY * stats->meanY);
    stats->stddevZ = sqrt(sumSqZ / new_point_count - stats->meanZ * stats->meanZ);
    stats->minX = min_x;
    stats->maxX = max_x;
    stats->minY = min_y;
    stats->maxY = max_y;
    stats->minZ = min_z;
    stats->maxZ = max_z;
}




/*************************************************************************
EXC2 - Preprocessamento do dados
*/
void preProcessPointCloud(Coord** in_points, Coord** out_points, int* point_count) {
    // Definir os limites
    const double radius_threshold = 2.0; 
    const double ahead_threshold = 30.0;  
    const double side_threshold = 10.0;   
    
    //allocar memoria para a nuvem de pontos de saida
    Coord* temp = realloc(*out_points, *point_count * sizeof(Coord));
    if(temp == NULL){
    	perror("Reallocation Failed on preProcess!!\n");
        return;
    }
    else{
    	*out_points = temp;
    	//printf("\nDentro de preProcessPointCloud:   PointCloud1->%p  |  PointCloud2->%p\n", in_points,*out_points);
    }

    int valid_count = 0;
    double dist_from_origin = 0;
    for (int i = 0; i < *point_count; i++) {
        dist_from_origin = sqrt((*in_points)[i].x * (*in_points)[i].x + (*in_points)[i].y * (*in_points)[i].y);
        if (dist_from_origin > radius_threshold && fabs((*in_points)[i].x) <= ahead_threshold && fabs((*in_points)[i].y) <= side_threshold && (*in_points)[i].x >= 0 && (*in_points)[i].z <= 0) {
            if (i != valid_count) {
                (*out_points)[valid_count] = (*in_points)[i];
            }
            valid_count++;
        }
    }

    // Atualizar o valor de point_count
    *point_count = valid_count;
}

/*************************************************************************
EXC3 - Identificação dos pontos pertencentes a estrada.
*/
void identifyDrivableArea(Coord** in_points, Coord** out_points, int* point_count) {
    const double z_threshold_for_drivable_area = 0.2;  
    
    
    //allocar memoria para a nuvem de pontos de saida
    Coord* temp = realloc(*out_points, *point_count * sizeof(Coord));
    if(*out_points == NULL){
    	perror("Reallocation Failed on preProcess!!\n");
        return;
    }
    else{
    	*out_points = temp;
    	//printf("\nDentro de DrivableArea:   PointCloud2->%p  |  PointCloud3->%p\n", in_points,*out_points);
    }

    int valid_count = 0;
    for (int i = 0; i < *point_count; i++) {
        if (fabs((*in_points)[i].z) <= z_threshold_for_drivable_area) {
            if (i != valid_count) {
                (*out_points)[valid_count] = (*in_points)[i];
            }
            valid_count++;
        }
    }

    // Atualizar point_count
    *point_count = valid_count;
}

/*************************************************************************
FUNÇÕES AUXILIARES
*/
// VISUALIZATION PURPOSE
void writeCoordToFile(const Coord* coord, const char* filename, int count){
    
    if (coord == NULL || filename == NULL) {
        fprintf(stderr, "Invalid input to writeCoordToFile\n");
        return;
    }

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < count; ++i) {
        fprintf(file, "%f %f %f\n", coord[i].x, coord[i].y, coord[i].z);
    }

    fclose(file);
}
// DEBUG PURPOSE
void checkFileAccess(const char* filename) {
    if (filename == NULL) {
        printf("Filename string is NULL.\n");
        return;
    }
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    printf("File '%s' opened successfully.\n", filename);
    fclose(file);
}
