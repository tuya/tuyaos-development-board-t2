/**
 * @file tdd_sensor_mpu60x0.c
 * @brief tdd_sensor_mpu60x0 module is used to drive mpu60x0 chip
 * @version 0.1
 * @date 2022-08-03
 */

#include "tdl_sensor_driver.h"
#include "tdd_sensor_imu.h"
#include "tal_system.h"
#include "tal_log.h"

/***********************************************************
************************macro define************************
***********************************************************/
/**
 * @brief work status
 */
#define MPU60X0_CLOSE                   0
#define MPU60X0_OPEN                    (!MPU60X0_CLOSE)

/**
 * @brief resource array index
 */
#define MPU60X0_RSRC_INDEX_STAT         0
#define MPU60X0_RSRC_INDEX_ADDR         1
#define MPU60X0_RSRC_INDEX_GFSR         2
#define MPU60X0_RSRC_INDEX_AFSR         3

/**
 * @brief default value
 */
#define MPU60X0_DEV_ID                  0x68
#define MPU60X0_GYRO_OUTPUT_RATE        1000
#define MPU60X0_SMPRT_DIV_MAX           255

/**
 * @brief register bit
 */
#define MPU60X0_REG_BIT_DEVICE_RESET    (1<<7)
#define MPU60X0_REG_BIT_SLEEP           (1<<6)
#define MPU60X0_REG_BIT_CLKSEL          (7<<0)
#define MPU60X0_REG_BIT_STBY_XYZA       (7<<3)
#define MPU60X0_REG_BIT_STBY_XYZG       (7<<0)
#define MPU60X0_REG_BIT_FIFO_EN_XYZG    (7<<4)
#define MPU60X0_REG_BIT_FIFO_EN_XYZA    (1<<3)
#define MPU60X0_REG_BIT_XYZG_ST         (7<<5)
#define MPU60X0_REG_BIT_XYZA_ST         (7<<5)
#define MPU60X0_REG_BIT_FS_SEL          (3<<3)
#define MPU60X0_REG_BIT_AFS_SEL         (3<<3)
#define MPU60X0_REG_BIT_DLPF_CFG        (3<<0)

/**
 * @brief DLPF
 */
#define MPU60X0_DLPF_BW_CFG_0           260
#define MPU60X0_DLPF_BW_CFG_1           184
#define MPU60X0_DLPF_BW_CFG_2           94
#define MPU60X0_DLPF_BW_CFG_3           44
#define MPU60X0_DLPF_BW_CFG_4           21
#define MPU60X0_DLPF_BW_CFG_5           10
#define MPU60X0_DLPF_BW_CFG_6           5

/**
 * @brief mpu60x0 FS_SEL value (GYRO_CONFIG)
 */
#define MPU60X0_FS_SEL_250              0
#define MPU60X0_FS_SEL_500              1
#define MPU60X0_FS_SEL_1000             2
#define MPU60X0_FS_SEL_2000             3

/**
 * @brief mpu60x0 AFS_SEL value (ACCEL_CONFIG)
 */
#define MPU60X0_AFS_SEL_2               0
#define MPU60X0_AFS_SEL_4               1
#define MPU60X0_AFS_SEL_8               2
#define MPU60X0_AFS_SEL_16              3

/**
 * @brief mpu60x0 CLKSEL value (PWR_MGMT_1)
 */
#define MPU60X0_CLKSEL_INTERNAL         0
#define MPU60X0_CLKSEL_PLL_XGYRO        1
#define MPU60X0_CLKSEL_PLL_YGYRO        2
#define MPU60X0_CLKSEL_PLL_ZGYRO        3
#define MPU60X0_CLKSEL_PLL_EXT32K       4
#define MPU60X0_CLKSEL_PLL_EXT19M       5
#define MPU60X0_CLKSEL_KEEP_RESET       7

/***********************************************************
***********************typedef define***********************
***********************************************************/
/**
 * @brief mpu60x0 register address
 */
