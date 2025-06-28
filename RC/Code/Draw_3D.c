/***************************************************************************************
  * 本程序由寂静谷联合创建并免费开源共享
  * 你可以任意查看、使用和修改，并应用到自己的项目之中
  * 程序版权归组织寂静谷所有，任何人或组织不得将其据为己有
  * 
  * 程序名称：				基于四元数的3D绘图反馈陀螺仪姿态程序
  * 程序创建时间：			2025.6.17
  * 当前程序版本：			V1.0
  * 当前版本发布时间：		2025.6.17
  *
  * 如果你发现程序中的漏洞或者笔误，可通过邮件向我们反馈：icks49003@gmail.com
  ***************************************************************************************
  */

#include "Draw_3D.h"
#include <math.h>
#include <stdio.h>

//定义静态变量
float cy, sy, cp, sp, cr, sr;

//陀螺仪姿态转四元数
void euler_to_quaternion(float yaw, float pitch, float roll, Quaternion *q) {
    cy = cosf(yaw * 0.5f * (M_PI / 180.0f));
    sy = sinf(yaw * 0.5f * (M_PI / 180.0f));
    cp = cosf(pitch * 0.5f * (M_PI / 180.0f));
    sp = sinf(pitch * 0.5f * (M_PI / 180.0f));
    cr = cosf(roll * 0.5f * (M_PI / 180.0f));
    sr = sinf(roll * 0.5f * (M_PI / 180.0f));

    q->w = cr * cp * cy + sr * sp * sy;
    q->x = sr * cp * cy - cr * sp * sy;
    q->y = cr * sp * cy + sr * cp * sy;
    q->z = cr * cp * sy - sr * sp * cy;
}

Quaternion euler_to_quaternion_1(float yaw, float pitch, float roll)
{
		Quaternion q = {0};
    cy = cosf(yaw * 0.5f * (M_PI / 180.0f));
    sy = sinf(yaw * 0.5f * (M_PI / 180.0f));
    cp = cosf(pitch * 0.5f * (M_PI / 180.0f));
    sp = sinf(pitch * 0.5f * (M_PI / 180.0f));
    cr = cosf(roll * 0.5f * (M_PI / 180.0f));
    sr = sinf(roll * 0.5f * (M_PI / 180.0f));

    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;
		
		return q;
}

//四元数乘法
Quaternion quat_mult(Quaternion q1, Quaternion q2) {
    Quaternion r;
    r.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
    r.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
    r.y = q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x;
    r.z = q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w;
    return r;
}

//四元数共轭
Quaternion quat_conjugate(Quaternion q) {
    Quaternion r = {q.w, -q.x, -q.y, -q.z};
    return r;
}

//四元数旋转向量
Vector3 quat_rotate_vector(Quaternion q, Vector3 v) {
    Quaternion vq = {0, v.x, v.y, v.z};
    Quaternion q_conj = quat_conjugate(q);
    Quaternion result = quat_mult(quat_mult(q, vq), q_conj);
    Vector3 out = {result.x, result.y, result.z};
    return out;
}

//四元数旋转数组
Vector3 quat_rotate_vector_arry(Quaternion q, int16_t x,int16_t y,int16_t z) {
    Quaternion vq = {0, x, y, z};
    Quaternion q_conj = quat_conjugate(q);
    Quaternion result = quat_mult(quat_mult(q, vq), q_conj);
    Vector3 out = {result.x, result.y, result.z};
    return out;
}

//绘制3D物体，第一版，模型可自行制定，但注意单片机Flash大小
//输入数组有固定大小，单片机上不能动态分配，因为会导致内存碎片化，所以结尾以xyz值全为INT16_MAX来标志结束
void Draw_Object(float yaw, float pitch, float roll, int16_t start_x, int16_t start_y, int16_t object_input[X_MAX][Y_MAX][Z_MAX]){
		Quaternion  world = euler_to_quaternion_1(yaw,pitch,roll);
		Vector3 temp;
		int i,j,k;
        OLED_ClearArea(start_x, start_y, start_x + X_MAX - 1, start_y + Z_MAX - 1);

		for(i = 0;i<X_MAX;i++){
				for(j = 0;j<Y_MAX;j++){
						for(k = 0;k<Z_MAX;k++){
								if(object_input[i][j][k]==1){
										temp = quat_rotate_vector_arry(world,i,j,k);
										OLED_DrawPoint((int16_t)temp.y+start_x+5,(int16_t)temp.z+start_y+5);
								}
						}
				}
		}
}

