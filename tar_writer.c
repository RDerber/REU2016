#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>

/****************************************************************************************/
/*	tar writer	*/
/****************************************************************************************/
#ifdef TAR_WRITER
{
#endif
#define  LF_OLDNORMAL '\0'       /* Normal disk file, Unix compatible */
#define  LF_NORMAL    '0'        /* Normal disk file */
#define  LF_LINK      '1'        /* Link to previously dumped file */
#define  LF_SYMLINK   '2'        /* Symbolic link */
#define  LF_CHR       '3'        /* Character special file */
#define  LF_BLK       '4'        /* Block special file */
#define  LF_DIR       '5'        /* Directory */
#define  LF_FIFO      '6'        /* FIFO special file */
#define  LF_CONTIG    '7'        /* Contiguous file */

struct tar_header {
	char name[100];		/* file name (includes trailing / if directory) */
	char mode[8];		/* file mode */
	char uid[8];		/* owner user id */
	char gid[8];		/* owner group id */
	char size[12];		/* file length in bytes */
	char mtime[12];		/* modify time */
	char cksum[8];		/* header checksum */
	char link;		/* 1 for linked file, 2 for symlink, 0 otherwise */
	char linkname[100];	/* name of linked file */
}; 

unsigned int calc_header_checksum (struct tar_header *hdr)
{
	unsigned int i, sum = 0;
	unsigned char *ptr = (unsigned char *)hdr;
	
	if (!hdr)
		return 0;
	
	memset(hdr->cksum, ' ', sizeof(hdr->cksum));

	for (i = 0; i < 512; i++)
    		sum += (0xFF & ptr[i]);

	return sum;
}

const char *get_last (const char *src, unsigned char sep)
{
	const char *base = src;
	
	while (*src)
		src++;
	
	while (src >= base && *src != sep)
		src--;
	
	return (src == base) ? NULL : src;
}

int read_file (const char *src_path, unsigned char **dst)
{
	int src_fd;
	int res, pos = 0, rem;
	struct stat st;
	
	if (stat(src_path, &st) < 0 || 
		S_ISDIR(st.st_mode) || 
		(rem = st.st_size) <= 0) {
		fprintf(stderr, "[%s]: stat failed\n", __func__);
		return -1;
	}
		
	if ((src_fd = open(src_path, O_RDONLY, 0777)) < 0) {
		fprintf(stderr, "[%s]: could not open socket\n", __func__);
		return -1;
	}
	
	*dst = calloc(1, st.st_size);
	
	while ((res = (int)read(src_fd, (*dst) + pos, rem)) > 0) {
		rem -= res;
		pos += res;
	}
	
	close(src_fd);
	
	fprintf(stderr, "[%s]: read %d bytes\n", __func__, (int)st.st_size);
	
	return (int)st.st_size;
}

void generate_padded_octal (char *dst, int src, int len)
{
	snprintf(dst, len, "%0.*o", len - 1, src);
}

int find_linktype (mode_t mode)
{
	if (S_ISCHR(mode))
		return LF_CHR;
	else if (S_ISBLK(mode))
		return LF_BLK;
	else if (S_ISDIR(mode))
		return LF_DIR;
	else if (S_ISFIFO(mode))
		return LF_FIFO;
	else if (S_ISREG(mode))
		return LF_OLDNORMAL;
	else if (S_ISLNK(mode))
		return LF_SYMLINK;
		
	return -1;
}

int write_file_entry (char *wpath, const char *d_name, struct dirent *ds, 
			struct stat st, int tfd)
{
	char buffer[512];
	struct tar_header *hdr = (struct tar_header *)&buffer[0];
	int cksum = 0, i = 0, len = 0;
	unsigned char *buf = NULL;
	char *ptr;
	
	memset(buffer, 0, sizeof(buffer));
	
	ptr = strstr(wpath, d_name) + strlen(d_name) + 1;

	strncpy(hdr->name, ptr, sizeof(hdr->name));
	
	generate_padded_octal(hdr->mode, st.st_mode, sizeof(hdr->mode));
	generate_padded_octal(hdr->uid, st.st_uid, sizeof(hdr->uid));
	generate_padded_octal(hdr->gid, st.st_gid, sizeof(hdr->gid));
	generate_padded_octal(hdr->mtime, st.st_mtime, sizeof(hdr->mtime));
		
	if (S_ISDIR(st.st_mode)) {
		hdr->name[strlen(hdr->name)] = '/';
		generate_padded_octal(hdr->size, 0, sizeof(hdr->size));
	} else {
		generate_padded_octal(hdr->size, st.st_size, sizeof(hdr->size));
	}

	if ((hdr->link = find_linktype(st.st_mode)) < 0)
		return -1;

	cksum = calc_header_checksum(hdr);
	generate_padded_octal(hdr->cksum, cksum, sizeof(hdr->cksum) - 1);
	
	hdr->cksum[6] = ' ';
	hdr->cksum[7] = '\0';
	
	write(tfd, buffer, sizeof(buffer));
	memset(buffer, 0, sizeof(buffer));
	 
	if (S_ISDIR(st.st_mode) || (len = read_file(wpath, &buf)) < 0) {
		len = 0;
		i = 0;
	} else {
		for (i = 0; i < 512; i++)
			if (!((len + i) & 0x1ff))
				break;
				
		if (write(tfd, buf, len) != len ||
			write(tfd, buffer, i) != i) 
			printf("error writing\n");
	}
	
	printf("read/wrote %d bytes + %d pad = %d total\n", len, i, len + i);

	free(buf);
	
	return 0;
}

int read_folder (const char *src_path, const char *d_name, int tfd)
{
	struct dirent *ds;
	struct stat st;
	char wpath[1024];
	DIR *dr;
	
	if ((dr = opendir(src_path)) == NULL)
		return -1;
		
	fprintf(stderr, "[%s]: opened dir \"%s\"\n", __func__, src_path);

	while ((ds = readdir(dr))) {
		if (*(ds->d_name) == '.') 
			continue;

		memset(wpath, 0, sizeof(wpath));
		snprintf(wpath, sizeof(wpath), "%s/%s", src_path, ds->d_name);
		
		if (stat(wpath, &st) < 0) 
			continue;
				
		if (write_file_entry(wpath, d_name, ds, st, tfd) < 0)
			printf("error\n");
			
		if (S_ISDIR(st.st_mode))
			if (read_folder(wpath, d_name, tfd) < 0)
				printf("error\n");
	}
	
	closedir(dr);	
	return 0;
}

int make_tar_file (const char *src, const char *dst)
{
	char blk_end[1024];
	int tfd, retn = 0;
	const char *endl = get_last(src, '/') + 1;
	
	if ((tfd = open(dst, O_CREAT|O_TRUNC|O_RDWR, 0777)) < 0) {
		fprintf(stderr, "[%s]: error opening socket\n", __func__);
		return -1;
	}
	
	if (read_folder(src, endl, tfd) < 0)
		retn = -1;
	
	memset(blk_end, 0, sizeof(blk_end));
	if (write(tfd, blk_end, sizeof(blk_end)) != sizeof(blk_end))
		retn = -1;
	
	close(tfd);

	return retn;
}
#ifdef TAR_WRITER
}
#endif