typedef BYTE_T MPU60X0_RA_E;
#define MPU60X0_RA_XG_OFFS_TC           0x00
#define MPU60X0_RA_YG_OFFS_TC           0x01
#define MPU60X0_RA_ZG_OFFS_TC           0x02
#define MPU60X0_RA_X_FINE_GAIN          0x03
#define MPU60X0_RA_Y_FINE_GAIN          0x04
#define MPU60X0_RA_Z_FINE_GAIN          0x05
#define MPU60X0_RA_XA_OFFS_H            0x06
#define MPU60X0_RA_XA_OFFS_L_TC         0x07
#define MPU60X0_RA_YA_OFFS_H            0x08
#define MPU60X0_RA_YA_OFFS_L_TC         0x09
#define MPU60X0_RA_ZA_OFFS_H            0x0A
#define MPU60X0_RA_ZA_OFFS_L_TC         0x0B
#define MPU60X0_RA_XG_OFFS_USRH         0x13
#define MPU60X0_RA_XG_OFFS_USRL         0x14
#define MPU60X0_RA_YG_OFFS_USRH         0x15
#define MPU60X0_RA_YG_OFFS_USRL         0x16
#define MPU60X0_RA_ZG_OFFS_USRH         0x17
#define MPU60X0_RA_ZG_OFFS_USRL         0x18
#define MPU60X0_RA_SMPRT_DIV            0x19
#define MPU60X0_RA_CONFIG               0x1A
#define MPU60X0_RA_GYRO_CONFIG          0x1B
#define MPU60X0_RA_ACCEL_CONFIG         0x1C
#define MPU60X0_RA_FF_THR               0x1D
#define MPU60X0_RA_FF_DUR               0x1E
#define MPU60X0_RA_MOT_THR              0x1F
#define MPU60X0_RA_MOT_DUR              0x20
#define MPU60X0_RA_ZRMOT_THR            0x21
#define MPU60X0_RA_ZRMOT_DUR            0x22
#define MPU60X0_RA_FIFO_EN              0x23
#define MPU60X0_RA_I2C_MST_CTRL         0x24
#define MPU60X0_RA_I2C_SLV0_ADDR        0x25
#define MPU60X0_RA_I2C_SLV0_REG         0x26
#define MPU60X0_RA_I2C_SLV0_CTRL        0x27
#define MPU60X0_RA_I2C_SLV1_ADDR        0x28
#define MPU60X0_RA_I2C_SLV1_REG         0x29
#define MPU60X0_RA_I2C_SLV1_CTRL        0x2A
#define MPU60X0_RA_I2C_SLV2_ADDR        0x2B
#define MPU60X0_RA_I2C_SLV2_REG         0x2C
#define MPU60X0_RA_I2C_SLV2_CTRL        0x2D
#define MPU60X0_RA_I2C_SLV3_ADDR        0x2E
#define MPU60X0_RA_I2C_SLV3_REG         0x2F
#define MPU60X0_RA_I2C_SLV3_CTRL        0x30
#define MPU60X0_RA_I2C_SLV4_ADDR        0x31
#define MPU60X0_RA_I2C_SLV4_REG         0x32
#define MPU60X0_RA_I2C_SLV4_DO          0x33
#define MPU60X0_RA_I2C_SLV4_CTRL        0x34
#define MPU60X0_RA_I2C_SLV4_DI          0x35
#define MPU60X0_RA_I2C_MST_STATUS       0x36
#define MPU60X0_RA_INT_PIN_CFG          0x37
#define MPU60X0_RA_INT_ENABLE           0x38
#define MPU60X0_RA_DMP_INT_STATUS       0x39
#define MPU60X0_RA_INT_STATUS           0x3A
#define MPU60X0_RA_ACCEL_XOUT_H         0x3B
#define MPU60X0_RA_ACCEL_XOUT_L         0x3C
#define MPU60X0_RA_ACCEL_YOUT_H         0x3D
#define MPU60X0_RA_ACCEL_YOUT_L         0x3E
#define MPU60X0_RA_ACCEL_ZOUT_H         0x3F
#define MPU60X0_RA_ACCEL_ZOUT_L         0x40
#define MPU60X0_RA_TEMP_OUT_H           0x41
#define MPU60X0_RA_TEMP_OUT_L           0x42
#define MPU60X0_RA_GYRO_XOUT_H          0x43
#define MPU60X0_RA_GYRO_XOUT_L          0x44
#define MPU60X0_RA_GYRO_YOUT_H          0x45
#define MPU60X0_RA_GYRO_YOUT_L          0x46
#define MPU60X0_RA_GYRO_ZOUT_H          0x47
#define MPU60X0_RA_GYRO_ZOUT_L          0x48
#define MPU60X0_RA_EXT_SENS_DATA_00     0x49
#define MPU60X0_RA_EXT_SENS_DATA_01     0x4A
#define MPU60X0_RA_EXT_SENS_DATA_02     0x4B
#define MPU60X0_RA_EXT_SENS_DATA_03     0x4C
#define MPU60X0_RA_EXT_SENS_DATA_04     0x4D
#define MPU60X0_RA_EXT_SENS_DATA_05     0x4E
#define MPU60X0_RA_EXT_SENS_DATA_06     0x4F
#define MPU60X0_RA_EXT_SENS_DATA_07     0x50
#define MPU60X0_RA_EXT_SENS_DATA_08     0x51
#define MPU60X0_RA_EXT_SENS_DATA_09     0x52
#define MPU60X0_RA_EXT_SENS_DATA_10     0x53
#define MPU60X0_RA_EXT_SENS_DATA_11     0x54
#define MPU60X0_RA_EXT_SENS_DATA_12     0x55
#define MPU60X0_RA_EXT_SENS_DATA_13     0x56
#define MPU60X0_RA_EXT_SENS_DATA_14     0x57
#define MPU60X0_RA_EXT_SENS_DATA_15     0x58
#define MPU60X0_RA_EXT_SENS_DATA_16     0x59
#define MPU60X0_RA_EXT_SENS_DATA_17     0x5A
#define MPU60X0_RA_EXT_SENS_DATA_18     0x5B
#define MPU60X0_RA_EXT_SENS_DATA_19     0x5C
#define MPU60X0_RA_EXT_SENS_DATA_20     0x5D
#define MPU60X0_RA_EXT_SENS_DATA_21     0x5E
#define MPU60X0_RA_EXT_SENS_DATA_22     0x5F
#define MPU60X0_RA_EXT_SENS_DATA_23 	0x60
#define MPU60X0_RA_MOT_DETECT_STATUS    0x61
#define MPU60X0_RA_I2C_SLV0_DO      	0x63
#define MPU60X0_RA_I2C_SLV1_DO      	0x64
#define MPU60X0_RA_I2C_SLV2_DO      	0x65
#define MPU60X0_RA_I2C_SLV3_DO      	0x66
#define MPU60X0_RA_I2C_MST_DELAY_CTRL   0x67
#define MPU60X0_RA_SIGNAL_PATH_RESET    0x68
#define MPU60X0_RA_MOT_DETECT_CTRL      0x69
#define MPU60X0_RA_USER_CTRL            0x6A
#define MPU60X0_RA_PWR_MGMT_1           0x6B
#define MPU60X0_RA_PWR_MGMT_2           0x6C
#define MPU60X0_RA_BANK_SEL             0x6D
#define MPU60X0_RA_MEM_START_ADDR       0x6E
#define MPU60X0_RA_MEM_R_W              0x6F
#define MPU60X0_RA_DMP_CFG_1            0x70
#define MPU60X0_RA_DMP_CFG_2            0x71
#define MPU60X0_RA_FIFO_COUNTH          0x72
#define MPU60X0_RA_FIFO_COUNTL          0x73
#define MPU60X0_RA_FIFO_R_W             0x74
#define MPU60X0_RA_WHO_AM_I             0x75

