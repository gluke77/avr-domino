#if !defined _CODENET_INCLUDED
#define _CODENET_INCLUDED

#define CN_ESC			(0x1B)
#define CN_EOT			(0x04)
#define CN_ACK			(0x06)
#define CN_NAK			(0x15)
#define CN_ETX			(0x03)

#define CN_SW_RESET		CN_ETX
#define CN_PRINT_ID		(0x41) // 'A'

#define CODENET_DELAY	(100)
#define CODENET_TIMEOUT	(10)


#define CN_MAX_MSG_LENGTH		(100)

int codenet_continious_printing(int /* head */, int /* on */, int /* pitch */);
int	codenet_message_to_head_assignment(int /* head */, int /* msg */);
int codenet_head_enable(int /* head */, int /* on */);
long codenet_product_counting(int /* req */);

#endif /* _CODENET_INCLUDED */