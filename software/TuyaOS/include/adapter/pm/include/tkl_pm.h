#ifndef _TKL_PM_H_
#define _TKL_PM_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include "tuya_cloud_types.h"
#include "tuya_list.h"



typedef INT_T IOCTRL_CMD;

#define IOCTRL_CMD_SET                  1
#define IOCTRL_CMD_GET_STRING           2
#define IOCTRL_CMD_TRANS_INT_STRING     3
#define IOCTRL_CMD_TRANS_RAW            4

typedef struct {
  	LIST_HEAD list;
	/*device name -- it is devcie unique identifier*/
	CHAR_T name[16];
	/* voltage output range (inclusive) - for voltage control */
	INT_T min_mV;
	INT_T max_mV;
	/* current output range (inclusive) - for current control */
	INT_T min_mA;
	INT_T max_mA;
    /* 用户私有定义值 */
    UINT_T usr_arg;
}TUYA_PM_DEV_DESC_T;

/**
 * @brief 取消一个已经注册到tkl层一个电源管理设备的对象
 *
 * @param devname 设备名称

 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tkl_pm_dev_unregistor(CONST CHAR_T *devname);


/**
 * @brief 获取指定设备的描述信息。
 *
 * @param devname 设备名称
 *
 * @return TUYA_PM_DEV_DESC_T 设备描述对象
 */
TUYA_PM_DEV_DESC_T *tkl_pm_get_dev_info(CONST CHAR_T *devname);


/**
 * @brief 获取在注册到tkl框架中的pm设备列表。
 *
 * @param 无
 *
 * @return TUYA_PM_DEV_DESC_T pm设备列表头指针
 */
TUYA_PM_DEV_DESC_T *tkl_pm_get_dev_list_head(VOID_T);


/**
 * @brief 设置设备电源输出电压。
 *
 * @param devname 设备名称
 * @param mV 输出电压，单位毫伏
 *
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tkl_pm_set_voltage(CONST CHAR_T *devname, INT_T mV);


/**
 * @brief 获取设备电源当前输出电压。
 *
 * @param devname 设备名称
 *
 * @return 电压，单位毫伏 .小于0 失败
 */
INT_T tkl_pm_get_voltage(CONST CHAR_T *devname);

/**
 * @brief 设置设备电源输出电流。
 *
 * @param devname 设备名称
 * @param mA 输出电压，单位毫安
 *
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tkl_pm_set_current(CONST CHAR_T *devname,INT_T mA);

/**
 * @brief 获取设备电源当前输出电流。
 *
 * @param devname 设备名称
 *
 * @return 电压，单位毫安，小于0 失败
 */
INT_T  tkl_pm_get_current(CONST CHAR_T *devname);

/**
 * @brief 使能设备电源功能。
 *
 * @param devname 设备名称
 * @param lp_en 低功耗是否关闭
 *
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tkl_pm_enable(CONST CHAR_T *devname,INT_T lp_en);

/**
 * @brief 关闭设备电源功能。
 *
 * @param devname 设备名称
 *
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tkl_pm_disable(CONST CHAR_T *devname);

/**
 * @brief 获取设备电源功能是否使能。
 *
 * @param devname 设备名称
 * @param status 返回状态，TRUE enable,FALSE disable
 * @return 1 打开；0 关闭
 */
OPERATE_RET tkl_pm_is_enable(CONST CHAR_T *devname,BOOL_T *status);


/**
 * @brief 关闭系统电源，设备关机。
 *
 * @param devname 设备名称
 * @status
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tkl_pm_power_off(CONST CHAR_T *devname);


/**
 * @brief 重启设备，该设备可以是整个系统，也可以是某个单独管理的设备。
 *
 * @param devname 设备名称
 *
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tkl_pm_reset(CONST CHAR_T *devname);

/**
 * @brief 设备的ioctl接口，这个接口是为了给设备提供了扩展的接口
 *
 * @param devname  设备名称
 * @param ctl_cmd  控制命令
 * @param param    输入参数指针
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tkl_pm_ioctl(CONST CHAR_T *devname,INT_T ctl_cmd,VOID_T *param);



#ifdef __cplusplus
}
#endif

#endif