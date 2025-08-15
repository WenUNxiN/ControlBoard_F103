#include "Key.h"

/****************************************************************
 * 函数名称：Key_Init
 * 功    能：初始化按键对应的GPIO引脚
 * 参    数：无
 * 返 回 值：无
 * 备注信息：
 *   1. 使能按键所在端口的时钟
 *   2. 配置按键引脚为“下拉输入”模式（GPIO_Mode_IPD）
 *      默认被下拉到低电平，按键未按下时为0
 ****************************************************************/
void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                /* 定义GPIO初始化结构体变量 */

    /* 使能按键端口的时钟 */
    RCC_APB2PeriphClockCmd(KEY_GPIO_CLK, ENABLE);

    /* 配置按键引脚参数 */
    GPIO_InitStructure.GPIO_Pin  = KEY_GPIO_PIN;        /* 选择按键引脚 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;       /* 下拉输入模式：无按键时电平为低 */

    /* 调用库函数完成GPIO初始化 */
    GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);
}

/****************************************************************
 * 函数名称：Key_Scan
 * 功    能：非阻塞式按键扫描（带简单消抖）
 * 参    数：无
 * 返 回 值：u8
 *            KEY_PRESS   —— 检测到一次“按下→释放”的完整动作
 *            KEY_RELEASE —— 当前处于未按下或持续按下状态
 * 备注信息：
 *   1. 使用两个静态变量记录上一次和本次的电平状态
 *   2. 仅当上一次为按下(KEY_PRESS)且本次为释放(KEY_RELEASE)时，
 *      认为完成一次有效按键动作，返回 KEY_PRESS
 *   3. 其余情况均返回 KEY_RELEASE，实现“松手检测”
 ****************************************************************/
u8 Key_Scan(void)
{
    static u8 Key_Flag     = KEY_RELEASE;   /* 当前电平状态 */
    static u8 Key_LastFlag = KEY_RELEASE;   /* 上一次电平状态 */

    /* 保存上一次状态，并读取当前引脚电平 */
    Key_LastFlag = Key_Flag;
    Key_Flag = GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN);

    /* 仅在“上一次按下→本次释放”的瞬间返回 KEY_PRESS */
    if (Key_LastFlag == KEY_PRESS && Key_Flag == KEY_RELEASE)
    {
        return KEY_PRESS;
    }

    /* 其他情况均视为未发生有效按键动作 */
    return KEY_RELEASE;
}
	
