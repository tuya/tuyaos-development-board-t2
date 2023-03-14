# T2开发平台原厂烧录工具使用说明

## 烧录流程介绍
### 1.打开原厂烧录工具

以管理员身份打开名为 `bk_writer_gui_V1.7.5.exe` 的可执行文件。

![image.png](https://images.tuyacn.com/fe-static/docs/img/ceaeaf25-546e-433f-97cb-1b0fffd20f5f.png)

### 2. 烧录对象

烧录对象选择 T2

![image.png](https://images.tuyacn.com/fe-static/docs/img/99827e2e-e732-48d8-a6a2-9506000d455f.png)

### 3.烧录地址

当我们烧录UA文件的时候，需要把起始地址设置为： `0X00011000`。

当我们烧录QIO文件(包括BootLoader和UA文件)的时候，需要把起始地址设置为：`0X00000000`，T2默认烧录QIO文件。


![image.png](https://images.tuyacn.com/fe-static/docs/img/88788cf0-75fa-4a6c-b343-2adb63b811ec.png)

### 4.选择烧录文件

1. 第一步选择浏览
2. 第二步选择编译生成的QIO文件
3. 第三步点击打开

![image.png](https://images.tuyacn.com/fe-static/docs/img/b6251a13-1fa6-48d8-9d12-63d4e4731ddb.png)

### 5.选择COM口
如果开发者不知道是那个下载口，可打开计算机设备管理器，点击端口下拉菜单。
连接、断开模组的下载口，观察端口的变化，即可看到模组的下载口。

波特率可以选择为921600或1500000等

![image.png](https://images.tuyacn.com/fe-static/docs/img/0ed9f474-7150-4121-a854-051df2a4b1d4.png)B

### 6.烧录
点击烧录，同时按下模组的复位按钮，即可完成烧录流程。

![image.png](https://images.tuyacn.com/fe-static/docs/img/bdf797c2-d05f-411e-ab9a-4352b67d6ff9.png)


