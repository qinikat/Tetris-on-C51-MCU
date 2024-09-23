# Tetris-on-C51-MCU

## C51单片机上的俄罗斯方块

硬件特性：

1. 8x8的LED的矩阵灯显示

2. 8位LED的二进制的得分表，上限得分2^8=256分

3. 按键操作

按键功能说明：

​			按键S1 - 游戏速度调节，共0.4-1.5秒方块下降间隔

​			按键S5 - 游戏开始/重新开始

​			按键S9 - 游戏暂停

​			按键S14，S15，S16 - 分别为方块左移动，下移动，右移动

​			按键S11 - 方块旋转

#### 更新记录：

V1.0 - 首发版本。

​	BUG记录:

​					1.目前已发现方块L，I，J贴紧左边界选择时会出现卡死现象，需重新上电。

### 样例图片

![sample](https://github.com/qinikat/Tetris-on-C51-MCU/blob/master/Sample_Picture/sample.webp)