/****************************************************************************************/
/*	bit stream (Explain)	*/
/****************************************************************************************/
#ifdef BIT_STREAM
{
#endif
struct bit_stream {
	int bit_pos;
	int byte_pos;
	
	unsigned char *buf;
	int bytes;
};

void print_bits (uint64_t in, int num)
{
	int j;
	
	printf("%llu: ", in);
	
	for (j = 0; j < num; j++)
		putchar((in & (1 << j)) ? '1' : '0');

	printf("\n");
}

uint64_t reverse_bits (uint64_t in, int num)
{
	uint64_t out = 0;
	int i;
	
	for (i = 0; i < num; i++)
		out = (out << 1) | ((in & (1 << i)) ? 1 : 0);
	
	return out;
}

int alloc_bit_stream (struct bit_stream **bs, int bytes_max)
{
	if (((*bs) = calloc(sizeof(struct bit_stream), 1)) == NULL)
		return -1;
	
	(*bs)->bit_pos = 0;
	(*bs)->byte_pos = 0;
	
	(*bs)->bytes = bytes_max;
	
	if (((*bs)->buf = calloc(bytes_max, 1)) == NULL) {
		free((*bs));
		return -1;
	}
	
	return 0;
}

int write_bit_stream (struct bit_stream *bs, uint64_t src, int bits)
{
	int i;
	
	for (i = 0; i < bits; i++) {
		if (bs->bit_pos >= 8) {
			bs->bit_pos = 0;
			if ((++bs->byte_pos) >= bs->bytes) 
				return -1;
		}
		
		if (src & (1 << i))
			bs->buf[bs->byte_pos] |= (1 << bs->bit_pos);
		else
			bs->buf[bs->byte_pos] &= ~(1 << bs->bit_pos);
		
		bs->bit_pos++;
	}
#ifdef PRINT_BITS
	printf("wrote %d bits (%llu) to stream (now at byte %d bit %d)\n", 
		bits, src, bs->byte_pos, bs->bit_pos);
#endif

	return 0;
}

void print_bit_stream (struct bit_stream *bs)
{
	int i, j;
	
	if (bs->byte_pos + 1 >= bs->bytes)
		return;
	
	for (i = 0; i < bs->byte_pos + 1; i++) {
		printf("\nbyte #%.03d: %.02x: ", i, bs->buf[i]);
		for (j = 0; j < 8; j++) {
			if (bs->buf[i] & (1 << j))
				putchar('1');
			else
				putchar('0');
		}
	}
}

void free_bit_stream (struct bit_stream *bs)
{
	free(bs->buf);
	free(bs);
}

void test_bit_stream (void)
{
	struct bit_stream *bs;
	
	printf("[%s]: testing bit stream\n", __func__);
	
	alloc_bit_stream(&bs, 10);
	
	write_bit_stream(bs, 0x55, 8);
	write_bit_stream(bs, 0, 8);
	write_bit_stream(bs, 1, 2);
	write_bit_stream(bs, 0xFFFF, 11);
	
	print_bit_stream(bs);
	
	printf("\n");
	
	free_bit_stream(bs);
}
#ifdef BIT_STREAM
}
#endif

/****************************************************************************************/
/*	3rd party functions	*/
/****************************************************************************************/

/* Return the CRC of the bytes buf[0..len-1]. */
unsigned long crc (unsigned char *buf, int len)
{
	unsigned long c = 0L ^ 0xffffffffL;
  	unsigned long crc_table[256];
	int n, k;

	for (n = 0; n < 256; n++) {
		c = (unsigned long) n;
		for (k = 0; k < 8; k++) {
			if (c & 1) {
				c = 0xedb88320L ^ (c >> 1);
			} else {
				c = c >> 1;
			}
		}
		crc_table[n] = c;
	}
  
  	c = 0L ^ 0xffffffffL;
	
	for (n = 0; n < len; n++)
		c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);

	return c ^ 0xffffffffL;
}


