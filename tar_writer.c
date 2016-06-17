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

int write_file_entry (char *wpath, const char *d_name, struct dirent *ds, struct stat st, int tfd)
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

int main (void)
{
	return make_tar_file("/Users/x/Desktop/work", 
				"/Users/x/Desktop/test.tar");
}


