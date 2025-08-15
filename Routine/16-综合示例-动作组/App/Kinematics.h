#ifndef _KINEMATICS__H_
#define _KINEMATICS__H_

#include <string.h> 
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "Application.h"

// 定义PI和角度弧度转换
#define PI 3.14159265358979323846f
#define DEG2RAD(x) ((x) * PI / 180.0)
#define RAD2DEG(x) ((x) * 180.0 / PI)

// 位置和姿态结构体
typedef struct {
    float x, y, z;      // 位置 (mm)
    float roll, pitch, yaw;  // 欧拉角 (度)
} Pose;

// 改进DH参数表 (a, alpha, d, theta)
// 单位：mm和度
typedef struct {
    float a;        // 连杆长度
    float alpha;    // 连杆扭角
    float d;        // 连杆偏移
    float theta;    // 关节角度
} DH_Params;

// 关节角度结构体
typedef struct {
    float theta[6]; // 6个关节角度 (度)
} JointAngles;

int inverseKinematics(const Pose* pose, JointAngles* solutions);


#endif
