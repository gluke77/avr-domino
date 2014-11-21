#if !defined _CODENET_INCLUDED
#define _CODENET_INCLUDED

#define CN_ESC			(0x1B)
#define CN_EOT			(0x04)
#define CN_ACK			(0x06)
#define CN_NAK			(0x15)
#define CN_ETX			(0x03)

#define CN_SW_RESET		CN_ETX
#define CN_PRINT_ID		(0x41) // 'A'


#define CN_MAX_MSG_LENGTH		(100)


#endif /* _CODENET_INCLUDED */