.. SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)

BPF_PROG_TYPE_STORAGE_DEV
=========================

`BPF_PROG_TYPE_STORAGE_DEV` is a program type that can be attached to block devices
to monitor and filter I/O requests.

Context
-------

The context for this program type is `struct bpf_storage_dev_ctx`, which is
defined in `include/uapi/linux/bpf.h`. It provides access to the `struct request`
associated with the I/O operation.

.. code-block:: c

	struct bpf_storage_dev_ctx {
		struct request *rq;
	};

Attachment
----------

These programs can be attached to a block device using the `BPF_PROG_ATTACH`
command with `attach_type` set to `BPF_ATTACH_TYPE_STORAGE_DEV`. The `target_fd`
must be a file descriptor of a block device.

Return Value
------------

The program must return 0 to allow the I/O request to proceed, or a non-zero
value to drop the request.

Usage
-----

These programs can be used for various purposes, such as:

- I/O request filtering based on request properties (e.g., sector, size).
- I/O accounting and statistics gathering.
- Implementing custom I/O scheduling logic.