/**
 * @brief axis
 */
typedef struct {
    SHORT_T x;
    SHORT_T y;
    SHORT_T z;
} MPU60X0_AXIS_T;

/***********************************************************
********************function declaration********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
/**
 * @brief element value type
 */
STATIC SR_ELE_CFG_T sg_mpu60x0_ele_val_tp[SR_IMU_ELE_NUM] = {
    {
        .id     = SR_IMU_ELE_ID_GYRO_X,
        .val_tp = SR_VAL_TP_FLOAT,
    },

    {
        .id     = SR_IMU_ELE_ID_GYRO_Y,
        .val_tp = SR_VAL_TP_FLOAT,
    },

    {
        .id     = SR_IMU_ELE_ID_GYRO_Z,
        .val_tp = SR_VAL_TP_FLOAT,
    },

    {
        .id     = SR_IMU_ELE_ID_ACCEL_X,
        .val_tp = SR_VAL_TP_FLOAT,
    },
         {
        .id     = SR_IMU_ELE_ID_ACCEL_Y,
        .val_tp = SR_VAL_TP_FLOAT,
    },

    {
        .id     = SR_IMU_ELE_ID_ACCEL_Z,
        .val_tp = SR_VAL_TP_FLOAT,
    },

    {
        .id     = SR_IMU_ELE_ID_TEMP,
        .val_tp = SR_VAL_TP_FLOAT,
    }
};

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief delay (ms)
 *
 * @param[in] tm: delay time
 *
 * @return none
 */
STATIC VOID_T __mpu60x0_delay_ms(IN ULONG_T tm)
{
    tal_system_sleep(tm);
}

/**
 * @brief read data from mpu60x0's register
 *
 * @param[in] dev: device resource
 * @param[in] reg_addr: register address
 * @param[out] reg_val: register value
 * @param[in] reg_num: number of registers to read
 *
 * @return none
 */