/****************************************************************************************/
/*	huffman coder	*/
/****************************************************************************************/
#ifdef HUFF_CODER
{
#endif
struct huff_list {
	uint16_t val;
	uint32_t count;
	
	uint16_t hcode;
	uint16_t hc_len;
};

struct min_heap {
	unsigned int size;
	
	struct mh_node {
		unsigned int data;
		unsigned int freq;
		struct mh_node *left;
		struct mh_node *right;
	} **array;
};

struct mh_node *new_node (unsigned int data, unsigned int freq)
{
	struct mh_node *temp = calloc(sizeof(struct mh_node), 1);
	
	//fprintf(stderr, "[%s]: node %p alloc'd\n", __func__, temp);
	
	temp->left = temp->right = NULL;
	temp->data = data;
	temp->freq = freq;
	
	return temp;
}

void swapmh_node (struct mh_node **a, struct mh_node **b)
{
	struct mh_node *t = *a;
	*a = *b;
	*b = t;
}

void min_heapify (struct min_heap *mh, int idx)
{
	int smallest = idx;
	int left = 2 * idx + 1;
	int right = 2 * idx + 2;
 
	if (left < mh->size && mh->array[left]->freq < mh->array[smallest]->freq)
		smallest = left;
 
	if (right < mh->size && mh->array[right]->freq < mh->array[smallest]->freq)
		smallest = right;
 
	if (smallest != idx) {
        	swapmh_node(&mh->array[smallest], &mh->array[idx]);
        	min_heapify(mh, smallest);
	}
}

struct mh_node *extract_min (struct min_heap *min_heap)
{
	struct mh_node *temp = min_heap->array[0];
	
	min_heap->array[0] = min_heap->array[--min_heap->size];

	min_heapify(min_heap, 0);
	
	return temp;
}

void mh_insert (struct min_heap *min_heap, struct mh_node *mh_node)
{
	int i = min_heap->size++; 

	while (i && mh_node->freq < min_heap->array[(i - 1) / 2]->freq) {
		min_heap->array[i] = min_heap->array[(i - 1) / 2];
		i = (i - 1) / 2;
	}

	min_heap->array[i] = mh_node;
}

void make_huffman_tree (struct mh_node **tree, struct huff_list *hlist, int unique, int max)
{
	struct mh_node *left, *right, *top;
	int i;
	struct min_heap mh;
	
	memset(&mh, 0, sizeof(struct min_heap));
   
	mh.size = unique;
	mh.array = calloc(2 * unique, sizeof(struct mh_node *));
	
	for (i = 0; i < unique; ++i)
		mh.array[i] = new_node(hlist[i].val, hlist[i].count);

	for (i = (mh.size - 2) / 2; i >= 0; --i)
		min_heapify(&mh, i);

	while (mh.size != 1) {
		left = extract_min(&mh);
		right = extract_min(&mh);

		top = new_node(max + 1, left->freq + right->freq);
		top->left = left;
		top->right = right;
		mh_insert(&mh, top);
	}
	
	*tree = extract_min(&mh);

	free(mh.array);
	mh.array = NULL;
}

void find_huffcode (struct huff_list *hl, int chars, struct mh_node *base, int buf, int pos)
{
	if (base->left) {
		buf &= ~(1 << pos);
		find_huffcode(hl, chars, base->left, buf, pos + 1);
	} 
	 
	if (base->right) { 
		buf |= (1 << pos);
		find_huffcode(hl, chars, base->right, buf, pos + 1);
	}
	
	if ((!(base->left) && !(base->right))) {
		int j;
		
		//print_bits(buf, pos);	
		
		for (j = 0; j < chars; j++) {
			if (hl[j].val == base->data) {
				if (hl[j].count == 0) {
					hl[j].hcode = hl[j].hc_len = 0;
				}
				
				printf("\thl[%3d] = (freq %4d) (val %3d) (len %2d) code ", 
					j, hl[j].count, base->data, pos);
					
				print_bits(buf, pos);	
				
				hl[j].hcode = reverse_bits(buf, pos);
				hl[j].hc_len = pos;
				break;
			}
		}
	}
	
	//fprintf(stderr, "[%s]: node %p freed\n", __func__, base);
	free(base);
	base = NULL;
}

void print_huffcodes (struct huff_list *hlist, int start, int end)
{
	int i;
	
	for (i = start; i < end; i++) {
		printf("#%3d, val = %3d, count = %4d, code =(%2d bits): ", i, 
			hlist[i].val, hlist[i].count, hlist[i].hc_len);
			
		print_bits(hlist[i].hcode, hlist[i].hc_len);
	}
}

int huff_encode (uint16_t *src, int num, int chars, struct huff_list **hlist)
{
	int *count = calloc(sizeof(int), chars + 1);
	struct mh_node *htree = NULL;
	int i, j;
	
	*hlist = calloc(chars + 1, sizeof(struct huff_list));
	
	for (i = 0; i < num; i++)
		count[src[i]]++;
		
	if (chars >= 256)
		count[256]++;

	for (i = 0, j = 0; i < chars; i++) { 
		if (count[i]) {
			(*hlist)[j].val = i;
			(*hlist)[j].count = count[i];
			printf("%3d: %4d entries with value %d\n", j, count[i], i);
			j++;
		} 
	}
	
	printf("[%s]: encoding %d values (%d max char, %d unique chars)\n",
		__func__, num, chars, j);
	
	make_huffman_tree(&htree, *hlist, j, chars);
	
	find_huffcode(*hlist, j, htree, 0, 0);
	free(count);
	
	return j;
}

void make_static_hlist (struct huff_list **phlist, struct huff_list **pdlist, int **bit_count)
{
	(*bit_count) = calloc(sizeof(int), 288);
	struct huff_list *hlist = (*phlist) = calloc(sizeof(struct huff_list), 288);
	struct huff_list *dlist = (*pdlist) = calloc(sizeof(struct huff_list), 32);
	
	int n = 0, len;
	while (n <= 143) hlist[n++].hc_len = 8, bit_count[8]++;
	while (n <= 255) hlist[n++].hc_len = 9, bit_count[9]++;
	while (n <= 279) hlist[n++].hc_len = 7, bit_count[7]++;
	while (n <= 287) hlist[n++].hc_len = 8, bit_count[8]++;

	uint16_t next_code[16]; /* next code value for each bit length */
	uint16_t code = 0;              /* running code value */
	int bits;                  /* bit index */

	for (bits = 1; bits <= 15; bits++)
		next_code[bits] = code = (code + (*bit_count)[bits-1]) << 1;

	for (n = 0; n <= 287; n++) {
		hlist[n].val = n;
		
		if ((len = hlist[n].hc_len) == 0) 
			continue;

		hlist[n].hcode = reverse_bits(next_code[len]++, len);
		printf("hlist[%d] = %d (%d bits)\n", n, hlist[n].hcode, hlist[n].hc_len);
	}

	for (n = 0; n < 32; n++) {
		dlist[n].hc_len = 5;
		dlist[n].val = n;
        	dlist[n].hcode = reverse_bits(n, 5);
	}
}

void test_hcoder (void)
{
	uint16_t test[17] = {1, 2, 1, 2, 3, 4, 4, 4, 5, 6, 7, 7, 7, 7, 7, 7, 10};
	struct huff_list *hlist;
	
	printf("[%s]: testing huffman coder\n", __func__);
	
	huff_encode(test, 17, 288, &hlist);
	
	print_huffcodes(hlist, 0, 17);
	
	free(hlist);
	
	printf("\n");
}	

#ifdef HUFF_CODER
}
#endif

