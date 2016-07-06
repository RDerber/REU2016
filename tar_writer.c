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
/*	bit stream	*/
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
	
	printf("wrote %d bits to stream (now at byte %d bit %d)\n", 
		bits, bs->byte_pos, bs->bit_pos);

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

void make_huffman_tree (struct mh_node **tree, struct huff_list *hlist, int num, int unique)
{
	struct mh_node *left, *right, *top;
	int i;
	struct min_heap mh;
	
	memset(&mh, 0, sizeof(struct min_heap));
   
	mh.size = unique;
	mh.array = calloc(num, sizeof(struct mh_node *));
	
	for (i = 0; i < num; ++i)
		mh.array[i] = new_node(hlist[i].val, hlist[i].count);

	for (i = (mh.size - 2) / 2; i >= 0; --i)
		min_heapify(&mh, i);

	while (mh.size != 1) {
		left = extract_min(&mh);
		right = extract_min(&mh);

		top = new_node(257, left->freq + right->freq);
		top->left = left;
		top->right = right;
		mh_insert(&mh, top);
	}
	
	*tree = extract_min(&mh);

	free(mh.array);
}

void find_huffcode (struct huff_list hl[256], struct mh_node *base, int buf, int pos)
{
	if (base->left) {
		buf &= ~(1 << pos);
		find_huffcode(hl, base->left, buf, pos + 1);
	} 
	 
	if (base->right) { 
		buf |= (1 << pos);
		find_huffcode(hl, base->right, buf, pos + 1);
	}
	
	if (base->data < 256 && (!(base->left) && !(base->right))) {
		printf("%d: ", base->data);	
		
		int j;
		for (j = 0; j < pos; j++) {
			if (buf & (1 << j))
				putchar('1');
			else
				putchar('0');
		}
		
		putchar('\n');
		
		for (j = 0; j < 256; j++) {
			if (hl[j].val == base->data) {
				hl[j].hcode = buf;
				hl[j].hc_len = pos;
				break;
			}
		}
	}
	
	//fprintf(stderr, "[%s]: node %p freed\n", __func__, base);
	free(base);
}

void print_huffcodes (struct huff_list *hlist, int num)
{
	int i, j;
	
	for (i = 0; i < num; i++) {
		printf("#%d, val = %d, code =(%d bits): %d | ", i, 
			hlist[i].val, hlist[i].hc_len, hlist[i].hcode);

			for (j = 0; j < hlist[i].hc_len; j++) {
				if (hlist[i].hcode & (1 << j))
					putchar('1');
				else
					putchar('0');
			}
		
		putchar('\n');
	}
}

void huff_encode (uint8_t *src, int num, struct huff_list **hlist)
{
	int count[256] = { 0 };
	struct mh_node *htree = NULL;
	int unique = 0, i;
	
	*hlist = calloc(256, sizeof(struct huff_list));
	
	for (i = 0; i < num; i++)
		count[src[i]]++;
		
	for (i = 0; i < 256; i++) {
		(*hlist)[i].val = i;
		(*hlist)[i].count = count[i];
		
		if (count[i])
			++unique;
	}
	
	make_huffman_tree(&htree, *hlist, num, unique);
	
	find_huffcode(*hlist, htree, 0, 0);
}

void test_hcoder (void)
{
	uint8_t test[17] = {1, 2, 1, 2, 3, 4, 4, 4, 5, 6, 7, 7, 7, 7, 7, 7, 10};
	struct huff_list *hlist;
	
	printf("[%s]: testing huffman coder\n", __func__);
	
	huff_encode(test, 17, &hlist);
	
	print_huffcodes(hlist, 17);
	
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
void lz77 (uint8_t *in, int bytes, uint8_t *out, int win_size)
{
	int i = 0, length, pos;

	while (i < bytes) {
		get_longest_match(in, bytes, i, &pos, &length);
		
		if (length > 0) {
			if ((i + length + 1) > bytes) {
				printf("<%d, %d, />, ", pos, length);
			}else{
				printf("<%d, %d, %c>, ", pos, length, 
					*(in + i + length + 1));
			}
			i += length + 1;
		} else {
			printf("<0, %d>, ", in[i]);
			i++;
		}
	}
	
	printf("\n");
}

void test_lz77 (void)
{
	uint8_t out[256] = { 0 };
	uint8_t *in = (uint8_t *)"aacaacabcabaaac";
	
	printf("[%s]: testing lzw\n", __func__);
	printf("expected, output:\n<0, 97>, <1, 1, a>, <3, 4, c>, <3, 3, a>, <12, 3, />\n");
	
	lz77(in, strlen((char *)in), out, 1);
	
	printf("\n");
}

#ifdef LZW
}
#endif

