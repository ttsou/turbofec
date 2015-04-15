#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#include "noise.h"
#include "codes.h"
#include "turbofec/conv.h"

#define MAX_LEN_BITS		32758
#define MAX_LEN_BYTES		(MAX_LEN_BITS / 8)
#define DEFAULT_ITER		10000
#define DEFAULT_THREADS		1
#define MAX_THREADS		32
#define MAX_CODES		2048

/*
 * Parameters for soft symbol generation
 *
 * Signal-to-noise ratio specified in dB and symbol amplitude, which has a
 * valid range from 0 (no signal) to 127 (saturation).
 */
#define DEFAULT_SNR	8.0
#define DEFAULT_AMP	32.0

/*
 * Command line arguments
 *
 * iter     - Number of iterations
 * threads  - Number of concurrent threads to launch for benchmark test
 * bench    - Enable the benchmarking test
 * length   - Enable length checks
 * ber      - Enable the bit-error-rate test
 * num      - Run code number if specified 
 */
struct cmd_options {
	int iter;
	int threads;
	int bench;
	int length;
	int ber;
	int num;
	float snr;
};

/* Argument passing struct for benchmark threads */
struct benchmark_thread_arg {
	const struct conv_test_vector *test;
	struct lte_conv_code *code;
	int base;
	int iter;
	int err;
};

struct conv_test_vector {
	const char *name;
	const char *spec;
	const struct lte_conv_code *code;
	int in_len;
	int out_len;
	int has_vec;
	const char vec_in[MAX_LEN_BYTES];
	const char vec_out[MAX_LEN_BYTES];
};