/****************************************************************************************/
/*	huff length coder	*/
/****************************************************************************************/
#ifdef HUFF_LCODER
{
#endif
/*{
    		 Extra               Extra               Extra
            Code Bits Length(s) Code Bits Lengths   Code Bits Length(s)
            ---- ---- ------     ---- ---- -------   ---- ---- -------
             257   0     3       267   1   15,16     277   4   67-82
             258   0     4       268   1   17,18     278   4   83-98
             259   0     5       269   2   19-22     279   4   99-114
             260   0     6       270   2   23-26     280   4  115-130
             261   0     7       271   2   27-30     281   5  131-162
             262   0     8       272   2   31-34     282   5  163-194
             263   0     9       273   3   35-42     283   5  195-226
             264   0    10       274   3   43-50     284   5  227-257
             265   1  11,12      275   3   51-58     285   0    258
             266   1  13,14      276   3   59-66
}*/

struct huff_len_table {
	uint16_t len_code;
	uint16_t num_exbits;
	uint16_t ext_val;
};

void make_hufftable (struct huff_len_table hlt[259])
{
	int length = 3, n, code;
	static const short lext[29] = 
		{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
		  3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0 };

	memset(hlt, 0, sizeof(struct huff_len_table) * 259);

	for (code = 0; code < 29-1; code++) {
		for (n = 0; n < (1 << lext[code]); n++) {
			hlt[length].len_code = 257 + code;
			hlt[length].num_exbits = lext[code];
			hlt[length].ext_val = (length - 3) & ((1 << lext[code]) - 1);
			
			length++;
		}
	} 
	
	hlt[length - 1].len_code = 285;
	hlt[length - 1].num_exbits = hlt[length - 1].ext_val = 0;
}

void encode_hlist (struct huff_list *hlist, int num)
{
	struct huff_len_table hlt[259];
	int len;
	
	printf("[%s]: testing huffman length coding\n", __func__);
	
	make_hufftable(hlt);
	
	for (len = 0; len < 259; len++) {
		printf("len %d = code %d with %d extra bits (%d)\n", len, hlt[len].len_code, 
			hlt[len].num_exbits, hlt[len].ext_val);	
	}
	
	printf("\n");
}

#ifdef HUFF_LCODER
}
#endif

/****************************************************************************************/
/*	dist len coder	*/
/****************************************************************************************/
#ifdef DLC
{
#endif

/*                  Extra           Extra               Extra
             Code Bits Dist  Code Bits   Dist     Code Bits Distance
             ---- ---- ----  ---- ----  ------    ---- ---- --------
               0   0    1     10   4     33-48    20    9   1025-1536
               1   0    2     11   4     49-64    21    9   1537-2048
               2   0    3     12   5     65-96    22   10   2049-3072
               3   0    4     13   5     97-128   23   10   3073-4096
               4   1   5,6    14   6    129-192   24   11   4097-6144
               5   1   7,8    15   6    193-256   25   11   6145-8192
               6   2   9-12   16   7    257-384   26   12  8193-12288
               7   2  13-16   17   7    385-512   27   12 12289-16384
               8   3  17-24   18   8    513-768   28   13 16385-24576
               9   3  25-32   19   8   769-1024   29   13 24577-32768
*/

int dist_code (int dist, int *code, int *exbits, int *ebval)
{
	int i;
	static const short dext[30] = {
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
		7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
		12, 12, 13, 13};
	static const short dists[30] = {
		1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
		257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
		8193, 12289, 16385, 24577};

	if (!code || !exbits || dist < 0 || dist > 32768)
		return -1;
		
	for (i = 0; i < 30; i++) {
		if (dist < dists[i + 1]) {
			*code = i;
			break;
		}
	}
	
	if (i == 30)
		*code = 29;

	*exbits = dext[*code];
	*ebval = dist - dists[*code];

	return 0;
}

void test_dlcoder (void)
{
	int a, b, c, i;
	
	for (i = 0; i < 500; i++) {
		dist_code(i, &a, &b, &c);
		printf("%d: code %d bits %d = %d\n", i, a, b, c);
	}
	
	for (i = 500; i < 40000; i += 500) {
		dist_code(i, &a, &b, &c);
		printf("%d: code %d bits %d = %d\n", i, a, b, c);
	}
}

#ifdef DLC
}
#endif

