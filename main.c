#include <string.h>
#include <stdio.h>
#include "main.h"

// LED点阵行控制
uint8_t gled_row[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
uint8_t gled_col[8] = {0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};

// 游戏控制变量
uint8_t game_state = 0;			// 游戏状态,0为未开始，1为正在运行，2为暂停，3为结束
uint8_t game_speed_state = 10;	// 游戏速度，0为最慢，1为慢，2为中，3为快，4为最快, 暂未使用
uint8_t block_shape = 0;		// 方块类型，0为直线，1为正方形，2为L形，3为反L形，4为Z形，5为反Z形，6为T形
uint8_t yid = 0;				// 方块移动完成标志，0为未完成，1为完成。
uint8_t block_rotate_state = 0; // 方块旋转状态，0为0度，1为90度，2为180度，3为270度

uint8_t block_y = 0; // 范围3*3左上角方块y坐标
uint8_t block_x = 3; // 范围3*3左上角方块x坐标

uint8_t score = 0; // 游戏得分

uint16_t time_temp = 0;		   // 计时器变量，用于控制游戏速度
uint16_t time_temp_randon = 0; // 计时器随机数变量，用于生成随机数

// 定义点阵动态地图
uint8_t pdata led_map[2 + 8 + 1][1 + 8 + 1] = {{0}};
// 定义点阵静态地图
uint8_t xdata led_map_static[2 + 8 + 1][1 + 8 + 1] = {{0}};

// 生成0-6随机数
uint8_t random_num_0_6()
{
	uint8_t num = 0;
	return time_temp_randon % 7;
}

// 生成0-3随机数
uint8_t random_num_0_3()
{
	uint8_t num = 0;
	return time_temp_randon % 4;
}

// 用于反转 8 位二进制数
uint8_t reverse_bits(uint8_t num)
{
	uint8_t reversed_num, i = 0;
	for (i = 0; i < 8; i++)
	{
		reversed_num <<= 1;		   // 左移腾出位置
		reversed_num |= (num & 1); // 获取 num 的最低位并放到 reversed_num 的最低位
		num >>= 1;				   // 右移 num 准备处理下一个位
	}
	return reversed_num;
}

// LED灯计分显示，P2口显示，共8位2进制数
void update_led_display(uint8_t score)
{
	uint8_t reversed_score = reverse_bits(score);
	// 反转 score 的二进制值：0 -> 1, 1 -> 0
	// 因为LED控制的逻辑是 0 为亮，1 为灭，所以要对 score 的二进制位取反
	P2 = ~reversed_score; // 按位取反以适应LED的显示逻辑
}

// 延时函数
void delay_10us(uint16_t ten_us)
{
	while (ten_us--)
		;
}

// 矩阵键盘按键扫描函数
uint8_t key_matrix_ranks_scan(void)
{
	uint8_t key_value = 0;

	KEY_MATRIX_PORT = 0xf7;		 // 给第一列赋值0，其余全为1
	if (KEY_MATRIX_PORT != 0xf7) // 判断第一列按键是否按下
	{
		delay_10us(1000);		 // 消抖
		switch (KEY_MATRIX_PORT) // 保存第一列按键按下后的键值
		{
		case 0x77:
			key_value = 1;
			break;
		case 0xb7:
			key_value = 5;
			break;
		case 0xd7:
			key_value = 9;
			break;
		case 0xe7:
			key_value = 13;
			break;
		}
	}
	while (KEY_MATRIX_PORT != 0xf7)
		; // 等待按键松开

	KEY_MATRIX_PORT = 0xfb;		 // 给第二列赋值0，其余全为1
	if (KEY_MATRIX_PORT != 0xfb) // 判断第二列按键是否按下
	{
		delay_10us(1000);		 // 消抖
		switch (KEY_MATRIX_PORT) // 保存第二列按键按下后的键值
		{
		case 0x7b:
			key_value = 2;
			break;
		case 0xbb:
			key_value = 6;
			break;
		case 0xdb:
			key_value = 10;
			break;
		case 0xeb:
			key_value = 14;
			break;
		}
	}
	while (KEY_MATRIX_PORT != 0xfb)
		; // 等待按键松开

	KEY_MATRIX_PORT = 0xfd;		 // 给第三列赋值0，其余全为1
	if (KEY_MATRIX_PORT != 0xfd) // 判断第三列按键是否按下
	{
		delay_10us(1000);		 // 消抖
		switch (KEY_MATRIX_PORT) // 保存第三列按键按下后的键值
		{
		case 0x7d:
			key_value = 3;
			break;
		case 0xbd:
			key_value = 7;
			break;
		case 0xdd:
			key_value = 11;
			break;
		case 0xed:
			key_value = 15;
			break;
		}
	}
	while (KEY_MATRIX_PORT != 0xfd)
		; // 等待按键松开

	KEY_MATRIX_PORT = 0xfe;		 // 给第四列赋值0，其余全为1
	if (KEY_MATRIX_PORT != 0xfe) // 判断第四列按键是否按下
	{
		delay_10us(1000);		 // 消抖
		switch (KEY_MATRIX_PORT) // 保存第四列按键按下后的键值
		{
		case 0x7e:
			key_value = 4;
			break;
		case 0xbe:
			key_value = 8;
			break;
		case 0xde:
			key_value = 12;
			break;
		case 0xee:
			key_value = 16;
			break;
		}
	}
	while (KEY_MATRIX_PORT != 0xfe)
		; // 等待按键松开

	return key_value;
}

// 定时器0始初化,1ms进入一次中断
void timer0_init()
{
	TMOD |= 0x01; // 选择为定时器0模式，工作方式1，仅用TR0打开启动。
	TH0 = 0xfc;	  // 给定时器赋初值，定时1ms
	TL0 = 0x18;	  // 定时1ms
	ET0 = 1;	  // 打开定时器0中断允许
	EA = 1;		  // 打开总中断
	TR0 = 1;	  // 打开定时器
}

// 74HC595写数据函数
void hc595_write_data(uint8_t dat)
{
	uint8_t i = 0;
	for (i = 0; i < 8; i++) // 循环8次即可将一个字节写入寄存器中
	{
		SER = dat >> 7; // 优先传输一个字节中的高位
		dat <<= 1;		// 将低位移动到高位
		SRCLK = 0;
		delay_10us(1);
		SRCLK = 1;
		delay_10us(1); // 移位寄存器时钟上升沿将端口数据送入寄存器中
	}
	RCLK = 1;
	delay_10us(1);
	RCLK = 0; // 存储寄存器时钟上升沿将前面写入到寄存器的数据输出
}

// 地图显示函数
void show_map(uint8_t led_map[11][10])
{
	uint8_t gled_row_add_temp;
	uint8_t i, j = 0;
	for (i = 0; i < 8; i++) // 循环8次扫描8行、列
	{
		gled_row_add_temp = 0x00; // 初始化为全灭
		for (j = 0; j < 8; j++)
		{
			if (led_map[j + 2][i + 1] == 1)
			{
				gled_row_add_temp += gled_row[j];
			}
		}
		LEDDZ_COL_PORT = gled_col[i];		 // 传送列选数据
		hc595_write_data(gled_row_add_temp); // 传送行选数据
		delay_10us(50);						 // 延时一段时间，等待显示稳定
		hc595_write_data(0x00);				 // 消影
	}
}

// 生成方块
void creatblock(int shape, int form, int x, int y)
{
	uint8_t i, j;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (block[shape][form][i][j] == 1)
			{
				led_map[y + i][x + j] = 1;
			}
		}
	}
}

