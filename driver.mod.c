#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x122c3a7e, "_printk" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x3fa498ad, "class_create" },
	{ 0xfbe72bcf, "device_create" },
	{ 0xe78e8907, "cdev_init" },
	{ 0x39f012c6, "cdev_add" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xee9d5dd1, "i2c_get_adapter" },
	{ 0x900c9157, "i2c_new_client_device" },
	{ 0xfe266025, "i2c_register_driver" },
	{ 0xb1a12415, "i2c_put_adapter" },
	{ 0xb86bb5ac, "device_destroy" },
	{ 0xa4e5832c, "class_destroy" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xfe990052, "gpio_free" },
	{ 0xc8f72763, "i2c_unregister_device" },
	{ 0xb02e77f7, "i2c_del_driver" },
	{ 0xa4afd25e, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xb0d2a97, "gpio_to_desc" },
	{ 0xb161b235, "gpiod_direction_output_raw" },
	{ 0x3524cbe9, "gpiod_set_raw_value" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xcee724e1, "gpiod_direction_input" },
	{ 0xcfeba4de, "gpiod_get_raw_value" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x7682ba4e, "__copy_overflow" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x86c9432a, "i2c_transfer_buffer_flags" },
	{ 0xdcb764ad, "memset" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0xf9a482f9, "msleep" },
	{ 0x67a35d9, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("i2c:OLED_SSD1306");

MODULE_INFO(srcversion, "4F3BBEDAD5667AA3B8C3DDC");
