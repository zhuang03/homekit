
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "sdkconfig.h"

#include "driver/sdmmc_host.h"

#include "lv_sd_fatfs.h"

static const char *TAG = "sd";
#define MOUNT_POINT "/sdcard"

int photo_file_num = 0;
char file_name_list[DIR_FILE_NUM][DIR_FILE_NAME_MAX_LEN];
typedef  FIL file_t;
/*
 * IO14      CLK
 * IO15      CMD
 * IO2       D0
 * IO4       D1
 * IO12      D2
 * IO13      D3
 */
void release_file_info(File_Info *info)
{
    File_Info *cur_node = NULL; // 记录当前节点
    if (NULL == info)
    {
        return;
    }
    for (cur_node = info->next_node; NULL != cur_node;)
    {
        // 判断是不是循环一圈回来了
        if (info->next_node == cur_node)
        {
            break;
        }
        File_Info *tmp = cur_node; // 保存准备删除的节点
        cur_node = cur_node->next_node;
        free(tmp);
    }
    free(info);
}

void join_path(char *dst_path, const char *pre_path, const char *rear_path)
{
    while (*pre_path != 0)
    {
        *dst_path = *pre_path;
        ++dst_path;
        ++pre_path;
    }
    if (*(pre_path - 1) != '/')
    {
        *dst_path = '/';
        ++dst_path;
    }

    if (*rear_path == '/')
    {
        ++rear_path;
    }
    while (*rear_path != 0)
    {
        *dst_path = *rear_path;
        ++dst_path;
        ++rear_path;
    }
    *dst_path = 0;
}

int get_filename_len(const char *path)
{
    // 获取最后一个'/'所在的下标
    int index = -1;
    for (int cnt = 0; path[cnt] != 0; ++cnt)
    {
        if (path[cnt] == '/')
        {
            index = cnt;
        }
    }
    return index;
}