/****************************************************************************************/
/*	lzw encoder	*/
/****************************************************************************************/
#ifdef LZW
{
#endif

#define LA_SIZ	8
#define WIN_SIZ	256

#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#define MIN(x,y) (((x) < (y)) ? (x) : (y))

void get_longest_match (uint8_t *in, int bytes, int cp, int *dist, int *len)
{
	int eob = MIN((cp + LA_SIZ), (bytes + 1));
	int i, l, a, m, p;
	int reps, last;
	uint8_t subs[WIN_SIZ];
	uint8_t match[WIN_SIZ];
	
	*dist = *len = -1;
	
	for (i = cp + 1; i < eob; i++) {
		a = i - cp;
		l = MAX((cp - WIN_SIZ), 0);
		
		memset(subs, 0, sizeof(subs));
		memcpy(subs, in + cp, a);

		for (; l < cp; l++) {
			memset(match, 0, sizeof(match));
			p = cp - l;
			
			reps = a / p;
			last = a % p;

			for (m = 0; m < reps; m++)
				memcpy(match + (m * p), in + l, p);

			if (last)
				memcpy(match + (m * p), in + l, last);

			if (!strcmp((char *)match, (char *)subs) && a > *len) {
				*dist = p;
				*len = a;
			}
		}
	}
}

struct lz_out {
	uint16_t len;
	uint16_t dist;
};

int lz77 (uint8_t *in, int bytes, struct lz_out **out, int win_size)
{
	int i = 0, length, pos;
	int num_dc = 0, num = 0;

	*out = calloc(sizeof(struct lz_out), bytes);

	while (i < bytes) {
		get_longest_match(in, bytes, i, &pos, &length);
		
		if (length > 2) {
			(*out)[num].len = length;
			(*out)[num].dist = pos;
			//printf("%d (%d): pos %d len %d\n", i, in[i], pos, length);

			i += length + 1;
			num++, num_dc++;
		} else {
			(*out)[num].len = in[i];
			(*out)[num].dist = 0;
			//printf("%d (%d): lit %d\n", i, in[i], in[i]);

			num++, i++;
		}
	}
	
	fprintf(stderr, "[%s] total chars: %d, num LZ77: %d\n", __func__, num, num_dc);
	
	return num;
}

void test_lz77 (void)
{
	struct lz_out *bs;
	//uint8_t out[256] = { 0 };
	uint8_t *in = (uint8_t *)"aacaacabcabaaac";//"cabracadabrarrarrad";
	
	printf("[%s]: testing lzw\n", __func__);
	printf("expected, output:\n<0, 97>, <1, 1, a>, <3, 4, c>, <3, 3, a>, <12, 3, />\n");
	
	printf("%d\n", lz77(in, strlen((char *)in), &bs, 1));
	
	free(bs);
	
	printf("\n");
}

#ifdef LZW
}
#endif