// 游戏重新开始
void restart_game()
{
	uint8_t n;
	game_state = 1;
	memcpy(led_map, led_map_start, sizeof(led_map));
	time_temp = 5;
	while (time_temp)
	{
		show_map(led_map);
	}
	block_shape, block_y, block_x, yid, time_temp, time_temp_randon, block_rotate_state, score = 0;
	block_x = 3; // 范围4*4左上角方块x坐标
	// 清空地图
	memset(led_map, 0, sizeof(led_map));
	memset(led_map_static, 0, sizeof(led_map));
	// 初始化静态地图边界
	for (n = 0; n < 10; n++)
	{
		led_map_static[n][0] = 1;
		led_map_static[n][9] = 1;
		led_map_static[10][n] = 1;
		// 初始化动态地图边界
		led_map[n][0] = 1;
		led_map[n][9] = 1;
		led_map[10][n] = 1;
	}
}

// 游戏暂停
void pause_game()
{
	if (game_state != 2)
		game_state = 2;
	else
		game_state = 1;
}

// 游戏速度调节
void game_speed_chang()
{
	game_speed_state--;
	if (game_speed_state == 15)
		game_speed_state = 4;
}

// 合法性判断
uint8_t IsLegal(uint8_t x, uint8_t y)
{
	uint8_t i, j;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			// 如果方块落下的位置本来就已经有方块了，则不合法
			if ((block[block_shape][block_rotate_state][i][j] == 1) && (led_map_static[y + i][x + j] == 1))
				return 0; // 不合法
		}
	}
	return 1; // 合法
}