const struct conv_test_vector tests[] = {
	{
		.name = "GSM xCCH",
		.spec = "(N=2, K=5, non-recursive, flushed, not punctured)",
		.code = &gsm_conv_xcch,
		.in_len  = 224,
		.out_len = 456,
		.has_vec = 1,
		.vec_in  = { 0xf3, 0x1d, 0xb4, 0x0c, 0x4d, 0x1d, 0x9d, 0xae,
			     0xc0, 0x0a, 0x42, 0x57, 0x13, 0x60, 0x80, 0x96,
			     0xef, 0x23, 0x7e, 0x4c, 0x1d, 0x96, 0x24, 0x19,
			     0x17, 0xf2, 0x44, 0x99 },
		.vec_out = { 0xe9, 0x4d, 0x70, 0xab, 0xa2, 0x87, 0xf0, 0xe7,
			     0x04, 0x14, 0x7c, 0xab, 0xaf, 0x6b, 0xa1, 0x16,
			     0xeb, 0x30, 0x00, 0xde, 0xc8, 0xfd, 0x0b, 0x85,
			     0x80, 0x41, 0x4a, 0xcc, 0xd3, 0xc0, 0xd0, 0xb6,
			     0x26, 0xe5, 0x4e, 0x32, 0x49, 0x69, 0x38, 0x17,
			     0x33, 0xab, 0xaf, 0xb6, 0xc1, 0x08, 0xf3, 0x9f,
			     0x8c, 0x75, 0x6a, 0x4e, 0x08, 0xc4, 0x20, 0x5f,
			     0x8f },
	},
	{
		.name = "GPRS CS2",
		.spec = "(N=2, K=5, non-recursive, flushed, not punctured)",
		.code = &gsm_conv_cs2,
		.in_len  = 290,
		.out_len = 588,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GPRS CS3",
		.spec = "(N=2, K=5, non-recursive, flushed, not punctured)",
		.code = &gsm_conv_cs3,
		.in_len  = 334,
		.out_len = 676,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM RACH",
		.spec = "(N=2, K=5, non-recursive, flushed, not punctured)",
		.code = &gsm_conv_rach,
		.in_len  = 14,
		.out_len = 36,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM SCH",
		.spec = "(N=2, K=5, non-recursive, flushed, not punctured)",
		.code = &gsm_conv_sch,
		.in_len  = 35,
		.out_len = 78,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM TCH/FR",
		.spec = "(N=2, K=5, non-recursive, flushed, not punctured)",
		.code = &gsm_conv_tch_fr,
		.in_len  = 185,
		.out_len = 378,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM TCH/AFS 12.2",
		.spec = "(N=2, K=5, recursive, flushed, punctured)",
		.code = &gsm_conv_tch_afs_12_2,
		.in_len  = 250,
		.out_len = 448,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM TCH/AFS 10.2",
		.spec = "(N=3, K=5, recursive, flushed, punctured)",
		.code = &gsm_conv_tch_afs_10_2,
		.in_len  = 210,
		.out_len = 448,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM TCH/AFS 7.95",
		.spec = "(N=3, K=7, recursive, flushed, punctured)",
		.code = &gsm_conv_tch_afs_7_95,
		.in_len  = 165,
		.out_len = 448,
		.has_vec = 1,
		.vec_in  = { 0x87, 0x66, 0xc3, 0x58, 0x09, 0xd4, 0x06, 0x59,
			     0x10, 0xbf, 0x6b, 0x7f, 0xc8, 0xed, 0x72, 0xaa,
			     0xc1, 0x3d, 0xf3, 0x1e, 0xb0 },
		.vec_out = { 0x92, 0xbc, 0xde, 0xa0, 0xde, 0xbe, 0x01, 0x2f,
			     0xbe, 0xe4, 0x61, 0x32, 0x4d, 0x4f, 0xdc, 0x41,
			     0x43, 0x0d, 0x15, 0xe0, 0x23, 0xdd, 0x18, 0x91,
			     0xe5, 0x36, 0x2d, 0xb7, 0xd9, 0x78, 0xb8, 0xb1,
			     0xb7, 0xcb, 0x2f, 0xc0, 0x52, 0x8f, 0xe2, 0x8c,
			     0x6f, 0xa6, 0x79, 0x88, 0xed, 0x0c, 0x2e, 0x9e,
			     0xa1, 0x5f, 0x45, 0x4a, 0xfb, 0xe6, 0x5a, 0x9c },
	},
	{
		.name = "GSM TCH/AFS 7.4",
		.spec = "(N=3, K=5, recursive, flushed, punctured)",
		.code = &gsm_conv_tch_afs_7_4,
		.in_len  = 154,
		.out_len = 448,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM TCH/AFS 6.7",
		.spec = "(N=4, K=5, recursive, flushed, punctured)",
		.code = &gsm_conv_tch_afs_6_7,
		.in_len  = 140,
		.out_len = 448,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM TCH/AFS 5.9",
		.spec = "(N=4, K=7, recursive, flushed, punctured)",
		.code = &gsm_conv_tch_afs_5_9,
		.in_len  = 124,
		.out_len = 448,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM TCH/AHS 7.95",
		.spec = "(N=2, K=5, recursive, flushed, punctured)",
		.code = &gsm_conv_tch_ahs_7_95,
		.in_len  = 129,
		.out_len = 188,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM TCH/AHS 7.4",
		.spec = "(N=2, K=5, recursive, flushed, punctured)",
		.code = &gsm_conv_tch_ahs_7_4,
		.in_len  = 126,
		.out_len = 196,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM TCH/AHS 6.7",
		.spec = "(N=2, K=5, recursive, flushed, punctured)",
		.code = &gsm_conv_tch_ahs_6_7,
		.in_len  = 116,
		.out_len = 200,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM TCH/AHS 5.9",
		.spec = "(N=2, K=5, recursive, flushed, punctured)",
		.code = &gsm_conv_tch_ahs_5_9,
		.in_len  = 108,
		.out_len = 208,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM TCH/AHS 5.15",
		.spec = "(N=3, K=5, recursive, flushed, punctured)",
		.code = &gsm_conv_tch_ahs_5_15,
		.in_len  = 97,
		.out_len = 212,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "GSM TCH/AHS 4.75",
		.spec = "(N=3, K=7, recursive, flushed, punctured)",
		.code = &gsm_conv_tch_ahs_4_75,
		.in_len  = 89,
		.out_len = 212,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "WiMax FCH",
		.spec = "(N=2, K=7, non-recursive, tail-biting, non-punctured)",
		.code = &wimax_conv_fch,
		.in_len  = 48,
		.out_len = 96,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{
		.name = "LTE PBCH",
		.spec = "(N=3, K=7, non-recursive, tail-biting, non-punctured)",
		.code = &lte_conv_pbch,
		.in_len  = 512,
		.out_len = 1536,
		.has_vec = 0,
		.vec_in  = { },
		.vec_out = { },
	},
	{ },
};

static void print_codes()
{
	int i = 1;
	const struct conv_test_vector *test;

	printf("\n");
	printf("Code  0:  Test all codes\n");
	for (test = tests; test->name; test++)
		printf("Code %2i:  %-18s %s\n", i++, test->name, test->spec);
	printf("\n");
}

static void fill_random(uint8_t *b, int n)
{
	int i, r, m, c;

	c = 0;
	r = rand();
	m = sizeof(int) - 1;

	for (i = 0; i < n; i++) {
		if (c++ == m) {
			r = rand();
			c = 0;
		}

		b[i] = (r >> (i % m)) & 0x01;
	}
}

/* Generate soft bits with AWGN channel */
static int uint8_to_err(int8_t *dst, uint8_t *src, int n, float snr)
{
	int i, err = 0;

	add_noise(src, dst, n, snr, DEFAULT_AMP);

	for (i = 0; i < n; i++) {
		if ((src[i] && (dst[i] <= 0)) || (!src[i] && (dst[i] >= 0)))
			err++;
	}

	return err;
}

/* Output error input/output error rates */
static void print_error_results(const struct conv_test_vector *test,
				int iber, int ober, int fer, int iter)
{
	printf("[..] Input BER.......................... %f\n",
	       (float) iber / (iter * test->out_len));
	printf("[..] Output BER......................... %f\n",
	       (float) ober / (iter * test->out_len));
	printf("[..] Output FER......................... %f ",
	       (float) fer / iter);
	if (fer > 0)
		printf("(%i/%i)\n", fer, iter);
	else
		printf("\n");
}

/* Timed performance benchmark */
static double get_timed_results(struct timeval *tv0, struct timeval *tv1,
				const struct conv_test_vector *test,
				int iter, int threads)
{
	double elapsed;

	elapsed = (tv1->tv_sec - tv0->tv_sec);
	elapsed += (tv1->tv_usec - tv0->tv_usec) / 1e6;
	printf("[..] Elapsed time....................... %f secs\n", elapsed);
	printf("[..] Rate............................... %f Mbps\n",
	       (float) threads * test->in_len * iter / elapsed / 1e6);

	return elapsed;
}

/* Bit error rate test */
static int error_test(const struct conv_test_vector *test,
		      int iter, float snr)
{
	int i, n, l, iber = 0, ober = 0, fer = 0;
	int8_t *bs;
	uint8_t *bu0, *bu1;
	int (*decode) (const struct lte_conv_code *, const int8_t *, uint8_t *);

	bu0 = malloc(sizeof(uint8_t) * MAX_LEN_BITS);
	bu1 = malloc(sizeof(uint8_t) * MAX_LEN_BITS);
	bs  = malloc(sizeof(int8_t) * MAX_LEN_BITS);

	decode = lte_conv_decode;

	for (i = 0; i < iter; i++) {
		fill_random(bu0, test->in_len);

		l = lte_conv_encode(test->code, bu0, bu1);
		if (l != test->out_len) {
			fprintf(stderr, "[!] Failed length check (%i)\n", l);
			fprintf(stderr, "[!] Expected length %i\n", test->out_len);
			return -1;
		}

		iber += uint8_to_err(bs, bu1, l, snr);
		decode(test->code, bs, bu1);

		for (n = 0; n < test->in_len; n++) {
			if (bu0[n] != bu1[n])
				ober++;
		}

		if (memcmp(bu0, bu1, test->in_len))
			fer++;
	}

	print_error_results(test, iber, ober, fer, iter);

	free(bs);
	free(bu1);
	free(bu0);

	return 0;
}

static int init_thread_arg(struct benchmark_thread_arg *arg,
			    const struct conv_test_vector *test, int iter)
{
	int8_t *bs;
	uint8_t *bu;
	int (*decode) (const struct lte_conv_code *, const int8_t *, uint8_t *);
	struct lte_conv_code *code;

	code = (struct lte_conv_code *) malloc(sizeof(struct lte_conv_code));
	memcpy(code, test->code, sizeof(struct lte_conv_code));

	bu = malloc(sizeof(uint8_t) * MAX_LEN_BITS);
	bs = malloc(sizeof(int8_t) * MAX_LEN_BITS);

	decode = lte_conv_decode;
	decode(code, bs, bu);

	arg->test = test;
	arg->iter = iter;
	arg->code = code;
	arg->err = 0;

	free(bs);
	free(bu);

	return 0;
}

/* One benchmark benchmark thread with random valued input */
static void *thread_test(void *ptr)
{
	int i;
	int8_t *bs;
	uint8_t *bu0, *bu1;
	struct benchmark_thread_arg *arg = (struct benchmark_thread_arg *) ptr;
	int (*decode) (const struct lte_conv_code *, const int8_t *, uint8_t *);

	bu0 = malloc(sizeof(uint8_t) * MAX_LEN_BITS);
	bu1 = malloc(sizeof(uint8_t) * MAX_LEN_BITS);
	bs  = malloc(sizeof(int8_t) * MAX_LEN_BITS);

	decode = lte_conv_decode;

	for (i = 0; i < arg->iter; i++)
		decode(arg->code, bs, bu1);

	free(bs);
	free(bu1);
	free(bu0);

	pthread_exit(NULL);
}

/* Fire off benchmark threads and measure elapsed time */
static double run_benchmark(const struct conv_test_vector *test,
			    struct benchmark_thread_arg *args,
			    int num_threads, int iter)
{
	int i, rc, err = 0;
	void *status;
	struct timeval tv0, tv1;
	pthread_t threads[MAX_THREADS];

	for (i = 0; i < num_threads; i++) {
		rc = init_thread_arg(&args[i], test, iter);
		if (rc < 0)
			return -1.0;
	}


	gettimeofday(&tv0, NULL);
	for (i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL,
			       thread_test, (void *) &args[i]);
	}
	for (i = 0; i < num_threads; i++) {
		pthread_join(threads[i], &status);
		err |= args[i].err;
	}
	gettimeofday(&tv1, NULL);

	if (err)
		return -1.0;

	return get_timed_results(&tv0, &tv1, test, iter, num_threads);
}

