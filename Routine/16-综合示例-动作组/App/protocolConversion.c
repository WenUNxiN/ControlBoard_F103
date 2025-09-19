/* 明文指令转暗文 */
#include "protocolConversion.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

extern void uart2_send_bytes(const uint8_t *buf, int len);
extern void SetPrintfUart(int uart);
extern int  printf(const char *fmt, ...);

#define MAX_MULTI_SERVO 64
#define DEFAULT_MIN_POS 500
#define DEFAULT_MAX_POS 2500

/* ---------- 小工具 ---------- */
static uint8_t _chk(const uint8_t *buf, int len)
{
    uint8_t s = 0;
    for (int i = 0; i < len; i++) s += buf[i];
    return ~s;
}
static void _u16be(uint8_t *p, uint16_t v) { p[0] = v >> 8; p[1] = v; }

/* ---------- ID 映射：所有 ID +1 ---------- */
static inline uint8_t tx_id(uint8_t id)
{
    return (uint8_t)(id + 1);   // 0→1, 255→0(溢出)
}

/* =================================================================
 *  1. 打包函数
 * ================================================================= */
typedef int (*pack_func_t)(uint8_t id, const char *arg, uint8_t *para);

static int pack_none(uint8_t id, const char *arg, uint8_t *para)
{
    (void)id; (void)arg; (void)para;
    return 0;
}

static int pack_position(uint8_t id, const char *arg, uint8_t *para)
{
    int pos, tim;
    if (sscanf(arg, "P%4dT%4d", &pos, &tim) != 2) return -1;
    if (pos < DEFAULT_MIN_POS || pos > DEFAULT_MAX_POS) return -2;
    _u16be(para + 0, (uint16_t)pos);
    _u16be(para + 2, (uint16_t)tim);
    return 4;
}

static int pack_pid_write(uint8_t id, const char *arg, uint8_t *para)
{
    if (strcmp(arg, "PID") == 0) return 0;          // 读 ID
    int newId;
    if (sscanf(arg, "PID%3d", &newId) != 1) return -1;
    if (newId < 0 || newId > 255) return -2;
    para[0] = (uint8_t)(newId + 1);
    return 1;
}

/* =================================================================
 *  2. 命令表
 * ================================================================= */
typedef struct {
    const char *key;
    uint8_t     cmd;
    pack_func_t pack;
} cmd_entry_t;

static const cmd_entry_t cmd_table[] = {
    {"PID",  0x0B, pack_pid_write},  // 写 ID
    {"PID",  0x20, pack_none},       // 读 ID
    {"P",    0x02, pack_position},   // 单位置
    {"PULK", 0x08, pack_none},       // 力矩释放
    {"PCSM", 0x08, pack_none},
    {"PULR", 0x09, pack_none},
    {"PCSR", 0x09, pack_none},
    {"PSCK", 0x0C, pack_none},       // 偏差
    {"PMOD", 0x29, pack_none},       // 读模式
    {"PMOD", 0x14, pack_none},       // 写模式
    {"PBD",  0x21, pack_none},       // 波特率
    {"PSMI", 0x0D, pack_none},       // 极限
    {"PSMX", 0x0D, pack_none},
    {"PRTT", 0x1D, pack_none},       // 温度
    {"PRTV", 0x1C, pack_none},       // 电压
    {"PVER", 0x2D, pack_none},       // 版本
    {"PDPT", 0x10, pack_none},       // 暂停
    {"PDCT", 0x11, pack_none},       // 继续
    {"PDST", 0x12, pack_none},       // 停止
    {"PCLE", 0x18, pack_none},       // 复位
    {"PCSD", 0x16, pack_none},       // 初始位置
};
#define CMD_N (sizeof(cmd_table)/sizeof(cmd_table[0]))

/* ---------- 命令查找 ---------- */
static int lookup_cmd(const char *key)
{
    char buf[16] = {0};
    for (int i = 0; i < 15 && key[i]; i++) buf[i] = (char)toupper((unsigned char)key[i]);

    int best = -1, bestLen = 0;
    for (int i = 0; i < CMD_N; i++) {
        int klen = (int)strlen(cmd_table[i].key);
        if (klen > bestLen && strncmp(buf, cmd_table[i].key, klen) == 0) {
            best = i;
            bestLen = klen;
        }
    }
    return best;
}

