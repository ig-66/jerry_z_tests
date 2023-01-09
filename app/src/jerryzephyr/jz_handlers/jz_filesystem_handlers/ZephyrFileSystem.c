#include "ZephyrFileSystem.h"

#include <zephyr/zephyr.h>
#include <zephyr/fs/fs.h>
#include <stdio.h>
#include <string.h>

#pragma message "--- The following warning message is expected, please, ignore it ---"

#define FS_DATA_SIZE 1024

/*  Create and/or write a file with data
 *
 *   Returns > 0 - on success
 *   Returns < 0 - on error (error code)
 */
int zephyr_storage_write_file(char *pfile_name, char *pfile_data);

/*  Read data in a file
 *
 *   Returns > 0 - on success
 *   Returns < 0 - on error (error code)
 */
int zephyr_storage_read_file(char *pfile_name, char *pfile_data);

jerry_value_t
writeFile_handler(const jerry_call_info_t *call_info_p,
				  const jerry_value_t arguments[],
				  const jerry_length_t arguments_count)
{	
	if(arguments_count != 3){
		char error_message[] = "ERROR wrong number of arguments";
		jerry_value_t error_string = jerry_string(error_message, strlen(error_message), JERRY_ENCODING_UTF8);
		jerry_value_t arg[] = {error_string};

		jerry_value_t ret = jerry_call(arguments[2], jerry_undefined(), arg, 1);

		jerry_value_free(ret);
		jerry_value_free(error_string);

		return jerry_undefined();
	}

	jerry_value_t key = jerry_value_to_string(arguments[0]);
	uint8_t key_buffer[256];

	jerry_size_t copied_bytes = jerry_string_to_buffer(key, JERRY_ENCODING_UTF8, key_buffer, sizeof(key_buffer) - 1);
	key_buffer[copied_bytes] = '\0';

	jerry_value_free(key);

	jerry_value_t data = jerry_value_to_string(arguments[1]);
	uint8_t data_buffer[FS_DATA_SIZE];

	jerry_size_t copied_bytes1 = jerry_string_to_buffer(data, JERRY_ENCODING_UTF8, data_buffer, sizeof(data_buffer) - 1);
	data_buffer[copied_bytes1] = '\0';

	jerry_value_free(data);
	int res = zephyr_storage_write_file(key_buffer, data_buffer);

	jerry_value_t arg[] = {0};
	jerry_value_t error_string = 0;
	if (res < 0) {
		printk("-- ERROR writting the file\n");
		char error_message[32];
		sprintf(error_message, "ERROR reading file: %d", res);
		error_string = jerry_string(error_message, strlen(error_message), JERRY_ENCODING_UTF8);
		arg[0] = error_string;
	}
	jerry_value_t ret = jerry_call(arguments[2], jerry_undefined(), arg, 1);

	jerry_value_free(ret);
	jerry_value_free(error_string);

	return jerry_undefined();
}

jerry_value_t
readFile_handler(const jerry_call_info_t *call_info_p,
				 const jerry_value_t arguments[],
				 const jerry_length_t arguments_count)
{	
	if (arguments_count != 2) {
		char error_message[] = "ERROR wrong number of arguments";
		jerry_value_t error_string = jerry_string(error_message, strlen(error_message), JERRY_ENCODING_UTF8);
		jerry_value_t arg[] = {error_string};

		jerry_value_t ret = jerry_call(arguments[2], jerry_undefined(), arg, 1);

		jerry_value_free(ret);
		jerry_value_free(error_string);

		return jerry_undefined();
	}

	jerry_value_t key = jerry_value_to_string(arguments[0]);
	uint8_t key_buffer[256];

	jerry_size_t copied_bytes = jerry_string_to_buffer(key, JERRY_ENCODING_UTF8, key_buffer, sizeof(key_buffer) - 1);
	key_buffer[copied_bytes] = '\0';

	jerry_value_free(key);

	char data[FS_DATA_SIZE];
	jerry_value_t args[] = {0, 0};
	jerry_value_t data_string;
	jerry_value_t error_string = 0;

	int res = zephyr_storage_read_file(key_buffer, data);

	if(res < 0){
		printk("-- ERROR reading the file\n");
		char error_message[32];
		sprintf(error_message, "ERROR reading file: %d", res);
		error_string = jerry_string(error_message, strlen(error_message), JERRY_ENCODING_UTF8);
		args[1] = error_string;
	} else {
		data_string = jerry_string(data, strlen(data), JERRY_ENCODING_UTF8);
		args[0] = data_string;
	}

	jerry_value_t ret = jerry_call(arguments[1], jerry_undefined(), args, 2);

	jerry_value_free(ret);
	jerry_value_free(data_string);
	jerry_value_free(error_string);

	return jerry_undefined();
}

#if CONFIG_DISK_DRIVER_FLASH
#include <zephyr/storage/flash_map.h>
#endif

#if CONFIG_FAT_FILESYSTEM_ELM
#include <ff.h>
#endif

#if CONFIG_FILE_SYSTEM_LITTLEFS
#include <zephyr/fs/littlefs.h>
FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(storage);
#endif

static struct fs_mount_t fs_mnt;