// 以3*3矩阵的范围让方块顺时针旋转,传入范围在地图左上角的坐标
void block_rotate(int block_y, int block_x)
{
	uint8_t i, j = 0;
	uint8_t block_rotate_state_old = block_rotate_state;
	// 尝试旋转方块
	block_rotate_state++;
	if (block_rotate_state == 4)
		block_rotate_state = 0;
	// 判断旋转后的方块是否合法
	if (IsLegal(block_x, block_y))
	{
		// 清除原来的方块
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{

				if (block[block_shape][block_rotate_state_old][i][j] == 1)
				{
					led_map[block_y + i][block_x + j] = 0;
				}
			}
		}
		creatblock(block_shape, block_rotate_state, block_x, block_y);
	}
	else
	{
		block_rotate_state = block_rotate_state_old;
	}
}

// 定时器0中断函数
void timer0() interrupt 1
{
	static uint16_t i = 0;
	TH0 = 0xfc; // 给定时器赋初值，定时1ms
	TL0 = 0x18; // 定时1ms
	i++;
	time_temp_randon++;
	if (i == 100) // 0.1s进入一次清零
	{
		if (time_temp > 0)
		{
			time_temp--;
		};
		i = 0;
	};
	if (time_temp_randon > 10000)
	{
		time_temp_randon = 0;
	};
}

// 通用的方块移动函数
void block_move(BlockMoveDirection direction)
{
	uint8_t i, j;
	uint8_t new_x = block_x;
	uint8_t new_y = block_y;

	// 根据方向更新目标坐标
	switch (direction)
	{
	case MOVE_LEFT:
		new_x = block_x - 1;
		break;
	case MOVE_RIGHT:
		new_x = block_x + 1;
		break;
	case MOVE_DOWN:
		new_y = block_y + 1;
		break;
	}

	// 检查新坐标是否合法
	if (IsLegal(new_x, new_y))
	{
		// 清除原来的方块
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				if (block[block_shape][block_rotate_state][i][j] == 1)
				{
					led_map[block_y + i][block_x + j] = 0;
				}
			}
		}
		// 更新方块的坐标
		block_x = new_x;
		block_y = new_y;
		// 在新位置上绘制方块
		creatblock(block_shape, block_rotate_state, block_x, block_y);
	}
	else if (direction == MOVE_DOWN)
	{
		// 如果无法下移，存储至静态地图
		yid = 0;
	}
}

// 把某行以上内容下移一行
void block_down_move_row(uint8_t line)
{
	uint8_t i, j;
	for (i = line; i > 0; i--)
	{
		for (j = 0; j < 8; j++)
		{
			led_map[i + 1][j + 1] = led_map[i][j + 1];
		}
	}
}

// 消除满行
void block_clear()
{
	uint8_t i, j;
	uint8_t count = 0;
	for (i = 8; i > 0; i--)
	{
		for (j = 0; j < 8; j++)
		{
			if (led_map[i + 1][j + 1] == 1)
			{
				count++;
			}
		}
		if (count == 8)
		{
			// 行以上的内容下落
			block_down_move_row(i);
			score++;
		}
		count = 0;
	}
}

// 判断是否满一列
void block_loc_jud()
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		if (led_map_static[2][i + 1] == 1)
		{
			memcpy(led_map, led_map_over, sizeof(led_map));
			time_temp = 5;
			while (time_temp)
			{
				show_map(led_map);
			}
			game_state = 0;
		}
	}
}

void main()
{
	// uart_init(0XFA); // 波特率为9600
	timer0_init(); // 定时器0初始化
	while (1)
	{
		switch (key_matrix_ranks_scan())
		{
		case 1:
			game_speed_chang();
			break;
		case 5:
			restart_game();
			break;
		case 9:
			pause_game();
			break;
		case 11:
			block_rotate(block_y, block_x);
			break;
		case 14:
			block_move(MOVE_LEFT);
			break;
		case 15:
			block_move(MOVE_DOWN);
			break;
		case 16:
			block_move(MOVE_RIGHT);
			break;
		}
		// 游戏运行
		if (game_state == 1)
		{
			// 判断是否需要生成方块
			if (yid == 0)
			{
				block_clear();			   // 判断满一行，若满一行则消除这一行，使上面方块下移，并得分加一
				update_led_display(score); // 更新分数
				// 更新静态地图,复制数组
				memcpy(led_map_static, led_map, sizeof(led_map));
				block_x = 3;
				block_y = 0;
				block_shape = random_num_0_6();
				block_rotate_state = random_num_0_3();
				creatblock(block_shape, block_rotate_state, 3, 0);
				yid = 1;
			}
			// 定时下移一行
			if (time_temp == 0 && yid == 1)
			{
				block_move(MOVE_DOWN);
				time_temp = game_speed_state;
			};
			// 判断是否满一列
			if (yid == 1)
			{
				block_loc_jud(); // 判断是否满一列，若满一列则游戏结束
			}
		};
		// 显示游戏地图
		show_map(led_map);
	}
}
