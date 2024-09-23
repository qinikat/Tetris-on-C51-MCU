#include <string.h>
#include <stdio.h>
#include "main.h"

// LED�����п���
uint8_t gled_row[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
uint8_t gled_col[8] = {0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};

// ��Ϸ���Ʊ���
uint8_t game_state = 0;			// ��Ϸ״̬,0Ϊδ��ʼ��1Ϊ�������У�2Ϊ��ͣ��3Ϊ����
uint8_t game_speed_state = 10;	// ��Ϸ�ٶȣ�0Ϊ������1Ϊ����2Ϊ�У�3Ϊ�죬4Ϊ���, ��δʹ��
uint8_t block_shape = 0;		// �������ͣ�0Ϊֱ�ߣ�1Ϊ�����Σ�2ΪL�Σ�3Ϊ��L�Σ�4ΪZ�Σ�5Ϊ��Z�Σ�6ΪT��
uint8_t yid = 0;				// �����ƶ���ɱ�־��0Ϊδ��ɣ�1Ϊ��ɡ�
uint8_t block_rotate_state = 0; // ������ת״̬��0Ϊ0�ȣ�1Ϊ90�ȣ�2Ϊ180�ȣ�3Ϊ270��

uint8_t block_y = 0; // ��Χ3*3���ϽǷ���y����
uint8_t block_x = 3; // ��Χ3*3���ϽǷ���x����

uint8_t score = 0; // ��Ϸ�÷�

uint16_t time_temp = 0;		   // ��ʱ�����������ڿ�����Ϸ�ٶ�
uint16_t time_temp_randon = 0; // ��ʱ��������������������������

// �������̬��ͼ
uint8_t pdata led_map[2 + 8 + 1][1 + 8 + 1] = {{0}};
// �������̬��ͼ
uint8_t xdata led_map_static[2 + 8 + 1][1 + 8 + 1] = {{0}};

// ����0-6�����
uint8_t random_num_0_6()
{
	uint8_t num = 0;
	return time_temp_randon % 7;
}

// ����0-3�����
uint8_t random_num_0_3()
{
	uint8_t num = 0;
	return time_temp_randon % 4;
}

// ���ڷ�ת 8 λ��������
uint8_t reverse_bits(uint8_t num)
{
	uint8_t reversed_num, i = 0;
	for (i = 0; i < 8; i++)
	{
		reversed_num <<= 1;		   // �����ڳ�λ��
		reversed_num |= (num & 1); // ��ȡ num �����λ���ŵ� reversed_num �����λ
		num >>= 1;				   // ���� num ׼��������һ��λ
	}
	return reversed_num;
}

// LED�ƼƷ���ʾ��P2����ʾ����8λ2������
void update_led_display(uint8_t score)
{
	uint8_t reversed_score = reverse_bits(score);
	// ��ת score �Ķ�����ֵ��0 -> 1, 1 -> 0
	// ��ΪLED���Ƶ��߼��� 0 Ϊ����1 Ϊ������Ҫ�� score �Ķ�����λȡ��
	P2 = ~reversed_score; // ��λȡ������ӦLED����ʾ�߼�
}

// ��ʱ����
void delay_10us(uint16_t ten_us)
{
	while (ten_us--)
		;
}

// ������̰���ɨ�躯��
uint8_t key_matrix_ranks_scan(void)
{
	uint8_t key_value = 0;

	KEY_MATRIX_PORT = 0xf7;		 // ����һ�и�ֵ0������ȫΪ1
	if (KEY_MATRIX_PORT != 0xf7) // �жϵ�һ�а����Ƿ���
	{
		delay_10us(1000);		 // ����
		switch (KEY_MATRIX_PORT) // �����һ�а������º�ļ�ֵ
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
		; // �ȴ������ɿ�

	KEY_MATRIX_PORT = 0xfb;		 // ���ڶ��и�ֵ0������ȫΪ1
	if (KEY_MATRIX_PORT != 0xfb) // �жϵڶ��а����Ƿ���
	{
		delay_10us(1000);		 // ����
		switch (KEY_MATRIX_PORT) // ����ڶ��а������º�ļ�ֵ
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
		; // �ȴ������ɿ�

	KEY_MATRIX_PORT = 0xfd;		 // �������и�ֵ0������ȫΪ1
	if (KEY_MATRIX_PORT != 0xfd) // �жϵ����а����Ƿ���
	{
		delay_10us(1000);		 // ����
		switch (KEY_MATRIX_PORT) // ��������а������º�ļ�ֵ
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
		; // �ȴ������ɿ�

	KEY_MATRIX_PORT = 0xfe;		 // �������и�ֵ0������ȫΪ1
	if (KEY_MATRIX_PORT != 0xfe) // �жϵ����а����Ƿ���
	{
		delay_10us(1000);		 // ����
		switch (KEY_MATRIX_PORT) // ��������а������º�ļ�ֵ
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
		; // �ȴ������ɿ�

	return key_value;
}

// ��ʱ��0ʼ����,1ms����һ���ж�
void timer0_init()
{
	TMOD |= 0x01; // ѡ��Ϊ��ʱ��0ģʽ��������ʽ1������TR0��������
	TH0 = 0xfc;	  // ����ʱ������ֵ����ʱ1ms
	TL0 = 0x18;	  // ��ʱ1ms
	ET0 = 1;	  // �򿪶�ʱ��0�ж�����
	EA = 1;		  // �����ж�
	TR0 = 1;	  // �򿪶�ʱ��
}

// 74HC595д���ݺ���
void hc595_write_data(uint8_t dat)
{
	uint8_t i = 0;
	for (i = 0; i < 8; i++) // ѭ��8�μ��ɽ�һ���ֽ�д��Ĵ�����
	{
		SER = dat >> 7; // ���ȴ���һ���ֽ��еĸ�λ
		dat <<= 1;		// ����λ�ƶ�����λ
		SRCLK = 0;
		delay_10us(1);
		SRCLK = 1;
		delay_10us(1); // ��λ�Ĵ���ʱ�������ؽ��˿���������Ĵ�����
	}
	RCLK = 1;
	delay_10us(1);
	RCLK = 0; // �洢�Ĵ���ʱ�������ؽ�ǰ��д�뵽�Ĵ������������
}

// ��ͼ��ʾ����
void show_map(uint8_t led_map[11][10])
{
	uint8_t gled_row_add_temp;
	uint8_t i, j = 0;
	for (i = 0; i < 8; i++) // ѭ��8��ɨ��8�С���
	{
		gled_row_add_temp = 0x00; // ��ʼ��Ϊȫ��
		for (j = 0; j < 8; j++)
		{
			if (led_map[j + 2][i + 1] == 1)
			{
				gled_row_add_temp += gled_row[j];
			}
		}
		LEDDZ_COL_PORT = gled_col[i];		 // ������ѡ����
		hc595_write_data(gled_row_add_temp); // ������ѡ����
		delay_10us(50);						 // ��ʱһ��ʱ�䣬�ȴ���ʾ�ȶ�
		hc595_write_data(0x00);				 // ��Ӱ
	}
}

// ���ɷ���
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

// ��Ϸ���¿�ʼ
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
	block_x = 3; // ��Χ4*4���ϽǷ���x����
	// ��յ�ͼ
	memset(led_map, 0, sizeof(led_map));
	memset(led_map_static, 0, sizeof(led_map));
	// ��ʼ����̬��ͼ�߽�
	for (n = 0; n < 10; n++)
	{
		led_map_static[n][0] = 1;
		led_map_static[n][9] = 1;
		led_map_static[10][n] = 1;
		// ��ʼ����̬��ͼ�߽�
		led_map[n][0] = 1;
		led_map[n][9] = 1;
		led_map[10][n] = 1;
	}
}

// ��Ϸ��ͣ
void pause_game()
{
	if (game_state != 2)
		game_state = 2;
	else
		game_state = 1;
}

// ��Ϸ�ٶȵ���
void game_speed_chang()
{
	game_speed_state--;
	if (game_speed_state == 15)
		game_speed_state = 4;
}

// �Ϸ����ж�
uint8_t IsLegal(uint8_t x, uint8_t y)
{
	uint8_t i, j;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			// ����������µ�λ�ñ������Ѿ��з����ˣ��򲻺Ϸ�
			if ((block[block_shape][block_rotate_state][i][j] == 1) && (led_map_static[y + i][x + j] == 1))
				return 0; // ���Ϸ�
		}
	}
	return 1; // �Ϸ�
}

// ��3*3����ķ�Χ�÷���˳ʱ����ת,���뷶Χ�ڵ�ͼ���Ͻǵ�����
void block_rotate(int block_y, int block_x)
{
	uint8_t i, j = 0;
	uint8_t block_rotate_state_old = block_rotate_state;
	// ������ת����
	block_rotate_state++;
	if (block_rotate_state == 4)
		block_rotate_state = 0;
	// �ж���ת��ķ����Ƿ�Ϸ�
	if (IsLegal(block_x, block_y))
	{
		// ���ԭ���ķ���
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

// ��ʱ��0�жϺ���
void timer0() interrupt 1
{
	static uint16_t i = 0;
	TH0 = 0xfc; // ����ʱ������ֵ����ʱ1ms
	TL0 = 0x18; // ��ʱ1ms
	i++;
	time_temp_randon++;
	if (i == 100) // 0.1s����һ������
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

// ͨ�õķ����ƶ�����
void block_move(BlockMoveDirection direction)
{
	uint8_t i, j;
	uint8_t new_x = block_x;
	uint8_t new_y = block_y;

	// ���ݷ������Ŀ������
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

	// ����������Ƿ�Ϸ�
	if (IsLegal(new_x, new_y))
	{
		// ���ԭ���ķ���
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
		// ���·��������
		block_x = new_x;
		block_y = new_y;
		// ����λ���ϻ��Ʒ���
		creatblock(block_shape, block_rotate_state, block_x, block_y);
	}
	else if (direction == MOVE_DOWN)
	{
		// ����޷����ƣ��洢����̬��ͼ
		yid = 0;
	}
}

// ��ĳ��������������һ��
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

// ��������
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
			// �����ϵ���������
			block_down_move_row(i);
			score++;
		}
		count = 0;
	}
}

// �ж��Ƿ���һ��
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
	// uart_init(0XFA); // ������Ϊ9600
	timer0_init(); // ��ʱ��0��ʼ��
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
		// ��Ϸ����
		if (game_state == 1)
		{
			// �ж��Ƿ���Ҫ���ɷ���
			if (yid == 0)
			{
				block_clear();			   // �ж���һ�У�����һ����������һ�У�ʹ���淽�����ƣ����÷ּ�һ
				update_led_display(score); // ���·���
				// ���¾�̬��ͼ,��������
				memcpy(led_map_static, led_map, sizeof(led_map));
				block_x = 3;
				block_y = 0;
				block_shape = random_num_0_6();
				block_rotate_state = random_num_0_3();
				creatblock(block_shape, block_rotate_state, 3, 0);
				yid = 1;
			}
			// ��ʱ����һ��
			if (time_temp == 0 && yid == 1)
			{
				block_move(MOVE_DOWN);
				time_temp = game_speed_state;
			};
			// �ж��Ƿ���һ��
			if (yid == 1)
			{
				block_loc_jud(); // �ж��Ƿ���һ�У�����һ������Ϸ����
			}
		};
		// ��ʾ��Ϸ��ͼ
		show_map(led_map);
	}
}