int zephyr_storage_write_file(char *pfile_name, char *pfile_data)
{
	int res = 0;
	struct fs_file_t testfile;
	char file_name[38];

	sprintf(file_name, "%s", pfile_name);

	fs_file_t_init(&testfile);

	res = fs_open(&testfile, file_name, FS_O_CREATE | FS_O_RDWR);

	if (res < 0) {
		printk("-- ERROR: while openning the file: %s\n", file_name);
		return res;
	}
	res = fs_write(&testfile, pfile_data, strlen(pfile_data) + 1);

	if (res < 0) {
		printk("-- ERROR: Error while creating or writting file: %s\n", file_name);
	}

	fs_close(&testfile);

	return res;
}

int zephyr_storage_read_file(char *pfile_name, char *pfile_data)
{	
	int res = 0;
	struct fs_file_t testfile;
	char file_name[38];

	sprintf(file_name, "%s", pfile_name);

	fs_file_t_init(&testfile);

	res = fs_open(&testfile, file_name, FS_O_READ);

	if(res < 0){
		printk("-- ERROR: while openning the file: %s\n", file_name);
		return res;
	}

	res = fs_read(&testfile, pfile_data, FS_DATA_SIZE);

	if(res < 0) {
		printk("-- ERROR: while reading the file: %s\n", file_name);
	}

	fs_close(&testfile);

	return res;
}

static int setup_flash(struct fs_mount_t *mnt)
{
	int rc = 0;
#if CONFIG_DISK_DRIVER_FLASH
	unsigned int id;
	const struct flash_area *pfa;

	mnt->storage_dev = (void *)FLASH_AREA_ID(storage);
	id = (uintptr_t)mnt->storage_dev;

	rc = flash_area_open(id, &pfa);
	printk("Area %u at 0x%x on %s for %u bytes\n",
		   id, (unsigned int)pfa->fa_off, pfa->fa_dev_name,
		   (unsigned int)pfa->fa_size);

	if (rc < 0 && IS_ENABLED(CONFIG_APP_WIPE_STORAGE))
	{
		printk("Erasing flash area ... ");
		rc = flash_area_erase(pfa, 0, pfa->fa_size);
		printk("%d\n", rc);
	}

	if (rc < 0)
	{
		flash_area_close(pfa);
	}
#endif
	return rc;
}

static int mount_app_fs(struct fs_mount_t *mnt)
{
	int rc;

#if CONFIG_FAT_FILESYSTEM_ELM
	static FATFS fat_fs;

	mnt->type = FS_FATFS;
	mnt->fs_data = &fat_fs;
	if (IS_ENABLED(CONFIG_DISK_DRIVER_RAM))
	{
		mnt->mnt_point = "/RAM:";
	}
	else if (IS_ENABLED(CONFIG_DISK_DRIVER_SDMMC))
	{
		mnt->mnt_point = "/SD:";
	}
	else
	{
		mnt->mnt_point = "/NAND:";
	}

#elif CONFIG_FILE_SYSTEM_LITTLEFS
	mnt->type = FS_LITTLEFS;
	mnt->mnt_point = "/lfs";
	mnt->fs_data = &storage;
#endif
	rc = fs_mount(mnt);

	return rc;
}

static void setup_disk(void)
{
	struct fs_mount_t *mp = &fs_mnt;
	struct fs_dir_t dir;
	struct fs_statvfs sbuf;
	int rc;

	fs_dir_t_init(&dir);

	if (IS_ENABLED(CONFIG_DISK_DRIVER_FLASH))
	{
		rc = setup_flash(mp);
		if (rc < 0)
		{
			return;
		}
	}

	if (!IS_ENABLED(CONFIG_FILE_SYSTEM_LITTLEFS) &&
		!IS_ENABLED(CONFIG_FAT_FILESYSTEM_ELM))
	{
		return;
	}

	rc = mount_app_fs(mp);
	if (rc < 0)
	{
		return;
	}

	k_sleep(K_MSEC(50));

	rc = fs_statvfs(mp->mnt_point, &sbuf);
	if (rc < 0)
	{
		printk("FAIL: statvfs: %d\n", rc);
		return;
	}

	printk("%s: bsize = %lu ; frsize = %lu ;"
		   " blocks = %lu ; bfree = %lu\n",
		   mp->mnt_point,
		   sbuf.f_bsize, sbuf.f_frsize,
		   sbuf.f_blocks, sbuf.f_bfree);

	rc = fs_opendir(&dir, mp->mnt_point);
	printk("%s opendir: %d\n", mp->mnt_point, rc);

	if (rc < 0)
	{
	}

	while (rc >= 0)
	{
		struct fs_dirent ent = {0};

		rc = fs_readdir(&dir, &ent);
		if (rc < 0)
		{
			break;
		}
		if (ent.name[0] == 0)
		{
			printk("End of files\n");
			break;
		}
		printk("  %c %u %s\n",
			   (ent.type == FS_DIR_ENTRY_FILE) ? 'F' : 'D',
			   ent.size,
			   ent.name);
	}

	(void)fs_closedir(&dir);

	return;
}

int zephyr_storage_init(void)
{
	setup_disk();

	return 0;
}
