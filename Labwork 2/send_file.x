
program SEND_FILE_PROG{
	version SEND_FILE_VER{
		int SEND_NAME(string) = 1;
		int SEND_SIZE(long int) = 2;
		int SEND_BUFFER(string) = 3;
	} = 1;
} = 0x23451110;
