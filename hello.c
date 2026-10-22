// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/errno.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Hello world kernel module with timestamps");

/* Кількість привітань */
static unsigned int count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count, "Number of Hello world messages");

/* Структура для збереження часу */
struct hello_event {
	struct list_head list;
	ktime_t time;
};

/* Голова списку */
static LIST_HEAD(hello_list);

/* Звільнення пам'яті та виведення часу */
static void clear_hello_list(void)
{
	struct hello_event *event, *tmp;

	list_for_each_entry_safe(event, tmp, &hello_list, list) {
		pr_info("Hello time: %lld ns\n",
			(long long)ktime_to_ns(event->time));

		list_del(&event->list);
		kfree(event);
	}
}

/* Ініціалізація модуля */
static int __init hello_init(void)
{
	unsigned int i;
	struct hello_event *event;

	if (count > 10) {
		pr_err("Error: count must not exceed 10\n");
		return -EINVAL;
	}

	if (count == 0 || (count >= 5 && count <= 10))
		pr_warn("Warning: unusual count value %u\n", count);

	for (i = 0; i < count; i++) {
		event = kmalloc(sizeof(*event), GFP_KERNEL);

		if (!event) {
			clear_hello_list();
			return -ENOMEM;
		}

		event->time = ktime_get();
		list_add_tail(&event->list, &hello_list);

		pr_info("Hello, world!\n");
	}

	return 0;
}

/* Вивантаження модуля */
static void __exit hello_exit(void)
{
	clear_hello_list();
	pr_info("Module unloaded\n");
}

module_init(hello_init);
module_exit(hello_exit);
