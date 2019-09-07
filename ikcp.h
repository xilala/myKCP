//=====================================================================
//
// KCP - A Better ARQ Protocol Implementation
// skywind3000 (at) gmail.com, 2010-2011
//  
// Features:
// + Average RTT reduce 30% - 40% vs traditional ARQ like tcp.
// + Maximum RTT reduce three times vs tcp.
// + Lightweight, distributed as a single source file.
//
//=====================================================================
#ifndef __IKCP_H__
#define __IKCP_H__

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>


//=====================================================================
// 32BIT INTEGER DEFINITION 
//=====================================================================
#ifndef __INTEGER_32_BITS__
#define __INTEGER_32_BITS__
#if defined(_WIN64) || defined(WIN64) || defined(__amd64__) || \
	defined(__x86_64) || defined(__x86_64__) || defined(_M_IA64) || \
	defined(_M_AMD64)
	typedef unsigned int ISTDUINT32;
	typedef int ISTDINT32;
#elif defined(_WIN32) || defined(WIN32) || defined(__i386__) || \
	defined(__i386) || defined(_M_X86)
	typedef unsigned long ISTDUINT32;
	typedef long ISTDINT32;
#elif defined(__MACOS__)
	typedef UInt32 ISTDUINT32;
	typedef SInt32 ISTDINT32;
#elif defined(__APPLE__) && defined(__MACH__)
	#include <sys/types.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif defined(__BEOS__)
	#include <sys/inttypes.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif (defined(_MSC_VER) || defined(__BORLANDC__)) && (!defined(__MSDOS__))
	typedef unsigned __int32 ISTDUINT32;
	typedef __int32 ISTDINT32;
#elif defined(__GNUC__)
	#include <stdint.h>
	typedef uint32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#else 
	typedef unsigned long ISTDUINT32; 
	typedef long ISTDINT32;
#endif
#endif


//=====================================================================
// Integer Definition
//=====================================================================
#ifndef __IINT8_DEFINED
#define __IINT8_DEFINED
typedef char IINT8;
#endif

#ifndef __IUINT8_DEFINED
#define __IUINT8_DEFINED
typedef unsigned char IUINT8;
#endif

#ifndef __IUINT16_DEFINED
#define __IUINT16_DEFINED
typedef unsigned short IUINT16;
#endif

#ifndef __IINT16_DEFINED
#define __IINT16_DEFINED
typedef short IINT16;
#endif

#ifndef __IINT32_DEFINED
#define __IINT32_DEFINED
typedef ISTDINT32 IINT32;
#endif

#ifndef __IUINT32_DEFINED
#define __IUINT32_DEFINED
typedef ISTDUINT32 IUINT32;
#endif

#ifndef __IINT64_DEFINED
#define __IINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 IINT64;
#else
typedef long long IINT64;
#endif
#endif

#ifndef __IUINT64_DEFINED
#define __IUINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 IUINT64;
#else
typedef unsigned long long IUINT64;
#endif
#endif

#ifndef INLINE
#if defined(__GNUC__)

#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define INLINE         __inline__ __attribute__((always_inline))
#else
#define INLINE         __inline__
#endif

#elif (defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__))
#define INLINE __inline
#else
#define INLINE 
#endif
#endif

#if (!defined(__cplusplus)) && (!defined(inline))
#define inline INLINE
#endif


//=====================================================================
// QUEUE DEFINITION                                                  
//=====================================================================
#ifndef __IQUEUE_DEF__
#define __IQUEUE_DEF__

struct IQUEUEHEAD {
	struct IQUEUEHEAD *next, *prev;
};

typedef struct IQUEUEHEAD iqueue_head;


//---------------------------------------------------------------------
// queue init                                                         
//---------------------------------------------------------------------
#define IQUEUE_HEAD_INIT(name) { &(name), &(name) }
#define IQUEUE_HEAD(name) \
	struct IQUEUEHEAD name = IQUEUE_HEAD_INIT(name)

#define IQUEUE_INIT(ptr) ( \
	(ptr)->next = (ptr), (ptr)->prev = (ptr))

#define IOFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define ICONTAINEROF(ptr, type, member) ( \
		(type*)( ((char*)((type*)ptr)) - IOFFSETOF(type, member)) )

