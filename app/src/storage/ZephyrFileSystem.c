#include "ZephyrFileSystem.h"

#include <zephyr/zephyr.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/fs/fs.h>
#include <stdio.h>

jerry_value_t
fs_init_handler(const jerry_call_info_t *call_info_p,
				const jerry_value_t arguments[],
				const jerry_length_t arguments_count)
{	
	printk("initializing storage\n");
	zephyr_storage_init();
	printk("storage ready\n");

	return jerry_undefined();
}

jerry_value_t
fileWrite_handler(const jerry_call_info_t *call_info_p,
				  const jerry_value_t arguments[],
				  const jerry_length_t arguments_count)
{	
	if(arguments_count != 3){
		char error_message[] = "ERROR wrong number of arguments";
		jerry_value_t error_string = jerry_string(error_message, strlen(error_message), JERRY_ENCODING_UTF8);
		jerry_value_t arg[] = {error_string};

		jerry_value_t ret = jerry_call(arguments[2], jerry_undefined(), arg, 1);

		jerry_value_free(ret);
		jerry_value_free(arg[0]);
		jerry_value_free(error_string);

		return jerry_undefined();
	}


	jerry_value_t key = jerry_value_to_string(arguments[0]);
	jerry_char_t key_buffer[256];

	jerry_size_t copied_bytes = jerry_string_to_buffer(key, JERRY_ENCODING_UTF8, key_buffer, sizeof(key_buffer) - 1);
	key_buffer[copied_bytes] = '\0';

	jerry_value_free(key);

	jerry_value_t data = jerry_value_to_string(arguments[1]);
	jerry_char_t data_buffer[256];

	copied_bytes = jerry_string_to_buffer(data, JERRY_ENCODING_UTF8, data_buffer, sizeof(data_buffer) - 1);
	data_buffer[copied_bytes] = '\0';

	jerry_value_free(data);
	printk("Writing file: name: %s value: %s\n", key_buffer, data_buffer);

	/* debug only - uncomment before deploy: */
	int res = zephyr_storage_write_file(key_buffer, data_buffer);
	/* debug only - remove before deploy: */
	// int res = -1;
	printk("write result: %d\n", res);
	jerry_value_t arg[] = {0};
	if(res < 0){
		arg[0] = jerry_string("true", strlen("true"), 1);
	}
	jerry_value_t ret = jerry_call(arguments[2], jerry_undefined(), arg, 1);

	jerry_value_free(ret);

	return jerry_undefined();
}

jerry_value_t
fileRead_handler(const jerry_call_info_t *call_info_p,
				 const jerry_value_t arguments[],
				 const jerry_length_t arguments_count)
{
	if (arguments_count != 2) {
		char error_message[] = "ERROR wrong number of arguments";
		jerry_value_t error_string = jerry_string(error_message, strlen(error_message), JERRY_ENCODING_UTF8);
		jerry_value_t arg[] = {error_string};

		jerry_value_t ret = jerry_call(arguments[2], jerry_undefined(), arg, 1);

		jerry_value_free(ret);
		jerry_value_free(arg[0]);
		jerry_value_free(error_string);

		return jerry_undefined();
	}

	jerry_value_t key = jerry_value_to_string(arguments[0]);
	jerry_char_t key_buffer[256];

	jerry_size_t copied_bytes = jerry_string_to_buffer(key, JERRY_ENCODING_UTF8, key_buffer, sizeof(key_buffer) - 1);
	key_buffer[copied_bytes] = '\0';

	jerry_value_free(key);

	static char data[] = "data fromm file";
	jerry_value_t args[2] = {0, 0};
	
	/* DEBUG ONLY - UNCOMMENT BEFORE DEPLOY  */
	// int res = zephyr_storage_read_file(key_buffer, data);
	/* DEBUG ONLY - REMOVE BEFORE DEPLOY */
	int res = 1;
	if(res < 0){
		char txt[] = "true";
		args[1] = jerry_string("true", strlen("true"), 1);
		// printk("args[1]: %d ", args[1]);
		args[1] = jerry_string(txt, strlen(txt), 1);
		// printk("args[1]: %d\n", args[1]);
	} else {
		int valid = jerry_validate_string(data, strlen(data), JERRY_ENCODING_UTF8);
		if(valid){
			printk("String is valid\n");
		} else {
			printk("String is not valid\n");
		}
		// jerry_value_t data_string = jerry_string(data, strlen(data), JERRY_ENCODING_UTF8);
		// args[0] = data_string;
		args[0] = data;
		printk("args[0]: %d &data: %d\n", args[0], &data);
		// jerry_value_free(data_string);
		jerry_value_free(valid);
	}
	printk("-- here2\n");
	jerry_value_t ret = jerry_call(arguments[1], jerry_undefined(), args, 2);
	printk("-- here3\n");

	jerry_value_free(ret);
	jerry_value_free(args);

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

	// if (IS_ENABLED(CONFIG_DISK_DRIVER_RAM)) {
	// 	mount[] = "/RAM:/";
	// } else if (IS_ENABLED(CONFIG_DISK_DRIVER_SDMMC)) {
	// 	mount[] = "/SD:/";
	// } else {
	// 	mount[] = "/NAND:/";
	// }

	sprintf(file_name, "%s", pfile_name);

	fs_file_t_init(&testfile);

	res = fs_open(&testfile, file_name, FS_O_CREATE | FS_O_RDWR);

	if (res < 0) {
		printk("-- ERROR: while openning the file: %s\n", file_name);
		return res;
	}

	res = fs_write(&testfile, pfile_data, strlen(pfile_data));

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

	char mount[] = "/SD:/";
	// if (IS_ENABLED(CONFIG_DISK_DRIVER_RAM)) {
	// 	mount[] = "/RAM:/";
	// } else if (IS_ENABLED(CONFIG_DISK_DRIVER_SDMMC)) {
	// 	mount[] = "/SD:/";
	// } else {
	// 	mount[] = "/NAND:/";
	// }

	sprintf(file_name, "%s%s", mount, pfile_name);

	fs_file_t_init(&testfile);

	res = fs_open(&testfile, file_name, FS_O_READ);

	if(res < 0){
		printk("-- ERROR: while openning the file: %s\n", file_name);
		return res;
	}

	res = fs_read(&testfile, pfile_data, 256);

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
	int ret;

	setup_disk();

	/* THE USB MASS STORAGE CAN BE DISABLED */
	ret = usb_enable(NULL);
	if (ret != 0)
	{
		return 1;
	}

	return 0;
}