//绘制3D物体，第二版，模型可自行制定，但注意单片机Flash大小
//输入数组有固定大小，单片机上不能动态分配，因为会导致内存碎片化，所以结尾以xyz值全为INT16_MAX来标志结束
//画布大小：63*63，建议起始坐标为（0，63）
void Draw_Object_By_Map(float yaw, float pitch, float roll, int16_t start_x, int16_t start_y, const int16_t object_input[MAX_MAP_SIZE][3]){
        Quaternion  world = euler_to_quaternion_1(yaw,pitch,roll);
        Vector3 temp;
        int i;
        OLED_ClearArea(start_x, start_y, start_x + 50 - 1, start_y + 50 - 1);

        for(i = 0;i<MAX_MAP_SIZE;i++){
                if (object_input[i][0] == INT16_MAX && object_input[i][1] == INT16_MAX && object_input[i][2] == INT16_MAX) {
                    // 终止标记，跳出循环
                    break;
                }
                // 确保坐标在合理范围内
                if (object_input[i][0] < 0 || object_input[i][1] < 0 || object_input[i][2] < 0) {
                    continue; // 跳过无效坐标
                }
                temp = quat_rotate_vector_arry(world, object_input[i][0], object_input[i][1], object_input[i][2]);
                OLED_DrawPoint((int16_t)temp.z + start_x + 15, (int16_t)temp.x + start_y + 15);
        }
}

//生成正方体模型的点阵
//输入数组有固定大小，单片机上不能动态分配，因为会导致内存碎片化，所以结尾以xyz值全为INT16_MAX来标志结束
//请根据画布大小选择合适的边长和点数
void generate_cube(int16_t edge_length, int16_t object_input[MAX_POINT_NUMBER][3]) 
{
    /* 参数校验 */
    if (edge_length < 2) edge_length = 2;  // 最小边长为2（1个点无法构成边）
    
    /* 计算实际可生成的最大边长 */
    const int16_t max_allowed_edge = MAX_POINT_NUMBER / 12;
    if (edge_length > max_allowed_edge) edge_length = max_allowed_edge;
    if (edge_length < 2) return;  // 点数不足以生成任何边

    /* 生成12条边的点 */
    const int16_t max_idx = edge_length - 1;
    int16_t point_count = 0;

    // 辅助宏：安全添加点
    #define SAFE_ADD_POINT(x, y, z) do { \
        if (point_count < MAX_POINT_NUMBER) { \
            object_input[point_count][0] = (x); \
            object_input[point_count][1] = (y); \
            object_input[point_count][2] = (z); \
            point_count++; \
        } \
    } while(0)

    /* 生成12条边（避免重复顶点）*/
    for (int16_t i = 0; i < edge_length; i++) {
        /* 底面4边 (Z=0) */
        if (i >= 0) { // 跳过起点(0,0,0)避免重复
            SAFE_ADD_POINT(i, 0, 0);       // X轴边
            SAFE_ADD_POINT(0, i, 0);       // Y轴边
        }
        if (i < max_idx) { // 跳过终点(max,max,0)避免重复
            SAFE_ADD_POINT(max_idx, i, 0); // 对边X
            SAFE_ADD_POINT(i, max_idx, 0); // 对边Y
        }

        /* 顶面4边 (Z=max_idx) */
        if (i > 0 && i < max_idx) { // 跳过顶点
            SAFE_ADD_POINT(i, 0, max_idx);
            SAFE_ADD_POINT(0, i, max_idx);
            SAFE_ADD_POINT(max_idx, i, max_idx);
            SAFE_ADD_POINT(i, max_idx, max_idx);
        }

        /* 垂直4边 */
        if (i > 0 && i < max_idx) { // 跳过顶点
            SAFE_ADD_POINT(0, 0, i);
            SAFE_ADD_POINT(0, max_idx, i);
            SAFE_ADD_POINT(max_idx, 0, i);
            SAFE_ADD_POINT(max_idx, max_idx, i);
        }
    }

    /* 终止标记（使用INT16_MAX避免冲突）*/
    if (point_count < MAX_POINT_NUMBER) {
        object_input[point_count][0] = INT16_MAX;
        object_input[point_count][1] = INT16_MAX;
        object_input[point_count][2] = INT16_MAX;
    }
}