/*SD卡初始化*/
void SdCard::init()
{
  esp_err_t ret;
  /*Options for mounting the filesystem.
   *If format_if_mount_failed is set to true, SD card will be partitioned and formatted in case when mounting fails.*/
   esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
     };

  sdmmc_card_t* card;
  const char mount_point[] = MOUNT_POINT;
  ESP_LOGI(TAG, "Initializing SD card");


  ESP_LOGI(TAG, "Using SDMMC peripheral");
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();

  /*This initializes the slot without card detect (CD) and write protect (WP) signals.*/
  /* Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.*/
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

  /*To use 1-line SD mode, uncomment the following line:*/
  slot_config.width = 1;

  /*GPIOs 15, 2, 4, 12, 13 should have external 10k pull-ups.*/
  /*Internal pull-ups are not sufficient. However, enabling internal pull-ups*/
  /*does make a difference some boards, so we do that here.*/
  gpio_set_pull_mode(GPIO_NUM_15, GPIO_PULLUP_ONLY);    // CMD, needed in 4- and 1- line modes
  gpio_set_pull_mode(GPIO_NUM_2,  GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
  gpio_set_pull_mode(GPIO_NUM_4,  GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
 //   gpio_set_pull_mode(12, GPIO_PULLUP_ONLY);    // D2, needed in 4-line mode only
 //   gpio_set_pull_mode(13, GPIO_PULLUP_ONLY);    // D3, needed in 4- and 1-line modes

  ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);
  if(ret != ESP_OK) {
     if(ret == ESP_FAIL) {
        ESP_LOGE(TAG, "Failed to mount filesystem. "
            "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        }else {
          ESP_LOGE(TAG, "Failed to initialize the card (%s). "
             "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
     return;
    }

  /*Card has been initialized, print its properties*/
  sdmmc_card_print_info(stdout, card);

}

/*列表目录*/
void SdCard::listDir(const char *dirname, uint8_t levels)
{
  
    photo_file_num = 0;
    file_t file_p;
    levels =0x12;

   FRESULT res = f_open(&file_p, MOUNT_POINT"/hello.txt", levels);
	if(res == FR_OK)
	{
		  ESP_LOGE(TAG,"Listing directory: %s\n", dirname);
		
	}
 
// FRESULT res = f_open((file_t*)file_p, "/hello.txt", dirname);

    // File root = SD.open(dirname);
    // if (!root)
    // {
    //     Serial.println("Failed to open directory");
    //     return;
    // }
    // if (!root.isDirectory())
    // {
    //     Serial.println("Not a directory");
    //     return;
    // }

    // int dir_len = strlen(dirname) + 1;

    // File file = root.openNextFile();
    // while (file && photo_file_num < DIR_FILE_NUM)
    // {
    //     if (file.isDirectory())
    //     {
    //         Serial.print("  DIR : ");
    //         Serial.println(file.name());
    //         if (levels)
    //         {
    //             listDir(file.name(), levels - 1);
    //         }
    //     }
    //     else
    //     {
    //         Serial.print("  FILE: ");
    //         // 只取文件名 保存到file_name_list中
    //         strncpy(file_name_list[photo_file_num], file.name() + dir_len, DIR_FILE_NAME_MAX_LEN - 1);
    //         file_name_list[photo_file_num][strlen(file_name_list[photo_file_num]) - 4] = 0;

    //         char file_name[FILENAME_MAX_LEN] = {0};
    //         sprintf(file_name, "%s/%s.bin", dirname, file_name_list[photo_file_num]);
    //         Serial.print(file_name);
    //         ++photo_file_num;
    //         Serial.print("  SIZE: ");
    //         Serial.println(file.size());
    //     }
    //     file = root.openNextFile();
    // }
    // Serial.println(photo_file_num);
}


// File_Info *SdCard::listDir(const char *dirname)
// {
//     Serial.printf("Listing directory: %s\n", dirname);

//     File root = SD.open(dirname);
//     if (!root)
//     {
//         Serial.println("Failed to open directory");
//         return NULL;
//     }
//     if (!root.isDirectory())
//     {
//         Serial.println("Not a directory");
//         return NULL;
//     }

//     int dir_len = strlen(dirname) + 1;

//     // 头节点的创建（头节点用来记录此文件夹）
//     File_Info *head_file = (File_Info *)malloc(sizeof(File_Info));
//     head_file->file_type = FILE_TYPE_FOLDER;
//     head_file->file_name = (char *)malloc(dir_len);
//     // 将文件夹名赋值给头节点（当作这个节点的文件名）
//     strncpy(head_file->file_name, dirname, dir_len - 1);
//     head_file->file_name[dir_len - 1] = 0;
//     head_file->front_node = NULL;
//     head_file->next_node = NULL;

//     File_Info *file_node = head_file;

//     File file = root.openNextFile();
//     while (file)
//     {
//         // if (levels)
//         // {
//         //     listDir(file.name(), levels - 1);
//         // }
//         int index = get_filename_len(file.name());
//         // 字符数组长度为实际字符串长度+1
//         int filename_len = strlen(file.name()) - (index + 1) + 1;
//         if (filename_len > FILENAME_MAX_LEN - 10)
//         {
//             Serial.println("Filename is too long.");
//         }

//         // 创建新节点
//         file_node->next_node = (File_Info *)malloc(sizeof(File_Info));
//         // 让下一个节点指向当前节点
//         // （此时第一个节点的front_next会指向head节点，等遍历结束再调一下）
//         file_node->next_node->front_node = file_node;
//         // file_node指针移向节点
//         file_node = file_node->next_node;

//         // 船家创建新节点的文件名
//         file_node->file_name = (char *)calloc(1, filename_len);
//         strncpy(file_node->file_name, file.name() + (index + 1), filename_len); //
//         file_node->file_name[filename_len - 1] = 0;                             //
//         // 下一个节点赋空
//         file_node->next_node = NULL;

//         char tmp_file_name[FILENAME_MAX_LEN] = {0};
//         // sprintf(tmp_file_name, "%s/%s", dirname, file_node->file_name);
//         join_path(tmp_file_name, dirname, file_node->file_name);
//         if (file.isDirectory())
//         {
//             file_node->file_type = FILE_TYPE_FOLDER;
//             // 类型为文件夹
//             Serial.print("  DIR : ");
//             Serial.println(tmp_file_name);
//         }
//         else
//         {
//             file_node->file_type = FILE_TYPE_FILE;
//             // 类型为文件
//             Serial.print("  FILE: ");
//             Serial.print(tmp_file_name);
//             Serial.print("  SIZE: ");
//             Serial.println(file.size());
//         }

//         file = root.openNextFile();
//     }

//     if (NULL != head_file->next_node)
//     {
//         // 将最后一个节点的next_node指针指向 head_file->next_node
//         file_node->next_node = head_file->next_node;
//         // 调整第一个数据节点的front_node指针（非head节点）
//         head_file->next_node->front_node = file_node;
//     }
//     return head_file;
// }

// void SdCard::createDir(const char *path)
// {
//     Serial.printf("Creating Dir: %s\n", path);
//     if (SD.mkdir(path))
//     {
//         Serial.println("Dir created");
//     }
//     else
//     {
//         Serial.println("mkdir failed");
//     }
// }

// void SdCard::removeDir(const char *path)
// {
//     Serial.printf("Removing Dir: %s\n", path);
//     if (SD.rmdir(path))
//     {
//         Serial.println("Dir removed");
//     }
//     else
//     {
//         Serial.println("rmdir failed");
//     }
// }

// void SdCard::readFile(const char *path)
// {
//     Serial.printf("Reading file: %s\n", path);

//     File file = SD.open(path);
//     if (!file)
//     {
//         Serial.println("Failed to open file for reading");
//         return;
//     }

//     Serial.print("Read from file: ");
//     while (file.available())
//     {
//         Serial.write(file.read());
//     }
//     file.close();
// }

// String SdCard::readFileLine(const char *path, int num)
// {
//     Serial.printf("Reading file: %s line: %d\n", path, num);

//     File file = SD.open(path);
//     if (!file)
//     {
//         return ("Failed to open file for reading");
//     }

//     char *p = buf;
//     while (file.available())
//     {
//         char c = file.read();
//         if (c == '\n')
//         {
//             num--;
//             if (num == 0)
//             {
//                 *(p++) = '\0';
//                 String s(buf);
//                 s.trim();
//                 return s;
//             }
//         }
//         else if (num == 1)
//         {
//             *(p++) = c;
//         }
//     }
//     file.close();

//     return String("error parameter!");
// }

// void SdCard::writeFile(const char *path, const char *info)
// {
//     Serial.printf("Writing file: %s\n", path);

//     File file = SD.open(path, FILE_WRITE);
//     if (!file)
//     {
//         Serial.println("Failed to open file for writing");
//         return;
//     }
//     if (file.println(info))
//     {
//         Serial.println("Write succ");
//     }
//     else
//     {
//         Serial.println("Write failed");
//     }
//     file.close();
// }

// File SdCard::open(const String &path, const char *mode)
// {
//     return SD.open(path, FILE_WRITE);
// }

// void SdCard::appendFile(const char *path, const char *message)
// {
//     Serial.printf("Appending to file: %s\n", path);

//     File file = SD.open(path, FILE_APPEND);
//     if (!file)
//     {
//         Serial.println("Failed to open file for appending");
//         return;
//     }
//     if (file.print(message))
//     {
//         Serial.println("Message appended");
//     }
//     else
//     {
//         Serial.println("Append failed");
//     }
//     file.close();
// }

// void SdCard::renameFile(const char *path1, const char *path2)
// {
//     Serial.printf("Renaming file %s to %s\n", path1, path2);
//     if (SD.rename(path1, path2))
//     {
//         Serial.println("File renamed");
//     }
//     else
//     {
//         Serial.println("Rename failed");
//     }
// }

// boolean SdCard::deleteFile(const char *path)
// {
//     Serial.printf("Deleting file: %s\n", path);
//     if (SD.remove(path))
//     {
//         Serial.println("File deleted");
//         return true;
//     }
//     else
//     {
//         Serial.println("Delete failed");
//     }
//     return false;
// }

// boolean SdCard::deleteFile(const String &path)
// {
//     Serial.printf("Deleting file: %s\n", path);
//     if (SD.remove(path))
//     {
//         Serial.println("File deleted");
//         return true;
//     }
//     else
//     {
//         Serial.println("Delete failed");
//     }
//     return false;
// }

// void SdCard::readBinFromSd(const char *path, uint8_t *buf)
// {
//     File file = SD.open(path);
//     size_t len = 0;
//     if (file)
//     {
//         len = file.size();

//         while (len)
//         {
//             size_t toRead = len;
//             if (toRead > 512)
//             {
//                 toRead = 512;
//             }
//             file.read(buf, toRead);
//             len -= toRead;
//         }

//         file.close();
//     }
//     else
//     {
//         Serial.println("Failed to open file for reading");
//     }
// }

// void SdCard::writeBinToSd(const char *path, uint8_t *buf)
// {
//     File file = SD.open(path, FILE_WRITE);
//     if (!file)
//     {
//         Serial.println("Failed to open file for writing");
//         return;
//     }

//     size_t i;
//     for (i = 0; i < 2048; i++)
//     {
//         file.write(buf, 512);
//     }
//     file.close();
// }

// void SdCard::fileIO(const char *path)
// {
//     File file = SD.open(path);
//     static uint8_t buf[512];
//     size_t len = 0;
//     uint32_t start = millis();
//     uint32_t end = start;
//     if (file)
//     {
//         len = file.size();
//         size_t flen = len;
//         start = millis();
//         while (len)
//         {
//             size_t toRead = len;
//             if (toRead > 512)
//             {
//                 toRead = 512;
//             }
//             file.read(buf, toRead);
//             len -= toRead;
//         }
//         end = millis() - start;
//         Serial.printf("%u bytes read for %u ms\n", flen, end);
//         file.close();
//     }
//     else
//     {
//         Serial.println("Failed to open file for reading");
//     }

//     file = SD.open(path, FILE_WRITE);
//     if (!file)
//     {
//         Serial.println("Failed to open file for writing");
//         return;
//     }

//     size_t i;
//     start = millis();
//     for (i = 0; i < 2048; i++)
//     {
//         file.write(buf, 512);
//     }
//     end = millis() - start;
//     Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
//     file.close();
// }