#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

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
	{ 0x92a12ccb, "module_layout" },
	{ 0x6308d4e0, "pci_unregister_driver" },
	{ 0xe3ea0ee0, "__pci_register_driver" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xee4d9df4, "pci_read_config_byte" },
	{ 0xc38def9d, "pci_iomap" },
	{ 0x2c0446a4, "pci_request_region" },
	{ 0xa0e45b35, "_dev_err" },
	{ 0x4efda7e7, "pci_enable_device" },
	{ 0xf310fbc7, "__register_chrdev" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x92540fbf, "finish_wait" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0x1000e51, "schedule" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x3eeb2322, "__wake_up" },
	{ 0x4a453f53, "iowrite32" },
	{ 0xa78af5f3, "ioread32" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x206f1f7f, "pci_release_region" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("pci:v00001234d000011EAsv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "F89B8EA292F5C008614447A");