#define IQUEUE_ENTRY(ptr, type, member) ICONTAINEROF(ptr, type, member)


//---------------------------------------------------------------------
// queue operation                     
//---------------------------------------------------------------------
#define IQUEUE_ADD(node, head) ( \
	(node)->prev = (head), (node)->next = (head)->next, \
	(head)->next->prev = (node), (head)->next = (node))

#define IQUEUE_ADD_TAIL(node, head) ( \
	(node)->prev = (head)->prev, (node)->next = (head), \
	(head)->prev->next = (node), (head)->prev = (node))

#define IQUEUE_DEL_BETWEEN(p, n) ((n)->prev = (p), (p)->next = (n))

#define IQUEUE_DEL(entry) (\
	(entry)->next->prev = (entry)->prev, \
	(entry)->prev->next = (entry)->next, \
	(entry)->next = 0, (entry)->prev = 0)

#define IQUEUE_DEL_INIT(entry) do { \
	IQUEUE_DEL(entry); IQUEUE_INIT(entry); } while (0)

#define IQUEUE_IS_EMPTY(entry) ((entry) == (entry)->next)

#define iqueue_init		IQUEUE_INIT
#define iqueue_entry	IQUEUE_ENTRY
#define iqueue_add		IQUEUE_ADD
#define iqueue_add_tail	IQUEUE_ADD_TAIL
#define iqueue_del		IQUEUE_DEL
#define iqueue_del_init	IQUEUE_DEL_INIT
#define iqueue_is_empty IQUEUE_IS_EMPTY

#define IQUEUE_FOREACH(iterator, head, TYPE, MEMBER) \
	for ((iterator) = iqueue_entry((head)->next, TYPE, MEMBER); \
		&((iterator)->MEMBER) != (head); \
		(iterator) = iqueue_entry((iterator)->MEMBER.next, TYPE, MEMBER))

#define iqueue_foreach(iterator, head, TYPE, MEMBER) \
	IQUEUE_FOREACH(iterator, head, TYPE, MEMBER)

#define iqueue_foreach_entry(pos, head) \
	for( (pos) = (head)->next; (pos) != (head) ; (pos) = (pos)->next )
	

#define __iqueue_splice(list, head) do {	\
		iqueue_head *first = (list)->next, *last = (list)->prev; \
		iqueue_head *at = (head)->next; \
		(first)->prev = (head), (head)->next = (first);		\
		(last)->next = (at), (at)->prev = (last); }	while (0)

#define iqueue_splice(list, head) do { \
	if (!iqueue_is_empty(list)) __iqueue_splice(list, head); } while (0)

#define iqueue_splice_init(list, head) do {	\
	iqueue_splice(list, head);	iqueue_init(list); } while (0)


#ifdef _MSC_VER
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4996)
#endif

#endif


//---------------------------------------------------------------------
// WORD ORDER
//---------------------------------------------------------------------
#ifndef IWORDS_BIG_ENDIAN
    #ifdef _BIG_ENDIAN_
        #if _BIG_ENDIAN_
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #if defined(__hppa__) || \
            defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
            (defined(__MIPS__) && defined(__MIPSEB__)) || \
            defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
            defined(__sparc__) || defined(__powerpc__) || \
            defined(__mc68000__) || defined(__s390x__) || defined(__s390__)
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #define IWORDS_BIG_ENDIAN  0
    #endif
#endif

//kcp网络包头结构：
/*
|<------------ 4 bytes ------------>|
+--------+--------+--------+--------+
|               conv                | conv：Conversation, 会话序号，用于标识收发数据包是否一致
+--------+--------+--------+--------+ cmd: command，用于标识指令，例如：push，ack等
|  cmd   |  frg   |       wnd       | frg: Fragment, 分段序号，序号从大到小
+--------+--------+--------+--------+ wnd: 接收窗口大小
|                ts                 | ts: 发送的时间戳
+--------+--------+--------+--------+
|                sn                 | sn: Segment序号
+--------+--------+--------+--------+
|                una                | una: Unacknowledged, 当前未收到的序号，即代表这个序号之前的包均收到
+--------+--------+--------+--------+      
|                len                | len: data数据的长度
+--------+--------+--------+--------+
kcp包头总共占用了24个字节。给sn分配了4个字节，可以不用考虑序号越界的问题。

*/

