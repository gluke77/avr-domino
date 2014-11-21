#include "codenet.h"
#include "usart.h"
#include "common.h"

int codenet_continious_printing(int head, int on, int pitch)
{
	char	buf[CN_MAX_MSG_LENGTH];
	int		idx = 0;
	int		param; 
	
	param = pitch % 10000;
	
	buf[idx++] = CN_ESC;
	buf[idx++] = '[';
	buf[idx++] = '0' + head;
	buf[idx++] = (on)?'Y':'N';
	buf[idx++] = '0' + param / 1000;
	param %= 1000;
	buf[idx++] = '0' + param / 100;
	param %= 100;
	buf[idx++] = '0' + param / 10;
	param %= 10;
	buf[idx++] = '0' + param;
	buf[idx++] = CN_EOT;
	buf[idx++] = 0;

	if ((res = usart1_cmd(buf, buf, 50, 500)) != RESULT_OK)
		return -1;
		
	if (buf[0] != CN_ACK);
		return -2;
		
	retutn 1;
}

int	codenet_message_to_head_assignment(int head, int msg)
{
	char	buf[CN_MAX_MSG_LENGTH];
	int		idx = 0;
	int		param;
	
	param = msg % 1000;
	
	buf[idx++] = CN_ESC;
	buf[idx++] = 'P';
	buf[idx++] = '0' + head;
	if ('?' == msg)
		buf[idx++] = '?';
	else
	{
		buf[idx++] = '0' + param / 100;
		param %= 100;
		buf[idx++] = '0' + param / 10;
		param %= 10;
		buf[idx++] = '0' + param;
	}
	buf[idx++] = CN_EOT;
	buf[idx++] = 0;

	if ((res = usart1_cmd(buf, buf, 50, 500)) != RESULT_OK)
		return -1;
	
	if ('?' == msg)
	{
		if (buf[0] != CN_ESC)
			return -2;
		else
			return (buf[3]-'0') * 100 + (buf[4] - '0') * 10 + (buf[5] - '0');
	}
	else
	{
		if (buf[0] != CN_ACK)
			return -2;
		else
			return 1;
	}
}

int codenet_head_enable(int head, int on)
{
	char	buf[CN_MAX_MSG_LENGTH];
	int		idx = 0;
	
	buf[idx++] = CN_ESC;
	buf[idx++] = 'Q';
	buf[idx++] = '0' + head;
	if ('?' == msg)
		buf[idx++] = '?';
	else
		buf = (on)?'Y':'N';
	buf[idx++] = CN_EOT;
	buf[idx++] = 0;

	if ((res = usart1_cmd(buf, buf, 50, 500)) != RESULT_OK)
		return -1;
	
	if ('?' == msg)
	{
		if (buf[0] != CN_ESC)
			return -2;
		else
			return (buf[3] == 'Y')?1:0;
	}
	else
	{
		if (buf[0] != CN_ACK)
			return -2;
		else
			return 1;
	}
}

int codenet_product_counting(int req)
{
	char	buf[CN_MAX_MSG_LENGTH];
	int		idx = 0;
	
	buf[idx++] = CN_ESC;
	buf[idx++] = 'T';
	buf[idx++] = '1';
	if ('?' == msg)
		buf[idx++] = '?';
	else
		buf[idx++] = '0';
	buf[idx++] = CN_EOT;
	buf[idx++] = 0;

	if ((res = usart1_cmd(buf, buf, 50, 500)) != RESULT_OK)
		return -1;
	
	if ('?' == msg)
	{
		if (buf[0] != CN_ESC)
			return -2;
		else
			return (buf[3]-'0') * 100000 + 
				(buf[4] - '0') * 10000 + 
				(buf[5] - '0') * 1000 +
				(buf[6] - '0') * 100 + 
				(buf[7] - '0') * 10 + 
				(buf[8] - '0');
	}
	else
	{
		if (buf[0] != CN_ACK)
			return -2;
		else
			return 1;
	}
}
