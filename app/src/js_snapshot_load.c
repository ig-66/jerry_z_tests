#include "include/js_snapshot_load.h"
#include "storage/storage.h"

#include "jerryscript.h"
#include "jerryscript-ext/handlers.h"
#include "jerryscript-ext/properties.h"

#include <zephyr/zephyr.h>
#include <zephyr/fs/fs.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <stdio.h>
#include <string.h>

#define SDCARD DT_NODELABEL(sdhc0)

#if DT_NODE_HAS_STATUS(SDCARD, okay)
#else
#error "Unsupported board: SD card devicetree alias is not defined"
#endif


int exec_snapshot_file();


int main(){

	const struct device *sd_card_dev = DEVICE_DT_GET(SDCARD);

	if (!device_is_ready(sd_card_dev)) {
		/* Not ready, do not use */
		printk("SD card not ready\n");
		return -ENODEV;
	}
	printk("SD card ready!\n");

	storage_init();

	js_example_snapshot_load();

	return 0;
}


void js_example_snapshot_load(void){

	/* Init jerry engine */
	jerry_init(JERRY_INIT_EMPTY);

	/* Register print function */
	jerryx_register_global ("print", jerryx_handler_print);

	// if (IS_ENABLED(CONFIG_DISK_DRIVER_FLASH)) {
	//     static char mnt[] = "/NAND:/.snapshot";
	// } else if (IS_ENABLED(CONFIG_DISK_DRIVER_SDMMC)) {
	//     static char mnt[] = "/SD:/.snapshot";
	// } else {
	//     printk("--- ERROR: Disk driver was not defined\n");
	// }
	// printk("file location: %s\n", mnt);

	char file_name[] = "/SD:/.snapshot";
	exec_snapshot_file(file_name);

	/* Kill jerry engine */
	jerry_cleanup();
}


int exec_snapshot_file(char *snapshot_file_name){

	int error = 0;

	struct fs_file_t snapshot_file;

	uint32_t Snapshot[512];

	char file_name[32];
	sprintf(file_name, "%s", snapshot_file_name);
	
	printk("Opening snapshot file: %s...\n", file_name);
	
	fs_file_t_init(&snapshot_file);

	// if(fs_open(&snapshot_file, file_name, FS_O_READ) < 0) {
	//     printk("*** ERROR: Error while opening the file: %s\n", file_name);
	//     return -1;
	// } 
	error = fs_open(&snapshot_file, file_name, FS_O_READ);
	if(error < 0) {
		printk("*** ERROR: Error while opening the file: %s. Error %d\n", file_name, error);
		return error;
	} 


	if(fs_read(&snapshot_file, Snapshot, sizeof(Snapshot)) < 0) {
		printk("*** ERROR: Error while reading the file: %s\n", file_name);
		return -1;
	} 

	if (fs_close(&snapshot_file) < 0) {
		printk("*** ERROR: Error while closing the file: %s\n", file_name);
		return -1;
	}

	else {

		/* Execute snapshot */
		jerry_value_t ret_value = jerry_exec_snapshot (Snapshot, sizeof(Snapshot), 0, 0, NULL);

		if (jerry_value_is_exception(ret_value)) {
			printk("*** ERROR: Snapshot Execute error: Snapshot is an exception!\n");
			error = -1;
		}

		/* Free used values */
		jerry_value_free(ret_value);

	}

	return error;
}

