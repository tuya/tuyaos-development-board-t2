# 组件错误码生成工具使用

## 0. 错误码格式

* 错误码定义为2字节
* 定义时会添加负号：`(-0x0d00)`

## 1. 使用

```bash
python3 ./genCode.py  # 兼容python2和python3
```

参数1：指定文件中间名，如：`""`或`"base"`

参数2：可以指定模块列表，使用**逗号或空格**分隔：

```bash
python3 ./genCode.py "" "svc_mf_test svc_dp base_event"
python3 ./genCode.py "base" "svc_mf_test, svc_dp, base_event"
```

## 2. 产物

### 2.1 错误码头文件

执行命令后，在`release`目录下生成头文件（`tuya_error_code.h`）。

### 2.2 对外文档

执行命令后，在`release`目录下生成文档（`tuya_error_code.md`）。

## 3. 项目适配步骤

1. 在项目的编译准备阶段，下载最新的**错误码管理仓库**；
1. 进入仓库目录，执行**生成命令**；
1. 将生成后的**头文件**拷贝到所需的目录；
1. 如果需要，将生成的**文档**拷贝到所需目录。

## 4. 配置

新增或修改错误码的宏定义、值、注释等，均在根目录下的`module.json`文件中。

```json
        {"name":"svc_mf_test", "offset":13,  //组件名称 和 对应错误码的起始值
            "errcode":[                      //错误码宏定义 和 对应的注解
                {"UPDATE_DATA_LEN_EXECED":"升级数据长度超过处理上限"},
                {"UPDATE_CRC_ERROR":"升级crc校验失败"}
            ]
        },

```

以上配置会在头文件中生成如下的宏定义：

```c
/****************************************************************************
            the error code marco define for module SVC_MF_TEST 
****************************************************************************/
#define OPRT_SVC_MF_TEST_UPDATE_DATA_LEN_EXECED            (-0x0d00)  //-3328, 升级数据长度超过处理上限
#define OPRT_SVC_MF_TEST_UPDATE_CRC_ERROR                  (-0x0d01)  //-3329, 升级crc校验失败
#define OPRT_SVC_MF_TEST_ERRCODE_MAX_CNT 2
```

会在对外文档中生成如下内容：

```markdown
   No. | Name  | Value | message
-------|-------|-------|--------
1|`OPRT_SVC_MF_TEST_UPDATE_DATA_LEN_EXECED`|`(-0x0d00)`|`-3328, 升级数据长度超过处理上限`
2|`OPRT_SVC_MF_TEST_UPDATE_CRC_ERROR`|`(-0x0d01)`|`-3329, 升级crc校验失败`
```

   No. | Name  | Value | message
-------|-------|-------|--------
1|`OPRT_SVC_MF_TEST_UPDATE_DATA_LEN_EXECED`|`(-0x0d00)`|`-3328, 升级数据长度超过处理上限`
2|`OPRT_SVC_MF_TEST_UPDATE_CRC_ERROR`|`(-0x0d01)`|`-3329, 升级crc校验失败`

## 5. 对外网盘

操作地址： `https://backendng-cn.tuya-inc.com:7799/iss/oss/ossList?`
当前路径： `smart/embed/error_code/`
