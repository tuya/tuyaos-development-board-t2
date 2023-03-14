############################################################
# help / version
############################################################
version:
	@echo "V3.4.0"
	@echo "支持基线TuyaOS 3.4.0版本"
	@echo "------------------------------------"

help: version
	@echo "clean         : 清理app编译现场"
	@echo "config        : 生成配置头文件"
	@echo "os            : 编译各个组件代码"
	@echo "os_clean      : 清理sdk编译现场"
	@echo "tydoc         : 执行自动化文档"
	@echo "pack          : 打包产物生成压缩包"
	@echo "info          : 输出编译详情"
	@echo "app           : 编译调试应用代码"
	@echo "menuconfig    : 手动调整配置文件"
	@echo "config_choice : 从所有配置文件中选择"
	@echo "version       : 显示Makefile的版本"

.PHONY: version help