STATIC VOID_T __mpu60x0_read_register(IN CONST SR_RSRC_T *dev, IN MPU60X0_RA_E reg_addr, OUT UCHAR_T *reg_val, IN CONST UCHAR_T reg_num)
{
    SR_I2C_MSG_T wr_msg = {
        .flags = SR_I2C_FLAG_WR,
        .addr = dev->info[MPU60X0_RSRC_INDEX_ADDR],
        .len = 1,
        .buff = &reg_addr
    };
    tdd_sensor_i2c_xfer(dev->port, &wr_msg);

    SR_I2C_MSG_T rd_msg = {
        .flags = SR_I2C_FLAG_RD,
        .addr = dev->info[MPU60X0_RSRC_INDEX_ADDR],
        .len = reg_num,
        .buff = reg_val
    };
    tdd_sensor_i2c_xfer(dev->port, &rd_msg);
}

/**
 * @brief write data to mpu60x0's register
 *
 * @param[in] dev: device resource
 * @param[in] reg_addr: register address
 * @param[in] reg_val: register value
 *
 * @return none
 */
STATIC VOID_T __mpu60x0_write_register(IN CONST SR_RSRC_T *dev, IN CONST MPU60X0_RA_E reg_addr, IN CONST UCHAR_T reg_val)
{
    UCHAR_T buf[2];
    buf[0] = reg_addr;
    buf[1] = reg_val;

    SR_I2C_MSG_T wr_msg = {
        .flags = SR_I2C_FLAG_WR,
        .addr = dev->info[MPU60X0_RSRC_INDEX_ADDR],
        .len = 2,
        .buff = buf
    };
    tdd_sensor_i2c_xfer(dev->port, &wr_msg);
}

/**
 * @brief get LSB number
 *
 * @param[in] reg_bit: register valid bits
 *
 * @return LSB number
 */
STATIC UCHAR_T __mpu60x0_get_lsb_num(IN CONST UCHAR_T reg_bit)
{
    UCHAR_T lsb_num = 0;
    UCHAR_T find_code = 0x01;

    for (UCHAR_T i = 0; i < 8; i++) {
        if (reg_bit & find_code) {
            lsb_num = i;
            break;
        } else {
            find_code <<= 1;
        }
    }

    return lsb_num;
}

/**
 * @brief write data to mpu60x0's register bits
 *
 * @param[in] reg_addr: register address
 * @param[in] data: data to be written
 * @param[in] valid_bit: the code of valid bits
 * @param[in] bit_val: the value of the bits
 *
 * @return none
 */
STATIC VOID_T __mpu60x0_write_register_bit(IN CONST SR_RSRC_T *dev, IN CONST UCHAR_T reg_addr,
                                           IN CONST UCHAR_T reg_bit, IN CONST UCHAR_T bit_val)
{
    UCHAR_T reg_val;
    __mpu60x0_read_register(dev, reg_addr, &reg_val, 1);
    reg_val &= (~reg_bit);
    reg_val |= (bit_val << __mpu60x0_get_lsb_num(reg_bit));
    __mpu60x0_write_register(dev, reg_addr, reg_val);
}

