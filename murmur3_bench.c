/* Murmur 3 implementation
 * straight from wikipedia
 * https://en.wikipedia.org/wiki/MurmurHash
 */
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
# define tole(x) (x)
# define tobe(x) __builtin_bswap32(x)
#else
# define tole(x) __builtin_bswap32(x)
# define tobe(x) (x)
#endif

#define ROT32(x, y) ((x << y) | (x >> (32 - y))) // avoid effort
uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed) {
	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	static const uint32_t r1 = 15;
	static const uint32_t r2 = 13;
	static const uint32_t m = 5;
	static const uint32_t n = 0xe6546b64;

	uint32_t hash = seed;

	const int nblocks = len / 4;
	const uint32_t *blocks = (const uint32_t *) key;
	int i;
	uint32_t k;
	for (i = 0; i < nblocks; i++) {
		k = blocks[i];
		k *= c1;
		k = ROT32(k, r1);
		k *= c2;

		hash ^= k;
		hash = ROT32(hash, r2) * m + n;
	}

	const uint8_t *tail = (const uint8_t *) (key + nblocks * 4);
	uint32_t k1 = 0;

	switch (len & 3) {
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];

		k1 *= c1;
		k1 = ROT32(k1, r1);
		k1 *= c2;
		hash ^= k1;
	}

	hash ^= len;
	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);

	return hash;
}

int main(int argc, char *argv[])
{
	unsigned long length, iterations;
	unsigned char *data;
	unsigned long i;
	unsigned int crc = 0;

	if (argc != 3) {
		fprintf(stderr, "Usage: murmur3_bench length iterations\n");
		exit(1);
	}

	length = strtoul(argv[1], NULL, 0);
	iterations = strtoul(argv[2], NULL, 0);

	data = memalign(getpagesize(), length);

	srandom(1);
	for (i = 0; i < length; i++)
		data[i] = random() & 0xff;

	for (i = 0; i < iterations; i++) {
		crc = tobe(crc);
		crc = murmur3_32(data, length, crc);
		crc = tobe(crc);
	}

	printf("MURMUR3: %08x\n", crc);

	return 0;
}
