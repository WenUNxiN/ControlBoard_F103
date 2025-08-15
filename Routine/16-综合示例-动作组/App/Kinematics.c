#include "Kinematics.h"
#include "Uart.h"
#include <stdio.h>

const DH_Params dh_params[6] = {
	// a,    alpha, d,    theta
	{0.0, 0.0, 119.0, 0.0},	   // 关节1
	{35.0, -90.0, 0.0, 0.0},   // 关节2
	{130.0, 0.0, 0.0, 90.0},   // 关节3
	{45.0, -90.0, 130.0, 0.0}, // 关节4
	{0.0, 90.0, 0.0, 0.0},	   // 关节5
	{0.0, -90.0, 170.0, 0.0}   // 关节6 (工具长度)
};

// 机械臂关节限制 (根据实际机械臂调整)
const float joint_limits[6][2] = {
	{-180, 180}, // 关节1
	{-180, 180}, // 关节2
	{-180, 180}, // 关节3
	{-180, 180}, // 关节4
	{-180, 180}, // 关节5
	{-180, 180}	 // 关节6
};

int inverseKinematics(const Pose *pose, JointAngles *solutions)
{
	// 转换为弧度
	float roll = DEG2RAD(pose->roll);
	float pitch = DEG2RAD(pose->pitch);
	float yaw = DEG2RAD(pose->yaw);

	// 计算末端执行器的旋转矩阵（XYZ固定轴欧拉角）
	float cr = cosf(roll), sr = sinf(roll);
	float cp = cosf(pitch), sp = sinf(pitch);
	float cy = cosf(yaw), sy = sinf(yaw);

	// XYZ欧拉角旋转矩阵（顺序：Rz(yaw) * Ry(pitch) * Rx(roll)）
	float R[3][3] = {
		{cy * cp, cy * sp * sr - sy * cr, cy * sp * cr + sy * sr},
		{sy * cp, sy * sp * sr + cy * cr, sy * sp * cr - cy * sr},
		{-sp, cp * sr, cp * cr}};

	printf("Tool direction: [%f, %f, %f]\n", R[2][2], R[1][2], R[0][2]);
	// 计算腕部中心位置（减去工具长度）
	float tool_length = dh_params[5].d;
	float wx = pose->x - tool_length * R[2][2];
	float wy = pose->y - tool_length * R[1][2];
	float wz = pose->z - tool_length * R[0][2];
	printf("wx:%f, wy:%f, wz:%f\n", wx, wy, wz);
	// 关节1计算（两种可能解）
	float theta1 = atan2f(wy, wx);
	float theta1_alt = theta1 + (theta1 >= 0 ? -PI : PI);

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

	float a2 = dh_params[2].a;			  // 大臂长度
	float a3 = dh_params[3].d;			  // 小臂长度
	float d1 = dh_params[0].d;			  // 基座高度
	float d4 = dh_params[3].a;			  // 小臂的连杆偏移
	float aa2 = sqrtf(a3 * a3 + d4 * d4); // 关节3到腕部中心的直线距离

	// 计算平面距离和高度 (考虑J2的X轴偏移,J4的Z轴偏移)
	float r_proj = sqrtf(wx * wx + wy * wy) - dh_params[1].a; // 关节2到腕部中心的水平投影
	float h_proj = wz - d1;									  // 关节2到腕部中心的垂直投影
	printf("r_proj:%f, h_proj:%f]\n", r_proj, h_proj);
	// 检查可达性
	float d1_3 = sqrtf(r_proj * r_proj + h_proj * h_proj);
	float cos_theta33 = (r_proj * r_proj + h_proj * h_proj - a2 * a2 - aa2 * aa2) / (2 * a2 * aa2);
	printf("d1_3:%f,cos_theta33:%f, \n", d1_3, cos_theta33);
	if (fabsf(cos_theta33) > 1.0f)
	{
		return 1; // 位置不可达
	}
	/***
		直接使用反余弦函数，有两个问题：

		1.数值稳定性：当 D 接近 ±1 时，arccos 的计算可能因浮点误差导致结果不准确。

		2.象限模糊：arccos 只能返回 [0,π] 内的值，无法区分机械臂的两种构型（肘部向上/向下）

		通过三角恒等式，可得

		θ3=atan2(sin(θ3),cos(θ3))=atan2(±1−D2,D)
		***/
	// aa2与前一关节的夹角
	float theta33 = -atan2f(sqrt(1 - cos_theta33 * cos_theta33), cos_theta33);
	float theta33_alt = -theta33;
	printf("theta33:%f, \n", RAD2DEG(theta33));

	float theta3_ab = atan2f(a3, d4); // 小臂的对角

	float theta3 = theta33 + theta3_ab;

	// 关节2计算
	float gamma = atan2f(r_proj, h_proj);
	float beta = atan2f(aa2 * sinf(theta33), a2 + aa2 * cosf(theta33));
	float theta2 = gamma + beta;
	// printf("gamma:%f,beta:%f, \n", RAD2DEG(gamma),RAD2DEG(beta));

	float c1 = cos(theta1);
	float s1 = sin(theta1);
	float c23 = cos(theta2 + theta3);
	float s23 = sin(theta2 + theta3);

	// 计算R03
	float R03[3][3] = {
		{c1 * c23, -c1 * s23, s1},
		{s1 * c23, -s1 * s23, -c1},
		{s23, c23, 0}};

	// 计算R36 = R03' * R06
	float R36[3][3];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			R36[i][j] = 0;
			for (int k = 0; k < 3; k++)
			{
				R36[i][j] += R03[k][i] * R[k][j];
			}
		}
	}

	// 从R36中提取欧拉角 (ZYZ)
	// float theta5 = atan2(sqrt(R36[0][2]*R36[0][2] + R36[1][2]*R36[1][2]), R36[2][2]);

	/***
float theta4,theta6;
if (fabs(theta5) > 0.001) {
	theta4 = atan2(R36[1][2], R36[0][2]);
	theta6 = atan2(R36[2][1], -R36[2][0]);
} else {
	// 奇异位置
	theta4 = 0;
	theta6 = atan2(-R36[0][1], R36[1][1]);
}
	***/

	float theta4 = 0;
	float theta3_4 = atan2f(d4, a3); // 小臂偏移连杆的对角
	printf("theta3_4:%f, \n", RAD2DEG(theta3_4));
	float theta5 = PI - (theta2 - theta33 + theta3_4);
	float theta6 = 0;
	// 存储解 (这里只存储一个解，实际可以有多个解)
	solutions->theta[0] = RAD2DEG(theta1);
	solutions->theta[1] = RAD2DEG(theta2);
	solutions->theta[2] = RAD2DEG(theta3); // 小臂初始水平状态
	solutions->theta[3] = RAD2DEG(theta4);
	solutions->theta[4] = RAD2DEG(theta5) - 90;
	solutions->theta[5] = RAD2DEG(theta6);
	// 检查关节限位
	for (int i = 0; i < 6; i++)
	{
		if (solutions->theta[i] < joint_limits[i][0] || solutions->theta[i] > joint_limits[i][1])
		{
			printf("theta:%d,theta[%f]", i, solutions->theta[i]);
			return 2; // 超出关节限位
		}
	}
	return 0;
}
