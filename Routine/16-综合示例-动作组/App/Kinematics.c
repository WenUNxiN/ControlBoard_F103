/******************************************************************************************
*  0. 头文件与全局常量
******************************************************************************************/
#include "Kinematics.h"   // 运动学头文件，里面应该定义了 Pose / JointAngles / DH_Params
#include "Uart.h"         // 用于打印调试信息
#include <stdio.h>        // printf

/* 6 个连杆的 改进型DH参数表
*  顺序：a, α, d, θ
*  单位：mm / deg（θ 这一列只是初始偏移，真正关节角由 IK 计算）*/
const DH_Params dh_params[6] = {
    {0.0, 0.0,   119.0, 0.0},   // J1：底座旋转，基座高 119 mm
    {35.0, -90.0, 0.0,   0.0},   // J2：大臂俯仰，连杆偏距 35 mm，α = -90°
    {130.0, 0.0,  0.0,   90.0},  // J3：小臂俯仰，大臂长 130 mm
    {45.0, -90.0, 130.0, 0.0},   // J4：小臂旋转，d4 = 130 mm
    {0.0,  90.0,  0.0,   0.0},   // J5：手腕俯仰
    {0.0, -90.0,  170.0, 0.0}    // J6：工具法兰，工具长 170 mm
};

/* 关节限位，单位：度
*  这里全部 ±180° 只是占位，务必按实际机械臂手册修改！ */
const float joint_limits[6][2] = {
    {-180, 180},
    {-180, 180},
    {-180, 180},
    {-180, 180},
    {-180, 180},
    {-180, 180}
};