static void print_help()
{
	fprintf(stdout, "Options:\n"
		"  -h    This text\n"
		"  -i    Number of iterations\n"
		"  -j    Number of threads for benchmark (EXPERIMENTAL)\n"
		"  -a    Run all tests\n"
		"  -b    Run benchmark tests\n"
		"  -n    Run length checks\n"
		"  -e    Run bit error rate tests\n"
		"  -r    Specify SNR in dB (default %2.1f dB)\n"
		"  -c    Test specific code\n"
		"  -l    List supported codes\n", DEFAULT_SNR);
}

static void handle_options(int argc, char **argv, struct cmd_options *cmd)
{
	int option;

	cmd->iter = DEFAULT_ITER;
	cmd->threads = DEFAULT_THREADS;
	cmd->bench = 0;
	cmd->length = 0;
	cmd->ber = 0;
	cmd->num = 0;
	cmd->snr = DEFAULT_SNR;

	while ((option = getopt(argc, argv, "hi:baesoc:r:lj:")) != -1) {
		switch (option) {
		case 'h':
			print_help();
			exit(0);
			break;
		case 'i':
			cmd->iter = atoi(optarg);
			if (cmd->iter < 1) {
				printf("Iterations must be at least 1\n");
				exit(0);
			}
			break;
		case 'a':
			cmd->bench = 1;
			cmd->length = 1;
			cmd->ber = 1;
			break;
		case 'b':
			cmd->bench = 1;
			break;
		case 'n':
			cmd->length = 1;
			break;
		case 'e':
			cmd->ber = 1;
			break;
		case 'c':
			cmd->num = atoi(optarg);
			if (cmd->num < 0) {
				print_codes();
				exit(0);
			}
			break;
		case 'r':
			cmd->snr = atof(optarg);
			break;
		case 'l':
			print_codes();
			exit(0);
			break;
		case 'j':
			cmd->threads = atoi(optarg);
			if ((cmd->threads < 1) ||
			    (cmd->threads > MAX_THREADS)) {
				printf("Threads must be between 1 to %i\n",
				       MAX_THREADS);
				exit(0);
			}
			break;
		default:
			print_help();
			exit(0);
		}
	}

	if (!cmd->bench && !cmd->length && !cmd->ber) {
		cmd->length = 1;
		cmd->ber = 1;
	}
}