//=====================================================================
// SEGMENT
//=====================================================================
struct IKCPSEG
{
	struct IQUEUEHEAD node;
	IUINT32 conv;//会话序列号，接收到的数据包和发送的一致，才接收此数据包
	IUINT32 cmd;//指令类型，代表这个segment的类型，例如push、ack
	IUINT32 frg;//分段序号，序号从大到小
	IUINT32 wnd;//窗口大小
	IUINT32 ts;//发送的时间戳
	IUINT32 sn;//segment序号
	IUINT32 una;//当前未收到的序号，即代表该序号之前的包均收到
	IUINT32 len;//数据长度
	IUINT32 resendts;//重发时间戳
	IUINT32 rto;//超时重传的时间间隔
	IUINT32 fastack;//ack跳过的次数，用于快速重传
	IUINT32 xmit;//发送的次数，即重传的次数
	char data[1];
};

//kcp工作流程：
/*
1 kcp首先将要发送的数据存到kcp->buffer中，如果需要发送的数据总量的大小大于kcp->mtu，则将buffer中的数据调用output函数发送出去，output函数由用户传入。
2 kcp数据包发送顺序： 1. IKCP_CMD_ACK(ack) 2. IKCP_CMD_WASK(请求远程窗口大小) 3. IKCP_CMD_WINS(发送本地窗口大小) 4. IKCP_CMD_PUSH(push data)
3 在发送PUSH类型的数据时，首先需要将数据从sen_que移动到sen_buf中（在移动时会检测拥塞窗口的大小，sen_que可以理解为发送数的缓冲队列）。
4 kcp在发送sen_buf队列中的数据时会检测是否是第一次发送： - 如果该segment的发送次数为0，则直接发送。 - 如果发送次数大于0并且已经超时，则再次发送并调整rto和下次超时时间 - 如果没有超时但是达到了快速重传的条件（被跳过了几个包），则发送并且更新下次超时时间。
*/


//---------------------------------------------------------------------
// IKCPCB
//---------------------------------------------------------------------
//buffer的发送和接收均调用外部传入的函数
struct IKCPCB //kcp控制块，用于控制packet的发送、接收、重传等操作
{
	IUINT32 conv, mtu, mss, state;//会话id，最大传输单元，最大分片大小,连接状态
	IUINT32 snd_una, snd_nxt, rcv_nxt;//第一个未确认的包，待发送包的序号，待接收消息的序号
	IUINT32 ts_recent, ts_lastack, ssthresh;//ssthresh，拥塞窗口的阈值
	IINT32 rx_rttval, rx_srtt, rx_rto, rx_minrto;//rx_rto：由ack接收延迟计算出来的复原时间
	//cwnd：拥塞窗口大小，probe  探查变量，IKCP_ASK_TELL表示告知远端窗口大小。IKCP_ASK_SEND表示请求远端告知窗口大小。
	IUINT32 snd_wnd, rcv_wnd, rmt_wnd, cwnd, probe;//rmt_wnd：远端接收窗口大小
	IUINT32 current, interval, ts_flush, xmit;//interval：内部flush刷新间隔，ts_flush：下次flush刷新时间戳
	IUINT32 nrcv_buf, nsnd_buf;
	IUINT32 nrcv_que, nsnd_que;
	IUINT32 nodelay, updated;//nodelay：是否启动无延迟模式，update：是否调用过update函数的标识(kcp需要上层通过不断的ikcp_update和ikcp_check来驱动kcp的收发过程)
	IUINT32 ts_probe, probe_wait;//ts_probe：下次探查窗口的时间戳，probe_wait：探查窗口需要等待的时间
	IUINT32 dead_link, incr;////dead_link：最大重传次数，incr：可发送的最大数据量
	struct IQUEUEHEAD snd_queue;//发送消息的队列
	struct IQUEUEHEAD rcv_queue;//接收消息的队列
	struct IQUEUEHEAD snd_buf;//发送消息的缓存
	struct IQUEUEHEAD rcv_buf;//接收消息的缓存
	IUINT32 *acklist;//待发送的ack的列表
	IUINT32 ackcount;//ack数量
	IUINT32 ackblock;//acklist的大小
	void *user;
	char *buffer;//储存消息字节流的内存
	int fastresend;//触发快速重传的重复ack个数
	int nocwnd, stream;
	int logmask;
	int (*output)(const char *buf, int len, struct IKCPCB *kcp, void *user);//发送消息的回调函数
	void (*writelog)(const char *log, struct IKCPCB *kcp, void *user);
};



