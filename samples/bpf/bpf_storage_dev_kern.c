// SPDX-License-Identifier: GPL-2.0
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

SEC("storage_dev")
int bpf_prog(struct bpf_storage_dev_ctx *ctx)
{
	char msg[] = "bpf_storage_dev_kern: rq_disk=%p, sector=%llu, len=%u\n";

	bpf_trace_printk(msg, sizeof(msg), ctx->rq->rq_disk, ctx->rq->__sector, ctx->rq->__data_len);

	return 0;
}

char _license[] SEC("license") = "GPL";
