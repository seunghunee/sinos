#include <stdio.h>

#define BYTES_OF_SECTOR 512
// MBR entry location
#define DISK_SIGNATURE 0x1B8
#define FIRST_PARTITION_FIRST_CHS 0x1BF
#define FIRST_PARTITION_TYPE 0x1C2
#define FIRST_PARTITION_LAST_CHS 0x1C3
#define FIRST_PARTITION_FIRST_LBA 0x1C6
#define FIRST_PARTITION_NUM_SECTORS 0x1CA

int fileCopy(FILE* dest, FILE* src);
//복사된 바이트량을 리턴
int adjustInSectorSize(int bytes_file, FILE* file);
//추가된 바이트량을 리턴
void writeSinosSize(int bytes_32kernel, int bytes_64kernel, FILE* file);
//부트로더에 각 커널의 섹터수와 총 섹터수를 쓴다.
void writeSinosDiskSignature(FILE* file)
{
  int value;

  /* fseek(file, DISK_SIGNATURE, SEEK_SET); */
  /* value = 0xba75d; */
  /* fwrite(&value, 4, 1, file); */
  
  /* fseek(file, FIRST_PARTITION_FIRST_CHS, SEEK_SET); */
  /* value = 0x0101; */
  /* fwrite(&value, 3, 1, file); */

  fseek(file, FIRST_PARTITION_TYPE, SEEK_SET);
  value = 0x0c;
  fwrite(&value, 1, 1, file);

  /* fseek(file, FIRST_PARTITION_LAST_CHS, SEEK_SET); */
  /* value = 0xfdfe77; */
  /* fwrite(&value, 3, 1, file); */

  /* fseek(file, FIRST_PARTITION_FIRST_LBA, SEEK_SET); */
  /* value = 0x3e; */
  /* fwrite(&value, 4, 1, file); */

  /* fseek(file, FIRST_PARTITION_NUM_SECTORS, SEEK_SET); */
  /* value = 0x7405a2; */
  /* fwrite(&value, 4, 1, file); */
}
//부트로더에 disk signature를 기입한다.

int main(int argc, char *argv[])
{
  FILE* fp_sinos = fopen("Sinos.iso", "w");

  FILE* fp_boot_loader = fopen(argv[1], "r");
  fileCopy(fp_sinos, fp_boot_loader);
  fclose(fp_boot_loader);

  FILE* fp_32kernel = fopen(argv[2], "r");
  int bytes_32kernel = 0;
  bytes_32kernel += fileCopy(fp_sinos, fp_32kernel);
  fclose(fp_32kernel);
  bytes_32kernel += adjustInSectorSize(bytes_32kernel, fp_sinos);

  FILE* fp_64kernel = fopen(argv[3], "r");
  int bytes_64kernel = 0;
  bytes_64kernel += fileCopy(fp_sinos, fp_64kernel);
  fclose(fp_64kernel);
  bytes_64kernel += adjustInSectorSize(bytes_64kernel, fp_sinos);

  writeSinosSize(bytes_32kernel, bytes_64kernel, fp_sinos);
  writeSinosDiskSignature(fp_sinos);
  fclose(fp_sinos);

  return 0;
}

int fileCopy(FILE* dest, FILE* src)
{
  char buf[BYTES_OF_SECTOR];
  int read_bytes, copied_bytes = 0;

  while (!feof(src)) {
    read_bytes = fread(buf, 1, BYTES_OF_SECTOR, src);
    copied_bytes += fwrite(buf, 1, read_bytes, dest);
  }

  return copied_bytes;
}

int adjustInSectorSize(int bytes_file, FILE* file)
{
  int needed_bytes = BYTES_OF_SECTOR - bytes_file % BYTES_OF_SECTOR;
  const char zero = 0;

  int added_bytes = 0;
  for (int i = 0; i < needed_bytes; ++i)
    added_bytes += fwrite(&zero, 1, 1, file);

  return added_bytes;
}

void writeSinosSize(int bytes_32kernel, int bytes_64kernel, FILE* file)
{
  unsigned short sectors_32kernel = bytes_32kernel / BYTES_OF_SECTOR;
  fseek(file, 0xE2, SEEK_SET);
  fwrite(&sectors_32kernel, 2, 1, file);
  unsigned short sectors_64kernel = bytes_64kernel / BYTES_OF_SECTOR;
  fseek(file, 0xE4, SEEK_SET);
  fwrite(&sectors_64kernel, 2, 1, file);
  unsigned short sectors_sinos = sectors_32kernel + sectors_64kernel;
  //부트로더의 0xE4, 0xE5가 읽을 섹터 수를 나타낸다. (little endian!)
  fseek(file, 0xE8, SEEK_SET);
  fwrite(&sectors_sinos, 2, 1, file);
}