int main(int argc, char *argv[])
{
	int cnt = 0;
	const struct conv_test_vector *test;
	double elapsed = 0.0;
	struct benchmark_thread_arg args[MAX_THREADS * 2];
	struct cmd_options cmd;

	handle_options(argc, argv, &cmd);
	srandom(time(NULL));

	for (test = tests; test->name; test++) {
		if ((cmd.num > 0) && (cmd.num != ++cnt))
			continue;

		printf("\n=================================================\n");
		printf("[+] Testing: %s\n", test->name);
		printf("[.] Specs: %s\n", test->spec);

		/* BER test */
		if (cmd.ber) {
			printf("\n[.] BER test:\n");
			printf("[..] Testing:\n");
			if (error_test(test, cmd.iter, cmd.snr) < 0)
				return -1;
		}

		if (!cmd.bench)
			continue;

		/* Timed benchmark tests */
		printf("\n[.] Performance benchmark:\n");
		printf("[..] Encoding / Decoding %i bursts on %i thread(s):\n",
		       cmd.iter * cmd.threads, cmd.threads);

		printf("[..] Testing:\n");
		elapsed = run_benchmark(test, args, cmd.threads, cmd.iter);
		if (elapsed < 0.0)
			goto shutdown;

		printf("\n");
	}
	printf("\n");

shutdown:
	return 0;
}