/****************************************************************************************/
/*	repetition coder	*/
/****************************************************************************************/
#ifdef HUFF_RCODER
{
#endif

/*
   0 - 15: Represent code lengths of 0 - 15
       16: Copy the previous code length 3 - 6 times.
           The next 2 bits indicate repeat length
                 (0 = 3, ... , 3 = 6)
              Example:  Codes 8, 16 (+2 bits 11),
                        16 (+2 bits 10) will expand to
                        12 code lengths of 8 (1 + 6 + 5)
       17: Repeat a code length of 0 for 3 - 10 times.
           (3 bits of length)
       18: Repeat a code length of 0 for 11 - 138 times
           (7 bits of length)
*/

struct rep_out {
	uint16_t len_code;
	uint16_t lc_len;
	uint16_t num_exbits;
	uint16_t ext_val;
};

void add_rep_data (struct rep_out *out, int lc, int lcl, int ev, int evb)
{
	out->len_code = lc;
	out->lc_len = lcl;
				
	out->ext_val = ev;
	out->num_exbits = evb;
}

int code_rep (int code_len, int num, struct rep_out *out, int ind)
{
	int n;
	int start = ind;
	
	if (code_len < 0 || code_len > 15 || num < 0) {
		printf("cl = %d, num = %d, ind = %d\n", code_len, num, ind);
		return 0;
	}
		
	add_rep_data(&out[ind++], code_len, 4, 0, 0);
	--num;
		
	if (code_len > 0) {
		for (n = 6; n >= 3; n--) {
			while (num >= n) {
				add_rep_data(&out[ind++], 16, 5, n - 3, 2);
				num -= n;
			}
		}
	} else {
		for (n = 138; n >= 11; n--) {
			while (num >= n) {
				add_rep_data(&out[ind++], 18, 5, n - 11, 7);
				num -= n;
			}
		}
		
		for (n = 10; n >= 3; n--) {
			while (num >= n) {
				add_rep_data(&out[ind++], 17, 5, n - 3, 3);
				num -= n;
			}
		}
	}
	
	while (num--) {
		add_rep_data(&out[ind++], code_len, 4, 0, 0);
	}
	
	//printf("%d\n", ind - start);
		
	return ind - start;
}

int write_rle_codes (uint16_t *code_lens, int num, struct rep_out *out)
{
	int i, j, rl = 0;
	int count = 0;
	
	for (i = 0; i < num; ) {
		for (rl = 0, j = i; j < num; j++) {
			if (code_lens[i] != code_lens[j])
				break;
			rl++;
		}
		//fprintf(stderr, "[%s]: (%d, %d), count=%d, writing run of %d '%d'\n", 
		//		 __func__, i, j, count, rl, code_lens[i]);
		
		count += code_rep(code_lens[i], rl, out, count);
		
		i = j;
	}
	
	return count;
}

void test_repcode (void)
{
	struct rep_out *out = calloc(sizeof(struct rep_out), 256);
	
	uint16_t code_lens[40] = {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
			     0, 0, 0, 0, 0, 6, 3, 1, 6, 4, 
			     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 
			     5, 5, 5, 5, 5, 5, 5, 2, 2, 2};
			     
	int i, n = write_rle_codes(code_lens, 40, out);
	
	n += code_rep(1, 2, out, n);
	n += code_rep(4, 19, out, n);
	n += code_rep(0, 835, out, n);

	for (i = 0; i < n; i++)
		printf("%d: %d %d %d %d\n", i, out[i].len_code, 
			out[i].lc_len, out[i].num_exbits, out[i].ext_val);
	
	free(out);
}

#ifdef HUFF_CODER
}
#endif

/****************************************************************************************/
/*	gzip writer	*/
/****************************************************************************************/