//运用图，根据角点坐标在OLED上绘制立方体，大大减少对储存空间的依赖，优化计算量
void generate_cube_graph(VertexNode cube_vertices[NUM_VERTICES], int16_t edge_length) {
    // 顶点坐标
    int16_t half_edge = edge_length / 2; // 计算半边长，使立方体中心位于原点
    int16_t vertices[NUM_VERTICES][3] = {
        {-half_edge, -half_edge, -half_edge}, // 0
        { half_edge, -half_edge, -half_edge}, // 1
        {-half_edge,  half_edge, -half_edge}, // 2
        { half_edge,  half_edge, -half_edge}, // 3
        { half_edge, -half_edge,  half_edge}, // 4
        {-half_edge, -half_edge,  half_edge}, // 5
        {-half_edge,  half_edge,  half_edge}, // 6
        { half_edge,  half_edge,  half_edge}  // 7
    };

    // 连接关系
    uint8_t edges[NUM_VERTICES][MAX_EDGES] = {
        {1, 2, 5}, // 0
        {0, 3, 4}, // 1
        {0, 3, 6}, // 2
        {1, 2, 7}, // 3
        {1, 5, 7}, // 4
        {0, 4, 6}, // 5
        {2, 5, 7}, // 6
        {3, 4, 6}  // 7
    };

    // 初始化顶点信息
    for (int i = 0; i < NUM_VERTICES; i++) {
        cube_vertices[i].vertex.x = vertices[i][0];
        cube_vertices[i].vertex.y = vertices[i][1];
        cube_vertices[i].vertex.z = vertices[i][2];
        cube_vertices[i].edge_count = MAX_EDGES;

        // 初始化连接关系
        for (int j = 0; j < MAX_EDGES; j++) {
            cube_vertices[i].edges[j].vertex_index = edges[i][j];
        }
    }
}


//基于图结构的正方体绘制函数，其他模型也可根据实际情况以类似方式绘制
void Draw_Cube_By_Graph(float yaw, float pitch, float roll, int16_t start_x, int16_t start_y, int16_t scale, VertexNode cube_vertices[NUM_VERTICES]) {

    Quaternion world = euler_to_quaternion_1(yaw, pitch, roll);
    Vector3 rotated_vertex;
    
    // 清除绘图区域
    OLED_ClearArea(start_x, start_y, 127, 63);
    //绘制画布边框
    OLED_DrawLine(start_x, start_y, start_x, 63); // 上边

    // 绘制立方体的边
    for (int i = 0; i < NUM_VERTICES; i++) {
        for (int j = 0; j < cube_vertices[i].edge_count; j++) {
            int16_t target_index = cube_vertices[i].edges[j].vertex_index;
            if (target_index < NUM_VERTICES) {
                // 获取当前顶点和目标顶点的坐标
                rotated_vertex = quat_rotate_vector_arry(world, cube_vertices[i].vertex.x, cube_vertices[i].vertex.y, cube_vertices[i].vertex.z);
                int16_t x1 = (int16_t)(rotated_vertex.x * scale) + (start_x + 127) / 2;
                int16_t y1 = (int16_t)(rotated_vertex.y * scale) + (start_y + 63) / 2;

                rotated_vertex = quat_rotate_vector_arry(world, cube_vertices[target_index].vertex.x, cube_vertices[target_index].vertex.y, cube_vertices[target_index].vertex.z);
                int16_t x2 = (int16_t)(rotated_vertex.x * scale) + (start_x + 127) / 2;
                int16_t y2 = (int16_t)(rotated_vertex.y * scale) + (start_y + 63) / 2;

                // 绘制线段连接两个顶点
                OLED_DrawLine(x1, y1, x2, y2);
            }
        }
    }
}

void OLED_DrawProgressBar(int16_t X, int16_t Y, int value, int min, int max) {
    // 进度条尺寸
    const uint8_t bar_width = 40;
    const uint8_t bar_height = 5;
    const uint8_t border = 1; // 边框宽度

    // 1. 清除进度条区域
    OLED_ClearArea(X, Y, bar_width, bar_height);

    // 2. 画边框
    OLED_DrawRectangle(X, Y, bar_width, bar_height, OLED_UNFILLED);

    // 3. 计算进度百分比
    if (max <= min) max = min + 1; // 防止分母为0
    if (value < min) value = min;
    if (value > max) value = max;
    float percent = (float)(value - min) / (max - min);

    // 4. 画填充部分
    int fill_width = (int)((bar_width - 2*border) * percent + 0.5f); // 向上取整
    if (fill_width > 0) {
        OLED_DrawRectangle(X + border, Y + border, fill_width, bar_height - 2*border, OLED_FILLED);
    }

    // // 5. 显示百分比数值
    // char percent_str[8];
    // int percent_show = (int)(percent * 100.0f + 0.5f);
    // sprintf(percent_str, "%3d%%", percent_show);
    // // 居中显示
    // OLED_ShowString(X + (bar_width/2) - 12, Y + (bar_height/2) - 8, percent_str, OLED_8X16);
}

