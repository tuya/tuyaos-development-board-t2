/**
* @file examples_os_uf_file.c
* @author www.tuya.com
* @brief examples_os_uf_file module is used to 
* @version 0.1
* @date 2022-05-20
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "tuya_cloud_types.h"
#include "tuya_uf_db.h"

#include "tal_memory.h"
#include "tal_log.h"

/***********************************************************
*************************micro define***********************
***********************************************************/


/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/


/***********************************************************
***********************function define**********************
***********************************************************/
/**
* @brief base db task
*
* @param[in] param:Task parameters
* @return none
*/
VOID example_uf_file(INT_T argc, CHAR_T *argv[])
{
    OPERATE_RET op_ret = OPRT_OK;
    UCHAR_T * fd = NULL;
    UCHAR_T write_buff[] = {"Hello tuya"};
    UCHAR_T *read_buff = NULL;
    INT_T write_num = 0;
    INT_T read_num = 0;
    BOOL_T temp = TRUE;

    TAL_PR_NOTICE("------ uf file example start ------");

    /*open file, read and write mode*/
    fd = ufopen("test.txt", "w+");
    if(NULL == fd) {
        TAL_PR_ERR("file open fail");
    } else {
        TAL_PR_DEBUG("fd : %d", *fd);
    }

    /*write*/
    write_num = ufwrite(fd, write_buff, strlen((const char *)write_buff));
    TAL_PR_DEBUG("write number : %d", write_num);

    /*check if the current offset reach the end of the file*/
    temp = ufeof(fd);
    if(temp) {
        TAL_PR_DEBUG("offset is at the end");
    } else {
        TAL_PR_DEBUG("offset is not at the end");
    }

    /*change offset*/
    ufseek(fd, 0, UF_SEEK_SET);

    /*get current offset of the unique file*/
    op_ret = uftell(fd);
    TAL_PR_DEBUG("current offset : %d", op_ret);

    /*get the size of the unique file*/
    op_ret = ufgetsize("test.txt");
    TAL_PR_DEBUG("file size: %d", op_ret);

    /*read*/
    read_buff = (UCHAR_T *)tal_malloc(sizeof(CHAR_T)*write_num + 1);	
    if(NULL == read_buff) {
        TAL_PR_ERR("read buff malloc fail!");
    }
    read_num = ufread(fd, read_buff, write_num);
    TAL_PR_DEBUG("reads num: %d, read buff: %s", read_num, read_buff);

    /*close*/
    op_ret = ufclose(fd);
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>, close fail!", op_ret);
    }

    /*delete file*/
    op_ret = ufdelete("test.txt");
    if(OPRT_OK != op_ret) {
        TAL_PR_ERR("err<%d>,delete file fail!", op_ret);
    }
    tal_free(read_buff);

    return;
}
