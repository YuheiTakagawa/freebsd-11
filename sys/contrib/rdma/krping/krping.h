/*
 * $FreeBSD: releng/11.0/sys/contrib/rdma/krping/krping.h 297945 2016-04-14 00:25:11Z np $
 */

struct krping_stats {
	unsigned long long send_bytes;
	unsigned long long send_msgs;
	unsigned long long recv_bytes;
	unsigned long long recv_msgs;
	unsigned long long write_bytes;
	unsigned long long write_msgs;
	unsigned long long read_bytes;
	unsigned long long read_msgs;
	char name[16];
};

int krping_doit(char *, void *);
void krping_walk_cb_list(void (*)(struct krping_stats *, void *), void *);
void krping_init(void);
int krping_sigpending(void);