#ifdef GZIP_WRITER
{
#endif 

/* returns number of unique lengths */
int copy_huff_lens (struct huff_list *dlist, int num, uint16_t *dst)
{
	int i, count = 0; 
	int *vcl = calloc(sizeof(int), num);
	
	for (i = 0; i < num; i++) {
		//if (dlist[i].count == 0) {
		//	dlist[i].hc_len = 0;
		//}
		//printf("%d: code %d len %d freq %d\n", i, dlist[i].val, 
		//	dlist[i].hc_len, dlist[i].count);
			
		dst[i] = dlist[i].hc_len;
		
		if (vcl[dst[i]]++ == 0)
			++count;
	}
	
	free(vcl);
	
	return count;
}

void map_huff_array (struct huff_list *src, struct huff_list *dst, int num)
{
	int i;
	
	for (i = 0; i < num; i++)
		memcpy(&dst[src[i].val], &src[i], sizeof(struct huff_list));
}

void reverse_write (struct bit_stream *bs, uint64_t in, int bits)
{
	write_bit_stream(bs, reverse_bits(in, bits), bits);
}

void write_compressed_data (struct bit_stream *bs, struct lz_out *lz, int num, 
		struct huff_list *hlist, struct huff_list *dlist, 
		struct huff_len_table hlt[288])
{
	int i, code, ev, eb;
	struct huff_len_table *hp;
	
	for (i = 0; i < num; i++) {
		printf("%d: len = %3d, dist = %3d\n", i, lz[i].len, lz[i].dist);
		if (lz[i].dist) {
			hp = &hlt[lz[i].len];
			reverse_write(bs, hlist[hp->len_code].hcode,
					     hlist[hp->len_code].hc_len);
			if (hp->num_exbits)
				reverse_write(bs, hp->ext_val, hp->num_exbits);
			
			printf("\twriting lc %3d -> %3d (%2d bits) + ext %3d (%2d bits)\n", 
				hp->len_code, hlist[hp->len_code].hcode, 
				hlist[hp->len_code].hc_len, hp->ext_val, hp->num_exbits);
			
			dist_code(lz[i].dist, &code, &eb, &ev);
			reverse_write(bs, dlist[code].hcode, 
					     dlist[code].hc_len);
			if (eb)
				reverse_write(bs, ev, eb);
				
			printf("\twriting dc %3d -> %3d (%2d bits) + ext %3d (%2d bits)\n",
				code, dlist[code].hcode, dlist[code].hc_len, ev, eb);
		} else {
			reverse_write(bs, hlist[lz[i].len].hcode, 
					     hlist[lz[i].len].hc_len);
			
			printf("\twriting literal %3d (%2d bits)\n", 
				hlist[lz[i].len].hcode, hlist[lz[i].len].hc_len);
		}
	}
		
	reverse_write(bs, hlist[256].hcode, hlist[256].hc_len);
}

int max_code (struct huff_list *hl, int num)
{
	int i, c = 0;
	
	for (i = 0; i < num; i++)
		if (hl[i].count && hl[i].val > hl[c].val)
			c = i;

	printf("[%s]: max = %d at ind %d out of %d\n", __func__, hl[c].val, c, num);

	return c;
}

int write_rle_trees (struct bit_stream *bs, uint16_t *lens, uint16_t *dist, 
		struct huff_list **plist, int num_len, int num_dst, int last)
{
	int count = 0;
	int order[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
	struct rep_out *rt = calloc(288 * 2, sizeof(struct rep_out));
	uint16_t *tmp = calloc(288 * 2, sizeof(uint16_t));
	int i, j, h, nrt, maxbl = 0;
	struct huff_list *hlist, *hp; 

	for (i = 0, j = 0; i < num_len; i++)
		tmp[j++] = lens[i];
	
	for (i = 0; i < num_dst; i++)
		tmp[j++] = dist[i];

	nrt = write_rle_codes(tmp, num_dst + num_len, rt);
	
	for (i = 0; i < nrt; i++) {
		tmp[i] = rt[i].len_code;
		//printf("tmp[%d] = %d\n", i, tmp[i]);
	}

	h = huff_encode(tmp, nrt, 18, plist);
	
	hlist = calloc(sizeof(struct huff_list), 19);
	map_huff_array(*plist, hlist, h);
	
	print_huffcodes(hlist, 0, 19);
	
	for (maxbl = 19 - 1; maxbl >= 3; maxbl--)
		if (hlist[order[maxbl]].hc_len != 0)
			break;

	printf("[%s]: sending huffman trees:\n"
		"\tlength codes:      %d\n"
		"\tdist codes:        %d\n"
		"\tcode length codes: %d\n"
		"\trle length codes:  %d\n"
		"\tencoded rle codes: %d\n", 
		__func__, num_len, num_dst, maxbl, nrt, h);

	write_bit_stream(bs, last + (2 << 1), 3);
	write_bit_stream(bs, num_len - 257, 5);
	write_bit_stream(bs, num_dst - 1, 5);
	write_bit_stream(bs, maxbl + 1 - 4, 4);
	
	printf("[%s]: writing %d huffman code lengths: \n", __func__, maxbl + 1);
	
	for (i = 0; i < maxbl + 1; i++) {
		if (!hlist[order[i]].count)
			hlist[order[i]].hc_len = 0;
			
		write_bit_stream(bs, hlist[order[i]].hc_len, 3); 
		printf("\t%2d: %2d: %2d bits (freq %3d)\n", i, order[i], 
			hlist[order[i]].hc_len, hlist[order[i]].count);
		
		if (hlist[order[i]].hc_len)
			++count;
	}
	
	for (i = 0; i < nrt; i++) {
		hp = &hlist[rt[i].len_code];
		//if (!hlist[rt[i].len_code].hc_len)
		//	write_bit_stream(bs, 0, 1);
		//else
			write_bit_stream(bs, hp->hcode, hp->hc_len);
				 	
		if (rt[i].ext_val) 
			write_bit_stream(bs, rt[	i].ext_val, rt[i].num_exbits);
		/*	
		printf("%3d: code %3d for %3d (%3d bits) + extra %3d (%1d bits) count %3d\n", 
			i, hlist[rt[i].len_code].hcode, rt[i].len_code,
			hlist[rt[i].len_code].hc_len, rt[i].ext_val, 
			rt[i].num_exbits, hlist[rt[i].len_code].count);*/
	}
	
	free(hlist);
	
	free(rt);
	free(tmp);
	
	return count;	
}

#if 0
int write_gzdata_unc (unsigned char *src, int len, int fd, int blk_size)
{
	int pos = 0, toc, i, j, num, num_dist, num_lenc, max_dist, max_lenc;
	struct huff_len_table hlt[288];
	struct bit_stream *bs;
	struct huff_list *hlist, *dlist, *rlist;
	struct huff_list *hhlist, *ddlist;
	struct lz_out *lz;
	int code, eb, ev;
	uint16_t *lens, *dist;
	int *bl_count;
	
	make_hufftable(hlt);
	
	alloc_bit_stream(&bs, len * 2);
	
	make_static_hlist (&hhlist, &ddlist, &bl_count);
	
	while (pos < len) {
		if ((toc = (len - pos)) > blk_size)
			toc = blk_size;
		
		num = lz77(src + pos, toc, &lz, 512);

		lens = calloc(sizeof(uint16_t), toc + 288);
		dist = calloc(sizeof(uint16_t), toc + 288);
		
		num_dist = num_lenc = 0;

		for (i = 0, j = 0; i < num; i++) {
			if (lz[i].dist) { 
				dist_code(lz[i].dist, &code, &eb, &ev);
				dist[i] = code;
				lens[i] = hlt[lz[i].len].len_code;
				//printf("\t%3d: dist %d (code %d), len %d (code %d)\n", 
				//	i, lz[i].dist, code, lz[i].len, lens[i]);
			} else {
				lens[i] = lz[i].len;
				//printf("\t%3d: literal %d\n", i, lens[i]);
			}
		}
		
		num_dist = huff_encode (dist, num, 32, &dlist);
		num_lenc = huff_encode (lens, num, 288, &hlist); 
		
		ddlist = calloc(sizeof(struct huff_list), 288);
		hhlist = calloc(sizeof(struct huff_list), 288);
		
		map_huff_array(dlist, ddlist, num_dist);
		map_huff_array(hlist, hhlist, num_lenc);
		
		max_lenc = max_code(hhlist, 288);
		max_dist = max_code(ddlist, 32);
		
		print_huffcodes(hhlist, 0, 288);
		print_huffcodes(ddlist, 0, 31);
		
		printf("[%s]: encoded length/dist trees: \n"
			"\t# length = %d (%d max), %d unique\n"
			"\t# dist = %d (%d max), %d unique\n", __func__,
			num_lenc, max_lenc, copy_huff_lens(hhlist, max_lenc, lens),
			num_dist, max_dist, copy_huff_lens(ddlist, max_dist, dist));
			
		i = write_rle_trees(bs, lens, dist, &rlist, max_lenc + 1,
				max_dist + 1, ((toc != blk_size) ? 1 : 0));

		printf("wrote %d length codes\n", i);

		write_compressed_data(bs, lz, num, hhlist, ddlist, hlt);	
		
		printf("%d bytes to %d\n", toc, bs->byte_pos);

		free(lz);
		free(hlist);
		free(dlist);
		free(rlist);
		free(lens);
		free(dist);
		
		free(hhlist);
		free(ddlist);	
		
		pos += toc;
	}

	free(bl_count);
	//free(hhlist);
	//free(ddlist);		
	
	if (write(fd, bs->buf, bs->byte_pos) != bs->byte_pos)
		return -1;	
		
	printf("%d bytes to %d\n", len, bs->byte_pos);
	
	free_bit_stream(bs);
	
	
	return 0;
}
#endif

#if 1
int write_gzdata_unc (unsigned char *src, int len, int fd, int blk_size)
{ /* static huffman codes */
	int pos = 0, toc, num;
	struct huff_len_table hlt[288];
	struct bit_stream *bs;
	struct huff_list *hhlist, *ddlist;
	struct lz_out *lz;
	int *bl_count;
	
	make_hufftable(hlt);
	
	alloc_bit_stream(&bs, len * 2);
	
	make_static_hlist (&hhlist, &ddlist, &bl_count);

	while (pos < len) {
		if ((toc = (len - pos)) > blk_size)
			toc = blk_size;
		
		num = lz77(src + pos, toc, &lz, 512);

		write_bit_stream(bs, ((toc != blk_size) ? 1 : 0) + (1 << 1), 3);

		write_compressed_data(bs, lz, num, hhlist, ddlist, hlt);	
		
		printf("%d bytes to %d\n", toc, bs->byte_pos);

		free(lz);
		
		pos += toc;
	}

	free(bl_count);
	free(hhlist);
	free(ddlist);
	
	if (write(fd, bs->buf, bs->byte_pos) != bs->byte_pos)
		return -1;	
		
	printf("%d bytes to %d\n", len, bs->byte_pos);
	
	free_bit_stream(bs);
	
	return 0;
}
#else

int write_gzdata_unc (unsigned char *src, int len, int fd, int blk_size)
{
	int ret, pos = 0, toc;
	uint16_t lval;
	
	while (pos < len) {
		if ((toc = (len - pos)) > blk_size)
			toc = blk_size;
		
		lval = ((toc != blk_size) ? 1 : 0);
		if ((ret = write(fd, (uint8_t *)&lval, sizeof(uint8_t))) != sizeof(uint8_t))
			return -1;
		
		lval = (uint16_t)toc;
		if ((ret = write(fd, &lval, sizeof(uint16_t))) != sizeof(uint16_t))
			return -1;	
			
		lval = ~((uint16_t)toc);
		if ((ret = write(fd, &lval, sizeof(uint16_t))) != sizeof(uint16_t))
			return -1;				
			
		if ((ret = write(fd, src + pos, toc)) != toc)
			return -1;
			
		printf("wrote %d/%d bytes, pos %d/%d\n", toc, ret, pos, len); 
		
		pos += toc;
	}
	
	return 0;
}

#endif

int write_gzfile (const char *src, const char *dst)
{
	struct stat st;
	int len, src_fd;
	unsigned char *buf;
	
	struct gz_header {
		uint8_t id0;
		uint8_t id1;
		uint8_t comp_method;
		uint8_t flags;
		uint32_t mtime;
		uint8_t extra_flags;
		uint8_t os;
	} head;

	struct gz_trailer {
		uint32_t crc32;
		uint32_t isize;
	} gtl;
	
	if (stat(src, &st) < 0 || S_ISDIR(st.st_mode)) {
		fprintf(stderr, "[%s]: stat failed\n", __func__);
		return -1;
	}
	
	memset(&head, 0, sizeof(head));
	
	head.id0 = 0x1f;
	head.id1 = 0x8b;
	
	head.comp_method = 8;
	head.mtime = st.st_mtime;
	head.os = 0xFF;
	
	if ((len = read_file(src, &buf)) < 0)
		return -1;
	
	gtl.isize = len;
	gtl.crc32 = crc(buf, len);

	if ((src_fd = open(dst, O_CREAT|O_TRUNC|O_RDWR, 0777)) < 0) {
		free(buf);
		return -1;
	}
	
	if (write(src_fd, &head, 10) != 10 ||
	    write_gzdata_unc(buf, len, src_fd, 65534) < 0 ||
	    write(src_fd, &gtl, sizeof(gtl)) != sizeof(gtl)) {
		close(src_fd);
		free(buf);
		return -1;
	}
	
	fprintf(stderr, "[%s]: wrote %d bytes to %s\n", __func__, len, dst);

	close(src_fd);
	free(buf); 
	 
	return 0;
}

#ifdef TEST 
}
#endif


#ifdef HUFF_LCODER
}
#endif

int main (void)
{
	test_repcode();
	
	encode_hlist (NULL, 0);

	test_hcoder();
 
	test_bit_stream();
	
	test_lz77();
	
	uint64_t test = 1243243452;
	uint64_t out = reverse_bits(test, 31);
	
	print_bits(test, 31);
	print_bits(out, 31);

	make_tar_file("/Users/nobody1/Desktop/work", 
				"/Users/nobody1/Desktop/test.tar");
				
	return write_gzfile("/Users/nobody1/Desktop/test.tar",
				"/Users/nobody1/Desktop/test1.gz");
}