/**
 * @brief soft reset
 *
 * @param[in] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __mpu60x0_soft_reset(IN CONST SR_RSRC_T *dev)
{
    __mpu60x0_write_register_bit(dev, MPU60X0_RA_PWR_MGMT_1, MPU60X0_REG_BIT_DEVICE_RESET, 1);
    __mpu60x0_delay_ms(200);
    return OPRT_OK;
}

/**
 * @brief read serial number
 *
 * @param[in] dev: device resource
 * @param[out] serial_nbr: serial number
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __mpu60x0_get_device_id(IN CONST SR_RSRC_T *dev, OUT SR_IMU_DEV_ID_T *dev_id)
{
    __mpu60x0_read_register(dev, MPU60X0_RA_WHO_AM_I, &dev_id->val, 1);
    return OPRT_OK;
}

/**
 * @brief set sleep mode
 *
 * @param[in] dev: device resource
 * @param[in] enabled: TRUE on sleep mode, FALSE on work mode
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __mpu60x0_set_sleep_mode(IN CONST SR_RSRC_T *dev, IN CONST BOOL_T enabled)
{
    if (enabled) {
        __mpu60x0_write_register_bit(dev, MPU60X0_RA_PWR_MGMT_1, MPU60X0_REG_BIT_SLEEP, 1);
    } else {
        __mpu60x0_write_register_bit(dev, MPU60X0_RA_PWR_MGMT_1, MPU60X0_REG_BIT_SLEEP, 0);
    }
    return OPRT_OK;
}

/**
 * @brief set clock source
 *
 * @param[in] dev: device resource
 * @param[in] clk: clock source
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __mpu60x0_set_clk_src(IN CONST SR_RSRC_T *dev, IN CONST SR_IMU_CLK_SRC_E *clk)
{
    UCHAR_T clk_sel;
    OPERATE_RET op_ret = OPRT_OK;

    switch (*clk) {
    case SR_IMU_CLK_INTERNAL:
        clk_sel = MPU60X0_CLKSEL_INTERNAL;
        break;
    case SR_IMU_CLK_PLL_XGYRO:
        clk_sel = MPU60X0_CLKSEL_PLL_XGYRO;
        break;
    case SR_IMU_CLK_PLL_YGYRO:
        clk_sel = MPU60X0_CLKSEL_PLL_YGYRO;
        break;
    case SR_IMU_CLK_PLL_ZGYRO:
        clk_sel = MPU60X0_CLKSEL_PLL_ZGYRO;
        break;
    case SR_IMU_CLK_PLL_EXT32K:
        clk_sel = MPU60X0_CLKSEL_PLL_EXT32K;
        break;
    case SR_IMU_CLK_PLL_EXT19M:
        clk_sel = MPU60X0_CLKSEL_PLL_EXT19M;
        break;
    default:
        op_ret = OPRT_INVALID_PARM;
    }

    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("[MPU60X0] CLK: %d is not supported.", *clk);
        return op_ret;
    }

    __mpu60x0_write_register_bit(dev, MPU60X0_RA_PWR_MGMT_1, MPU60X0_REG_BIT_CLKSEL, clk_sel);

    return OPRT_OK;
}

/**
 * @brief set gyroscope's full-scale range
 *
 * @param[inout] dev: device resource
 * @param[in] g_fsr: gyroscope's full-scale range
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __mpu60x0_set_gyro_fsr(INOUT SR_RSRC_T *dev, IN CONST SR_IMU_GYRO_FSR_E *g_fsr)
{
    UCHAR_T fs_sel;
    OPERATE_RET op_ret = OPRT_OK;

    switch (*g_fsr) {
    case SR_IMU_GYRO_FSR_250:
        fs_sel = MPU60X0_FS_SEL_250;
        break;
    case SR_IMU_GYRO_FSR_500:
        fs_sel = MPU60X0_FS_SEL_500;
        break;
    case SR_IMU_GYRO_FSR_1000:
        fs_sel = MPU60X0_FS_SEL_1000;
        break;
    case SR_IMU_GYRO_FSR_2000:
        fs_sel = MPU60X0_FS_SEL_2000;
        break;
    default:
        op_ret = OPRT_INVALID_PARM;
    }

    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("[MPU60X0] GYRO_FSR: %d is not supported.", *g_fsr);
        return op_ret;
    }

    dev->info[MPU60X0_RSRC_INDEX_GFSR] = fs_sel;
    __mpu60x0_write_register_bit(dev, MPU60X0_RA_GYRO_CONFIG, MPU60X0_REG_BIT_FS_SEL, fs_sel);

    return OPRT_OK;
}

/**
 * @brief set accelerometer's full-scale range
 *
 * @param[inout] dev: device resource
 * @param[in] a_fsr: accelerometer's full-scale range
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __mpu60x0_set_accel_fsr(INOUT SR_RSRC_T *dev, IN CONST SR_IMU_ACCEL_FSR_E *a_fsr)
{
    UCHAR_T afs_sel;
    OPERATE_RET op_ret = OPRT_OK;

    switch (*a_fsr) {
    case SR_IMU_ACCEL_FSR_2:
        afs_sel = MPU60X0_AFS_SEL_2;
        break;
    case SR_IMU_ACCEL_FSR_4:
        afs_sel = MPU60X0_AFS_SEL_4;
        break;
    case SR_IMU_ACCEL_FSR_8:
        afs_sel = MPU60X0_AFS_SEL_8;
        break;
    case SR_IMU_ACCEL_FSR_16:
        afs_sel = MPU60X0_AFS_SEL_16;
        break;
    default:
        op_ret = OPRT_INVALID_PARM;
    }

    if (op_ret != OPRT_OK) {
        TAL_PR_ERR("[MPU60X0] ACCEL_FSR: %d is not supported.", *a_fsr);
        return op_ret;
    }

    dev->info[MPU60X0_RSRC_INDEX_AFSR] = afs_sel;
    __mpu60x0_write_register_bit(dev, MPU60X0_RA_ACCEL_CONFIG, MPU60X0_REG_BIT_AFS_SEL, afs_sel);

    return OPRT_OK;
}

/**
 * @brief get DLPF cfg value to set
 *
 * @param[in] bw: baud width
 *
 * @return DLPF_CFG value
 */