/* ---------- 构造帧 ---------- */
static int _build_frame(uint8_t *bin,
                        uint8_t id,
                        uint8_t cmd,
                        const uint8_t *para,
                        uint8_t paraLen)
{
    uint8_t *p = bin;
    *p++ = 0xAA; *p++ = 0xAA;
    *p++ = tx_id(id);              // ← 全部 ID +1
    *p++ = 1 + 1 + paraLen + 1;
    *p++ = cmd;
    for (uint8_t i = 0; i < paraLen; i++) *p++ = para[i];
    *p = _chk(bin + 2, (p - bin - 2));
    return (p - bin) + 1;
}

/* ---------- 多舵机 ---------- */
static int _build_multi_sync(const char *buf, uint8_t *bin)
{
    uint8_t ids[MAX_MULTI_SERVO], posH[MAX_MULTI_SERVO], posL[MAX_MULTI_SERVO];
    int cnt = 0;
    const char *s = buf;
    while (*s && cnt < MAX_MULTI_SERVO) {
        const char *hash = strchr(s, '#');
        if (!hash) break;
        int id, pos, tim;
        if (sscanf(hash, "#%3dP%4dT%4d", &id, &pos, &tim) == 3) {
            if (pos < DEFAULT_MIN_POS || pos > DEFAULT_MAX_POS) return BUS_SERVO_ERR_RANGE;
            ids[cnt]  = (uint8_t)id;
            posH[cnt] = pos >> 8;
            posL[cnt] = pos & 0xFF;
            cnt++;
            const char *bang = strchr(hash, '!');
            s = bang ? bang + 1 : hash + 1;
        } else break;
    }
    if (cnt == 0) return BUS_SERVO_ERR_PARSE;

    uint8_t para[1 + 3 * MAX_MULTI_SERVO + 2], *p = para;
    *p++ = cnt;
    for (int i = 0; i < cnt; i++) {
        *p++ = tx_id(ids[i]);   // ← 每个 ID 也 +1
        *p++ = posH[i];
        *p++ = posL[i];
    }
    uint16_t tim = 1000;
    _u16be(p, tim); p += 2;
    return _build_frame(bin, 0, 0x04, para, p - para);
}

/* =================================================================
 *  主入口
 * ================================================================= */
int busServo_asciiToBin(const char *asc, uint8_t *bin)
{
    if (!asc || !bin) return BUS_SERVO_ERR_PARSE;
    int ascLen = (int)strlen(asc);
    if (ascLen < 2) return BUS_SERVO_ERR_FRAME;

    char head = asc[0];
    if (head != '#' && head != '{' && head != '(' && head != '$')
        return BUS_SERVO_ERR_FRAME;

    /* 多舵机 */
    if (head == '{' || head == '(') {
        char endc = (head == '{') ? '}' : ')';
        if (asc[ascLen - 1] != endc) return BUS_SERVO_ERR_FRAME;
        char buf[BUS_SERVO_MAX_ASC];
        int n = ascLen - 2;
        if (n <= 0 || n >= BUS_SERVO_MAX_ASC) return BUS_SERVO_ERR_LONG;
        memcpy(buf, asc + 1, n); buf[n] = 0;
        return _build_multi_sync(buf, bin);
    }

    /* 单舵机 */
    if (head == '#') {
        if (asc[ascLen - 1] != '!') return BUS_SERVO_ERR_FRAME;
        int id;
        char tmp[64];
        if (sscanf(asc + 1, "%3d%63[^!]", &id, tmp) != 2)
            return BUS_SERVO_ERR_PARSE;
        int idx = lookup_cmd(tmp);
        if (idx < 0) return BUS_SERVO_ERR_UNSUPPORT;

        uint8_t para[16];
        int paraLen = cmd_table[idx].pack((uint8_t)id, tmp, para);
        if (paraLen < 0) return BUS_SERVO_ERR_RANGE;

        return _build_frame(bin, (uint8_t)id,
                            cmd_table[idx].cmd, para, (uint8_t)paraLen);
    }

    return BUS_SERVO_ERR_UNSUPPORT;
}

/* ---------------- 用户封装 ---------------- */
void user_parse_ascii_command(const char *cmd)
{
    uint8_t bin[BUS_SERVO_MAX_BIN];
    int len = busServo_asciiToBin(cmd, bin);
    if (len > 0) {
        // 回显发送的指令
//        SetPrintfUart(1);
//        for (int i = 0; i < len; ++i) printf("%02X ", bin[i]);
//        printf("\r\n");
        
        // 发送指令
        uart2_send_bytes(bin, len);
    } else {
        // 错误处理
        SetPrintfUart(1);
        printf("ascii->bin error:%d\r\n", len);
    }
}
