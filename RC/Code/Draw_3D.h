#ifndef DRAW_3D_h
#define DRAW_3D_h

#include "main.h"                  // Device header
#include "OLED.h"
#include "math.h"
#define M_PI    3.141592
#define X_MAX   4
#define Y_MAX   4
#define Z_MAX   4
#define MAX_MAP_SIZE  32
#define MAX_POINT_NUMBER  168+5*12
#define INT16_MAX 32767 
#define NUM_VERTICES 8 // 立方体有8个顶点
#define MAX_EDGES 3    // 每个顶点最多连接3条边

//uint8_t object_map[X_MAX][Y_MAX][Y_MAX];

//四元数容器
typedef struct {
    float w, x, y, z;
} Quaternion;


//向量容器
typedef struct {
    float x, y, z;
} Vector3;

/*立方体容器相关定义 */


typedef struct {
    int16_t x, y, z;  // 顶点坐标
} Vertex;

typedef struct {
    uint8_t vertex_index; // 连接的顶点索引
} Edge;

typedef struct {
    Vertex vertex;          // 顶点信息
    Edge edges[MAX_EDGES]; // 连接的边
    uint8_t edge_count;     // 边的数量
} VertexNode;


/*相关函数*/

void euler_to_quaternion(float yaw, float pitch, float roll, Quaternion *q);
Quaternion quat_mult(Quaternion q1, Quaternion q2);
Quaternion quat_conjugate(Quaternion q);
Vector3 quat_rotate_vector(Quaternion q, Vector3 v);
Vector3 quat_rotate_vector_arry(Quaternion q, int16_t x,int16_t y,int16_t z);
void Draw_Object(float yaw, float pitch, float roll, int16_t start_x, int16_t start_y, int16_t object_input[X_MAX][Y_MAX][Z_MAX]);
void Draw_Object_By_Map(float yaw, float pitch, float roll, int16_t start_x, int16_t start_y, const int16_t object_input[MAX_MAP_SIZE][3]);
void generate_cube(int16_t edge_length, int16_t object_input[MAX_POINT_NUMBER][3]);
void generate_cube_graph(VertexNode cube_vertices[NUM_VERTICES], int16_t edge_length);
void Draw_Cube_By_Graph(float yaw, float pitch, float roll, int16_t start_x, int16_t start_y, int16_t scale, VertexNode cube_vertices[NUM_VERTICES]);
//进度条函数
void OLED_DrawProgressBar(int16_t X, int16_t Y, int value, int min, int max);
#endif

