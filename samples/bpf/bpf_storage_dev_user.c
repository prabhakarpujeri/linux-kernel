// SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <bpf/bpf.h>
#include <bpf/libbpf.h>

#define BPF_OBJ_FILE "bpf_storage_dev_kern.o"

int main(int argc, char **argv)
{
	struct bpf_object *obj;
	struct bpf_program *prog;
	int dev_fd, prog_fd, err;
	char *dev_name;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <device>\n", argv[0]);
		return 1;
	}
	dev_name = argv[1];

	obj = bpf_object__open_file(BPF_OBJ_FILE, NULL);
	if (libbpf_get_error(obj)) {
		fprintf(stderr, "ERROR: opening BPF object file failed\n");
		return 1;
	}

	if (bpf_object__load(obj)) {
		fprintf(stderr, "ERROR: loading BPF object file failed\n");
		goto cleanup;
	}

	prog = bpf_object__find_program_by_name(obj, "bpf_prog");
	if (!prog) {
		fprintf(stderr, "ERROR: finding a prog in obj file failed\n");
		goto cleanup;
	}
	prog_fd = bpf_program__fd(prog);

	dev_fd = open(dev_name, O_RDONLY);
	if (dev_fd < 0) {
		fprintf(stderr, "ERROR: opening device %s failed: %s\n",
			dev_name, strerror(errno));
		goto cleanup;
	}

	err = bpf_prog_attach(prog_fd, dev_fd, BPF_ATTACH_TYPE_STORAGE_DEV, 0);
	if (err) {
		fprintf(stderr, "ERROR: attaching program failed: %s\n",
			strerror(errno));
		goto cleanup_dev;
	}

	printf("Program attached. Reading trace output...\n");
	system("cat /sys/kernel/debug/tracing/trace_pipe");

	bpf_prog_detach(prog_fd, dev_fd, BPF_ATTACH_TYPE_STORAGE_DEV);

cleanup_dev:
	close(dev_fd);
cleanup:
	bpf_object__close(obj);
	return 0;
}