STATIC UCHAR_T __mpu60x0_get_dlpf_cfg(IN CONST SHORT_T bw)
{
    UCHAR_T dlpf_cfg;
    if (bw >= MPU60X0_DLPF_BW_CFG_1) {
        dlpf_cfg = 1;
    } else if (bw >= MPU60X0_DLPF_BW_CFG_2) {
        dlpf_cfg = 2;
    } else if (bw >= MPU60X0_DLPF_BW_CFG_3) {
        dlpf_cfg = 3;
    } else if (bw >= MPU60X0_DLPF_BW_CFG_4) {
        dlpf_cfg = 4;
    } else if (bw >= MPU60X0_DLPF_BW_CFG_5) {
        dlpf_cfg = 5;
    } else {
        dlpf_cfg = 6;
    }
    return dlpf_cfg;
}

/**
 * @brief set MPU60X0's sample rate
 *
 * @param[in] dev: device resource
 * @param[in] freq_hz: sample rate gyroscope output rate divider value
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __mpu60x0_set_sample_rate(IN CONST SR_RSRC_T *dev, IN CONST USHORT_T *freq_hz)
{
    USHORT_T smp_rt = *freq_hz;
    if (smp_rt > MPU60X0_GYRO_OUTPUT_RATE) {
        smp_rt = MPU60X0_GYRO_OUTPUT_RATE;
    }
    if (smp_rt < (MPU60X0_GYRO_OUTPUT_RATE/MPU60X0_SMPRT_DIV_MAX)) {
        smp_rt = (MPU60X0_GYRO_OUTPUT_RATE/MPU60X0_SMPRT_DIV_MAX);
    }

    UCHAR_T div = MPU60X0_GYRO_OUTPUT_RATE / smp_rt - 1;
    __mpu60x0_write_register(dev, MPU60X0_RA_SMPRT_DIV, div);

    UCHAR_T dlpf_cfg = __mpu60x0_get_dlpf_cfg(smp_rt/2);
    __mpu60x0_write_register_bit(dev, MPU60X0_RA_CONFIG, MPU60X0_REG_BIT_DLPF_CFG, dlpf_cfg);

    return OPRT_OK;
}

/**
 * @brief set intterupt
 *
 * @param[in] dev: device resource
 * @param[in] active_low: TRUE - active low, FALSE - active high
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __mpu60x0_set_int(IN CONST SR_RSRC_T *dev, IN CONST SR_IMU_INT_CFG_T *int_cfg)
{
    if (int_cfg->active_low) {
        __mpu60x0_write_register(dev, MPU60X0_RA_INT_PIN_CFG, 0x90);
    } else {
        __mpu60x0_write_register(dev, MPU60X0_RA_INT_PIN_CFG, 0x50);
    }
    __mpu60x0_write_register(dev, MPU60X0_RA_INT_ENABLE, 0x01);
    return OPRT_OK;
}

/**
 * @brief mpu60x0 init
 *
 * @param[inout] dev: device resource
 * @param[in] cfg: configuration
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __mpu60x0_init(INOUT SR_RSRC_T *dev, IN CONST SR_IMU_BASE_CFG_T *cfg)
{
    SR_IMU_DEV_ID_T dev_id;

    __mpu60x0_soft_reset(dev);
    __mpu60x0_get_device_id(dev, &dev_id);

    if (dev_id.val != MPU60X0_DEV_ID) {
        TAL_PR_ERR("[MPU60X0] The device is not found.");
        return OPRT_NOT_FOUND;
    }

    __mpu60x0_set_sleep_mode(dev, FALSE);
    __mpu60x0_set_clk_src(dev, &cfg->clk_src);
    __mpu60x0_set_gyro_fsr(dev, &cfg->gyro_fsr);
    __mpu60x0_set_accel_fsr(dev, &cfg->accel_fsr);
    __mpu60x0_set_sample_rate(dev, &cfg->freq_hz);

    return OPRT_OK;
}

/**
 * @brief read raw data from gyroscope
 *
 * @param[in] dev: device resource
 * @param[out] gyro: raw data from gyroscope
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __mpu60x0_read_gyro(IN CONST SR_RSRC_T *dev, OUT MPU60X0_AXIS_T *gyro)
{
    UCHAR_T buf[6];
    __mpu60x0_read_register(dev, MPU60X0_RA_GYRO_XOUT_H, buf, 6);
    gyro->x = ((SHORT_T)buf[0] << 8) | buf[1];
    gyro->y = ((SHORT_T)buf[2] << 8) | buf[3];
    gyro->z = ((SHORT_T)buf[4] << 8) | buf[5];
    return OPRT_OK;
}

/**
 * @brief read raw data from accelerometer
 *
 * @param[in] dev: device resource
 * @param[out] temp: raw data from accelerometer
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __mpu60x0_read_accel(IN CONST SR_RSRC_T *dev, OUT MPU60X0_AXIS_T *accel)
{
    UCHAR_T buf[6];
    __mpu60x0_read_register(dev, MPU60X0_RA_ACCEL_XOUT_H, buf, 6);
    accel->x = ((SHORT_T)buf[0] << 8) | buf[1];
    accel->y = ((SHORT_T)buf[2] << 8) | buf[3];
    accel->z = ((SHORT_T)buf[4] << 8) | buf[5];
    return OPRT_OK;
}

/**
 * @brief read raw data from temperature sensor
 *
 * @param[in] dev: device resource
 * @param[out] accel: raw data from temperature sensor
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __mpu60x0_read_temp(IN CONST SR_RSRC_T *dev, OUT SHORT_T *temp)
{
    UCHAR_T buf[2];
    __mpu60x0_read_register(dev, MPU60X0_RA_TEMP_OUT_H, buf, 2);
    *temp = ((SHORT_T)buf[0] << 8) | buf[1];
    return OPRT_OK;
}

/**
 * @brief calculate angular rate
 *
 * @param[in] gyro: raw data from gyroscope
 * @param[out] gyro_val: angular rate value in dps
 * @param[in] fs_sel: gyro full-scale range selection
 *
 * @return none
 */