/****************************************************************************************/
/*	gzip writer	*/
/****************************************************************************************/
#ifdef GZIP_WRITER
{
#endif
struct gz_header {
	uint8_t id0;
	uint8_t id1;
	uint8_t comp_method;
	uint8_t flags;
	uint32_t mtime;
	uint8_t extra_flags;
	uint8_t os;
};

struct gz_trailer {
	uint32_t crc32;
	uint32_t isize;
};

int write_gzdata_blocked (unsigned char *src, int len, int fd, int blk_size)
{
	int ret, pos = 0, toc;
	uint16_t lval;
	
	while (pos < len) {
		if ((toc = (len - pos)) > blk_size)
			toc = blk_size;
			
		/*struct huff_list *hlist;
		huff_encode(src + pos, toc, &hlist);
		
		encode_hlist(hlist, 256);
	
		print_huffcodes(hlist, 256);
	
		free(hlist);*/
		/*{struct bit_stream *bs;
		int i;
		
		alloc_bit_stream(&bs, toc * 2);
		
		write_bit_stream(bs, (toc < blk_size) ? 0 : 1, 1);
		write_bit_stream(bs, 0, 2);
		
		write_bit_stream(bs, (uint16_t)len, 16);
		
		if (write(fd, bs->buf, bs->byte_pos) != bs->byte_pos)
			return -1;
		
		
		//for (i = 0; i < toc; i++) 
		//	write_bit_stream(bs, hlist[src[i]].hcode, hlist[src[i]].hc_len);
		
		
		printf("%d bytes to %d\n", toc, bs->byte_pos);
	
		free_bit_stream(bs);
		}*/
		lval = (uint16_t)len;
		if ((ret = write(fd, &lval, sizeof(uint16_t))) != sizeof(uint16_t))
			return -1;		
			
		if ((ret = write(fd, src + pos, toc)) != toc)
			return -1;
			
		printf("wrote %d/%d bytes, pos %d/%d\n", toc, ret, pos, len); 
		
		pos += toc;
	}
	
	return 0;
}

int write_gzfile (const char *src, const char *dst)
{
	struct gz_header head;
	struct gz_trailer gtl;
	struct stat st;
	int len, src_fd;
	unsigned char *buf;
	
	if (stat(src, &st) < 0 || S_ISDIR(st.st_mode)) {
		fprintf(stderr, "[%s]: stat failed\n", __func__);
		return -1;
	}
	
	head.id0 = 0x1f;
	head.id1 = 0x8b;
	
	head.comp_method = 0;
	head.flags = 0;
	head.mtime = st.st_mtime;
	
	head.extra_flags = 0;
	head.os = 0xFF;
	
	if ((len = read_file(src, &buf)) < 0)
		return -1;
	
	gtl.isize = len;
	gtl.crc32 = crc(buf, len);

	if ((src_fd = open(dst, O_CREAT|O_TRUNC|O_RDWR, 0777)) < 0) {
		free(buf);
		return -1;
	}
	
	if (write(src_fd, &head, sizeof(head)) != sizeof(head) ||
		write_gzdata_blocked(buf, len, src_fd, 1024000) < 0 ||
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

int main (void)
{
	encode_hlist (NULL, 0);

	test_hcoder();
 
	test_bit_stream();
	
	test_lz77();

	//make_tar_file("/Users/nobody1/Desktop/work", 
	//			"/Users/nobody1/Desktop/test.tar");
				
	return write_gzfile("/Users/nobody1/Desktop/test.tar",
				"/Users/nobody1/Desktop/test1.gz");
}