typedef struct IKCPCB ikcpcb;

#define IKCP_LOG_OUTPUT			1
#define IKCP_LOG_INPUT			2
#define IKCP_LOG_SEND			4
#define IKCP_LOG_RECV			8
#define IKCP_LOG_IN_DATA		16
#define IKCP_LOG_IN_ACK			32
#define IKCP_LOG_IN_PROBE		64
#define IKCP_LOG_IN_WINS		128
#define IKCP_LOG_OUT_DATA		256
#define IKCP_LOG_OUT_ACK		512
#define IKCP_LOG_OUT_PROBE		1024
#define IKCP_LOG_OUT_WINS		2048

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------
// interface
//---------------------------------------------------------------------

// create a new kcp control object, 'conv' must equal in two endpoint
// from the same connection. 'user' will be passed to the output callback
// output callback can be setup like this: 'kcp->output = my_udp_output'
ikcpcb* ikcp_create(IUINT32 conv, void *user);

// release kcp control object
void ikcp_release(ikcpcb *kcp);

// set output callback, which will be invoked by kcp
void ikcp_setoutput(ikcpcb *kcp, int (*output)(const char *buf, int len, 
	ikcpcb *kcp, void *user));

// user/upper level recv: returns size, returns below zero for EAGAIN
int ikcp_recv(ikcpcb *kcp, char *buffer, int len);

// user/upper level send, returns below zero for error
int ikcp_send(ikcpcb *kcp, const char *buffer, int len);

// update state (call it repeatedly, every 10ms-100ms), or you can ask 
// ikcp_check when to call it again (without ikcp_input/_send calling).
// 'current' - current timestamp in millisec. 
void ikcp_update(ikcpcb *kcp, IUINT32 current);

// Determine when should you invoke ikcp_update:
// returns when you should invoke ikcp_update in millisec, if there 
// is no ikcp_input/_send calling. you can call ikcp_update in that
// time, instead of call update repeatly.
// Important to reduce unnacessary ikcp_update invoking. use it to 
// schedule ikcp_update (eg. implementing an epoll-like mechanism, 
// or optimize ikcp_update when handling massive kcp connections)
IUINT32 ikcp_check(const ikcpcb *kcp, IUINT32 current);

// when you received a low level packet (eg. UDP packet), call it
int ikcp_input(ikcpcb *kcp, const char *data, long size);

// flush pending data
void ikcp_flush(ikcpcb *kcp);

// check the size of next message in the recv queue
int ikcp_peeksize(const ikcpcb *kcp);

// change MTU size, default is 1400
int ikcp_setmtu(ikcpcb *kcp, int mtu);

// set maximum window size: sndwnd=32, rcvwnd=32 by default
int ikcp_wndsize(ikcpcb *kcp, int sndwnd, int rcvwnd);

// get how many packet is waiting to be sent
int ikcp_waitsnd(const ikcpcb *kcp);

// fastest: ikcp_nodelay(kcp, 1, 20, 2, 1)
// nodelay: 0:disable(default), 1:enable
// interval: internal update timer interval in millisec, default is 100ms 
// resend: 0:disable fast resend(default), 1:enable fast resend
// nc: 0:normal congestion control(default), 1:disable congestion control
int ikcp_nodelay(ikcpcb *kcp, int nodelay, int interval, int resend, int nc);


void ikcp_log(ikcpcb *kcp, int mask, const char *fmt, ...);

// setup allocator
void ikcp_allocator(void* (*new_malloc)(size_t), void (*new_free)(void*));

// read conv
IUINT32 ikcp_getconv(const void *ptr);


#ifdef __cplusplus
}
#endif

#endif