STATIC VOID_T __mpu60x0_calc_gyro(IN CONST SHORT_T gyro, OUT SR_VAL_U *gyro_val, IN CONST UCHAR_T fs_sel)
{
    FLOAT_T gyro_sens = 32768.0f / ((1 << fs_sel) * 250);
    gyro_val->sr_float = gyro / gyro_sens;
}

/**
 * @brief calculate acceleration
 *
 * @param[in] accel: raw data from accelerometer
 * @param[out] accel_val: acceleration value in g
 * @param[in] afs_sel: accel full-scale range selection
 *
 * @return none
 */
STATIC VOID_T __mpu60x0_calc_accel(IN CONST SHORT_T accel, OUT SR_VAL_U *accel_val, IN CONST UCHAR_T afs_sel)
{
    FLOAT_T accel_sens = 32768.0f / ((1 << afs_sel) * 2);
    accel_val->sr_float = accel / accel_sens;
}

/**
 * @brief calculate temperature
 *
 * @param[in] temp: raw data from temperature sensor
 * @param[out] temp_val: temperature value in °C
 *
 * @return none
 */
STATIC VOID_T __mpu60x0_calc_temp(IN CONST SHORT_T temp, OUT SR_VAL_U *temp_val)
{
    temp_val->sr_float = temp / 340.0f + 36.53f;
}

/**
 * @brief open mpu60x0 device
 *
 * @param[inout] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_mpu60x0_open(INOUT SR_RSRC_T *dev)
{
    dev->info[MPU60X0_RSRC_INDEX_STAT] = MPU60X0_OPEN;
    return OPRT_OK;
}

/**
 * @brief close mpu60x0 device
 *
 * @param[inout] dev: device resource
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_mpu60x0_close(INOUT SR_RSRC_T *dev)
{
    dev->info[MPU60X0_RSRC_INDEX_STAT] = MPU60X0_CLOSE;
    return OPRT_OK;
}

/**
 * @brief control mpu60x0 device
 *
 * @param[inout] dev: device resource
 * @param[in] cmd: control command
 * @param[inout] param: command parameter, data type depends on the specified command
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_mpu60x0_control(INOUT SR_RSRC_T* dev, IN UCHAR_T cmd, INOUT VOID_T *param)
{
    OPERATE_RET op_ret = OPRT_OK;

    if ((MPU60X0_OPEN == dev->info[MPU60X0_RSRC_INDEX_STAT]) &&\
        (cmd == SR_IMU_CMD_SOFT_RST ||\
         cmd == SR_IMU_CMD_GET_DEV_ID)) {
        TAL_PR_ERR("[MPU60X0] Command <%d> is only supported when acquisition is closed.");
        return OPRT_COM_ERROR;
    }

    switch (cmd) {
    case SR_IMU_CMD_SOFT_RST:
        op_ret = __mpu60x0_soft_reset(dev);
        break;
    case SR_IMU_CMD_GET_DEV_ID:
        op_ret = __mpu60x0_get_device_id(dev, (SR_IMU_DEV_ID_T *)param);
        break;
    case SR_IMU_CMD_INIT_DEV:
        op_ret = __mpu60x0_init(dev, (SR_IMU_BASE_CFG_T *)param);
        break;
    case SR_IMU_CMD_SET_INT:
        op_ret = __mpu60x0_set_int(dev, (SR_IMU_INT_CFG_T *)param);
        break;
    default:
        op_ret = OPRT_INVALID_PARM;
        TAL_PR_ERR("[MPU60X0] Command <%d> is invalid.", cmd);
        break;
    }
    return op_ret;
}

/**
 * @brief read data from mpu60x0 device
 *
 * @param[in] dev: device
 * @param[out] ele_data: element data
 * @param[in] ele_num: number of elements
 *
 * @return OPRT_OK on success, others on error
 */