/******************************************************************************************
*  1. 逆运动学主函数
*     输入：目标末端位姿 Pose* pose
*     输出：一组关节角 JointAngles* solutions
*     返回值：0 成功；1 位置不可达；2 超出关节限位
******************************************************************************************/
int inverseKinematics(const Pose *pose, JointAngles *solutions)
{
    /******************** 1.1 把角度转弧度 ********************/
    float roll  = DEG2RAD(pose->roll);   // 绕固定轴 X
    float pitch = DEG2RAD(pose->pitch);  // 绕固定轴 Y
    float yaw   = DEG2RAD(pose->yaw);    // 绕固定轴 Z

    /******************** 1.2 欧拉角 → 旋转矩阵 R06 ********************/
    float cr = cosf(roll),  sr = sinf(roll);
    float cp = cosf(pitch), sp = sinf(pitch);
    float cy = cosf(yaw),   sy = sinf(yaw);

    /* 固定轴 XYZ 欧拉角：R = Rz(yaw) * Ry(pitch) * Rx(roll)
     * 结果 R[3][3] 就是末端坐标系在基坐标系中的姿态 */
    float R[3][3] = {
        { cy*cp,       cy*sp*sr - sy*cr,      cy*sp*cr + sy*sr },
        { sy*cp,       sy*sp*sr + cy*cr,      sy*sp*cr - cy*sr },
        {   -sp,           cp*sr,                    cp*cr     }
    };

    /* 调试用：打印末端工具 Z 轴在基坐标系下的方向向量 */
//    printf("Tool direction: [%f, %f, %f]\n", R[2][2], R[1][2], R[0][2]);

    /******************** 1.3 计算腕部中心 (wrist center) ********************/
    float tool_length = dh_params[5].d;   // 法兰到工具尖的偏移
    /* 把工具尖端沿末端 Z 轴反方向退回 tool_length，得到第 5 关节中心坐标 */
    float wx = pose->x - tool_length * R[2][2];
    float wy = pose->y - tool_length * R[1][2];
    float wz = pose->z - tool_length * R[0][2];
//    printf("wx:%f, wy:%f, wz:%f\n", wx, wy, wz);

    /******************** 2. 求 θ1（底座旋转） ********************/
    float theta1      = atan2f(wy, wx);                    // 主解
    float theta1_alt  = theta1 + (theta1 >= 0 ? -PI : PI); // 镜像解（左右手）
    
    // 关节3计算
	/***
	关节3的角度计算通常基于一个三角形，这个三角形由:

	关节2到关节3的连杆长度(a₂)

	关节3到腕部中心的连杆长度(d₄)

	腕部中心到关节1的投影距离

	cos(C) = a*a + b*b - c*c /2ab

	在机械臂的几何关系中：

	c = √(wx² + wy² + (wz - d₁)²)（腕部中心到关节1的距离）

	a1 = a₂（关节2到关节3的连杆长度）

	b1 = d₄（关节3到腕部中心的连杆长度）

	C = π - θ₃

	将余弦定理重排：
	cos(π - θ₃) = (a₂² + d₄² - c²) / (2 * a₂ * d₄)
	利用cos(π - θ₃) = -cos(θ₃)，可以得到：
	cos(θ₃) = (c² - a₂² - d₄²) / (2 * a₂ * d₄)
	***/

    /******************** 3. 求 θ3（小臂关节） ********************/
    /* 先取出几何参数，名字与 DH 对应：
     * a2  : 连杆 2 长度（大臂）
     * a3  : 连杆 3 偏距（注意 DH 里其实放在 d 列）
     * d1  : 基座高
     * d4  : 连杆 4 偏距（DH 中的 a3）
     * aa2 : 第 3 关节到腕部中心的直线距离 = √(a3² + d4²) */
    float a2  = dh_params[2].a;                  // 130 mm
    float a3  = dh_params[3].d;                  // 130 mm
    float d1  = dh_params[0].d;                  // 119 mm
    float d4  = dh_params[3].a;                  // 45 mm
    float aa2 = sqrtf(a3*a3 + d4*d4);            // ≈ 137.5 mm

    /* 把三维问题压扁到二维：
     * r_proj : 腕部中心到 J2 的水平距离（需扣除 J2 本身偏移 a1 = 35 mm）
     * h_proj : 腕部中心到 J2 的垂直距离（需扣除基座高 d1） */
    float r_proj = sqrtf(wx*wx + wy*wy) - dh_params[1].a;   // 减去 a1 = 35 mm
    float h_proj = wz - d1;

    /* 根据余弦定理计算虚拟夹角 theta33（见函数上方大注释） */
    float d1_3       = sqrtf(r_proj*r_proj + h_proj*h_proj);   // 腕中心到 J2 的直线
    float cos_theta33 = (r_proj*r_proj + h_proj*h_proj
                         - a2*a2 - aa2*aa2) / (2.0f * a2 * aa2);

    /* 可达性检查：若 |cos| > 1，目标超出工作空间 */
    if (fabsf(cos_theta33) > 1.0f) return 1;
    
    /***
		直接使用反余弦函数，有两个问题：

		1.数值稳定性：当 D 接近 ±1 时，arccos 的计算可能因浮点误差导致结果不准确。

		2.象限模糊：arccos 只能返回 [0,π] 内的值，无法区分机械臂的两种构型（肘部向上/向下）

		通过三角恒等式，可得

		θ3=atan2(sin(θ3),cos(θ3))=atan2(±1−D2,D)
	***/
    
    
	// aa2与前一关节的夹角

    /* 用 atan2 求 theta33，再考虑肘部上下两解（正负） */
    float theta33      = -atan2f(sqrtf(1.0f - cos_theta33*cos_theta33), cos_theta33);
    float theta33_alt  = -theta33;  // 肘部朝上的镜像解

    /* θ3 的 DH 角 = 虚拟夹角 + 固定偏角（由连杆几何决定） */
    float theta3_ab = atan2f(a3, d4);  // 固定偏角 ≈ 70.9°
    float theta3    = theta33 + theta3_ab;

    /******************** 4. 求 θ2（大臂俯仰） ********************/
    /* gamma : 腕部中心与 J2 连线与水平面夹角
     * beta  : 由余弦定理反推出的 a2 与 aa2 夹角
     * theta2 = gamma + beta */
    float gamma = atan2f(r_proj, h_proj);
    float beta  = atan2f(aa2 * sinf(theta33), a2 + aa2 * cosf(theta33));
    float theta2 = gamma + beta;

    /******************** 5. 计算 R03 ********************/
    /* R03 是基座到第 3 连杆的旋转矩阵，只与 θ1,θ2,θ3 有关 */
    float c1  = cosf(theta1);
    float s1  = sinf(theta1);
    float c23 = cosf(theta2 + theta3);
    float s23 = sinf(theta2 + theta3);

    float R03[3][3] = {
        {  c1*c23,  -c1*s23,   s1 },
        {  s1*c23,  -s1*s23,  -c1 },
        {   s23,      c23,     0  }
    };

    /******************** 6. 计算手腕姿态矩阵 R36 ********************/
    /* R36 = R03ᵀ * R06  (3×3 矩阵乘法) */
    float R36[3][3];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            R36[i][j] = 0.0f;
            for (int k = 0; k < 3; ++k)
                R36[i][j] += R03[k][i] * R[k][j];  // 注意 R03 需要转置，这里用 k,i
        }

    /******************** 7. 求 θ4, θ5, θ6 ********************/
    /* 标准方法：从 R36 里按 ZYZ 欧拉角提取 */
    float theta4 = 0.0f;
    float theta3_4 = atan2f(d4, a3);  // 连杆几何固定角
//    printf("theta3_4:%f deg\n", RAD2DEG(theta3_4));

    /* 简化版 θ5：让末端 Z 轴对齐目标，但 X/Y 方向不定 */
    float theta5 = PI - (theta2 - theta33 + theta3_4);
    float theta6 = 0.0f;

    /******************** 8. 把弧度结果转成角度，写入输出结构体 ********************/
    solutions->theta[0] = RAD2DEG(theta1);
    solutions->theta[1] = RAD2DEG(theta2);
    solutions->theta[2] = RAD2DEG(theta3);
    solutions->theta[3] = RAD2DEG(theta4);
    solutions->theta[4] = RAD2DEG(theta5) - 90.0f;  // 手动补偿 90°，使零位竖直
    solutions->theta[5] = RAD2DEG(theta6);

    /******************** 9. 关节限位检查 ********************/
    for (int i = 0; i < 6; ++i)
    {
        if (solutions->theta[i] < joint_limits[i][0] ||
            solutions->theta[i] > joint_limits[i][1])
        {
//            printf("Joint %d out of limit: %f°\n", i, solutions->theta[i]);
            return 2;  // 超限
        }
    }

    /******************** 10. 成功 ********************/
    return 0;
}
