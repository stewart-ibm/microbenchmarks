
CFLAGS=-O3

all: hash_sort_utf8 murmur3_128_bench mysql-innodb-new-checksum lockheck murmur3_bench nolockheck

clean:
	rm -f hash_sort_utf8 murmur3_128_bench mysql-innodb-new-checksum lockheck murmur3_bench nolockheck

hash_sort_utf8: hash_sort_utf8.c
	$(CC) $(CFLAGS) -o $@ $^

murmur3_128_bench: murmur3_128_bench.c
	$(CC) $(CFLAGS) -o $@ $^


mysql-innodb-new-checksum: mysql-innodb-new-checksum.c
	$(CC) $(CFLAGS) -o $@ $^


lockheck: lockheck.c
	$(CC) $(CFLAGS) -o $@ $^

murmur3_bench: murmur3_bench.c
	$(CC) $(CFLAGS) -o $@ $^

nolockheck: nolockheck.c
	$(CC) $(CFLAGS) -o $@ $^