STATIC OPERATE_RET __tdd_sensor_mpu60x0_read(IN SR_RSRC_T* dev, OUT SR_ELE_DATA_T *ele_data, IN UCHAR_T ele_num)
{
    MPU60X0_AXIS_T gyro, accel;
    SHORT_T temp;
    __mpu60x0_read_gyro(dev, &gyro);
    __mpu60x0_read_accel(dev, &accel);
    __mpu60x0_read_temp(dev, &temp);

    for (UCHAR_T i = 0; i < ele_num; i++) {
        switch (ele_data[i].id) {
        case SR_IMU_ELE_ID_GYRO_X:
            __mpu60x0_calc_gyro(gyro.x, &ele_data[i].val, dev->info[MPU60X0_RSRC_INDEX_GFSR]);
            break;
        case SR_IMU_ELE_ID_GYRO_Y:
            __mpu60x0_calc_gyro(gyro.y, &ele_data[i].val, dev->info[MPU60X0_RSRC_INDEX_GFSR]);
            break;
        case SR_IMU_ELE_ID_GYRO_Z:
            __mpu60x0_calc_gyro(gyro.z, &ele_data[i].val, dev->info[MPU60X0_RSRC_INDEX_GFSR]);
            break;
        case SR_IMU_ELE_ID_ACCEL_X:
            __mpu60x0_calc_accel(accel.x, &ele_data[i].val, dev->info[MPU60X0_RSRC_INDEX_AFSR]);
            break;
        case SR_IMU_ELE_ID_ACCEL_Y:
            __mpu60x0_calc_accel(accel.y, &ele_data[i].val, dev->info[MPU60X0_RSRC_INDEX_AFSR]);
            break;
        case SR_IMU_ELE_ID_ACCEL_Z:
            __mpu60x0_calc_accel(accel.z, &ele_data[i].val, dev->info[MPU60X0_RSRC_INDEX_AFSR]);
            break;
        case SR_IMU_ELE_ID_TEMP:
            __mpu60x0_calc_temp(temp, &ele_data[i].val);
            break;
        default:
            TAL_PR_ERR("[MPU60X0] Element ID: %d is invalid.", ele_data[i].id);
            break;
        }
    }
    return OPRT_OK;
}

/**
 * @brief register mpu60x0
 *
 * @param[in] name: device name
 * @param[in] i2c_cfg: i2c configuration
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET tdd_sensor_mpu60x0_register(IN CHAR_T *name, IN SR_IMU_I2C_CFG_T i2c_cfg)
{
    // check param
    if (i2c_cfg.gpio.scl == i2c_cfg.gpio.sda) {
        return OPRT_INVALID_PARM;
    }

    // I2C init
    tdd_sensor_i2c_init(i2c_cfg.port, i2c_cfg.gpio);

    // save resourse information
    SR_RSRC_T resource = {0};
    resource.port = i2c_cfg.port;
    resource.handle = NULL;
    resource.info[MPU60X0_RSRC_INDEX_STAT] = MPU60X0_CLOSE;
    resource.info[MPU60X0_RSRC_INDEX_ADDR] = i2c_cfg.addr;

    // define driver interfaces
    STATIC SR_INTFS_T s_intfs;
    s_intfs.open = __tdd_sensor_mpu60x0_open;
    s_intfs.close = __tdd_sensor_mpu60x0_close;
    s_intfs.control = __tdd_sensor_mpu60x0_control;
    s_intfs.read_ele = __tdd_sensor_mpu60x0_read;

    // register the device
    OPERATE_RET op_ret = tdl_sensor_register(name, &s_intfs, SR_IMU_ELE_NUM, sg_mpu60x0_ele_val_tp, &resource);
    if (OPRT_OK != op_ret) {
        return op_ret;
    }

    return OPRT_OK;
}
