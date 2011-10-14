本实验将实现如下功能：本实验将控制开发板上载有的3个按钮（KEY_UP、KEY0、KEY1），来控制板上的2个LED（DS0和DS1），其中KEY0控制DS0，按一次亮，再按一次，就灭。KEY1控制DS1，效果同KEY0。KEY_UP则同时控制DS0和DS1，按一次，他们的状态就翻转一次。

此代码只可以SWD这一种方式仿真，并且会得不到正确的结果，所以本实验不需要仿真，直接下载了拔掉JLINK,才能得到正确的结果。

               正点原子@ALIENTEK
               2010-12-29
	       购买：http://shop62103354.taobao.com
                     http://shop62057469.taobao.com
               技术支持论坛：www.openedv.com