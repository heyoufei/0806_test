#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum __bool { FALSE = 0, TRUE = 1, } bool;
int OpenDev(char *Dev);
void set_speed(int fd, int speed);
int set_Parity(int fd,int databits,int stopbits,int parity);

int main(void)
{
	int fd;
	int nwrite,i,j, nread;
	char buff1[10] = "AT+CMGF=1\r";
	char buff2[22] = "AT+CMGS=\"15757821531\"\r";
	char buff3[1000] = "mm";
	char buff_file[1000];
	FILE *fp;	
	FILE *fp1;
	char str_phone_num[50]="15757821531";  		

	char read_Buff[4096];
	char read_Buff2[4096];
	fd_set rd;
	int flag =0 ;
	int flag1 = 0;

	int sum =0;
	int count = 0;
	int k =0;
	int len_str = 0;
	char *dev  = "/dev/ttyUSB0";

	time_t curtime;
	char chtime[50];

	int a1, a2, a3, a4, mm;

	fd = 0;
	k = 0;
	fp = fopen("/mnt/nfs/2.txt", "r");
	fp1 = fopen("/mnt/nfs/3.txt", "a+");
	while(!feof(fp))
	{
		buff_file[k] = fgetc(fp);
		printf("%c", buff_file[k]);
		k++;
	}
	printf("\nk = %d\n", k);
	fclose(fp);
	for(i = 0; i < k-2; i++)
	{
		buff3[i+30] = buff_file[i];
	}
	buff3[i+30] = 26;
	len_str = i+31;
	/*打开串口*/
	fd = OpenDev(dev);
	set_speed(fd,115200);
	if (set_Parity(fd,8,1,'N') == FALSE)
	{
		printf("Set Parity Error\n");
		exit (0);
	}
	while(1)
	{ 
		printf("input the phone number:");
		scanf("%s", str_phone_num);
		if(strlen(str_phone_num) == 11)
		{
			break;
		}
		else
		{
			printf("please input right phone number!\n");
		}
	}
	for(i = 9; i < 9+11; i++)
	{
		buff2[i] = str_phone_num[i-9];
	}
	printf("input the message count:");
	scanf("%d", &sum);

	count = 0;
	for(i = 0; i < sum; i++)
	{
		a1 = i/1000;
	 	a2 = i%1000/100;
		a3 = i%100/10;
		a4 = i%10;
		buff3[0]= a1 +'0';
		buff3[1] = a2 + '0';
		buff3[2] = a3 + '0';
		buff3[3] = a4 + '0';
		buff3[4] = ':';
//		buff3[0]='0'+i/10;
//		buff3[1]='0'+i%10; 
		printf("the %d test\n", i);
		printf("fd =%d\n",fd);
		sleep(1);  
		nwrite = write(fd,buff1,strlen(buff1));
		sleep(1);
		
		

		nwrite = write(fd,buff2,strlen(buff2));
		sleep(1);
		time(&curtime);
		strcpy(chtime, ctime(&curtime));
		for(mm = 0; mm < 24; mm++)
		{
			buff3[5+mm] = chtime[mm];
		}
		buff3[5+mm] = 10;
		
		nwrite = write(fd,buff3,len_str);
		sleep(1);



		nread = read(fd, read_Buff, 4096);
		for(j = 0; j < nread; j++)
		{
			printf("%c", read_Buff[j]);

		}
		/*for(j = 0; j < nread; j++)
		  {
		  printf("the %d char is  %c = %x\n", j, read_Buff[j], read_Buff[j]);
		  }*/
		flag = 0;
		flag1 = 0;
		for(j = 0; j < nread ; j++)
		{
			if(read_Buff[j] == '+' && read_Buff[j+1] == 'C' && read_Buff[j+2] == 'M' && read_Buff[j+3] == 'G' && read_Buff[j+4] == 'S' && read_Buff[j+5] == ':')
			{
				flag = 1;
				break;
			}

		}
		if(flag == 1)
		{
			for(; j < nread; j++)
			{
				if(read_Buff[j] == 'O' && read_Buff[j+1] == 'K')
				{
					flag1 = 1;
					count ++;
					break;
				}
			}
		}
		else
		{
			read_Buff2[0] = a1+'0';
			read_Buff2[1] = a2 + '0';
			read_Buff2[2] = a3 + '0';
			read_Buff2[3] = a4 +'0';
			read_Buff2[4] = ':';
			for(mm = 0; mm < nread; mm++)
			{
				read_Buff2[mm+5] = read_Buff[mm]; 
			}
			fwrite(read_Buff2, nread+5, 1, fp1);		
		}
		printf("\n");
		printf("the nread = %d\n", nread);
		sleep(55);
	}
	close(fd);
	fclose(fp1);
	printf("the success send is %d, the fail send is %d\n", count , sum - count);
	return (1);
}

int set_Parity(int fd,int databits,int stopbits,int parity)
{ 
	struct termios options; 
	if  ( tcgetattr( fd,&options)  !=  0) { 
		perror("SetupSerial 1\n");     
		return(FALSE);  
	}
	options.c_cflag &= ~CSIZE; 
	switch (databits) 
	{   
		case 7: options.c_cflag |= CS7; break;
		case 8:    options.c_cflag |= CS8;    break;   
		default: fprintf(stderr,"Unsupported data size\n"); return (FALSE);  
	}                           
	switch (parity) 
	{   
		case 'n':
		case 'N': options.c_cflag &= ~PARENB;   
			  options.c_iflag &= ~INPCK;     break;  
		case 'o':   
		case 'O': options.c_cflag |= (PARODD | PARENB);  
			  options.c_iflag |= INPCK; break;  
		case 'e':  
		case 'E': options.c_cflag |= PARENB;      
			  options.c_cflag &= ~PARODD;    
			  options.c_iflag |= INPCK;  break;
		case 'S': 
		case 's': options.c_cflag &= ~PARENB;
			  options.c_cflag &= ~CSTOPB;break;  
		default: fprintf(stderr,"Unsupported parity\n");    
			 return (FALSE);  
	}  
	switch (stopbits)
	{   
		case 1: options.c_cflag &= ~CSTOPB;    break;  
		case 2:    options.c_cflag |= CSTOPB;  break;
		default: fprintf(stderr,"Unsupported stop bits\n");  
			 return (FALSE); 
	} 
	if (parity != 'n')   
		options.c_iflag |= INPCK; 
	tcflush(fd,TCIFLUSH);
	options.c_cc[VTIME] = 40; 
	options.c_cc[VMIN] = 196;
	if (tcsetattr(fd,TCSANOW,&options) != 0)   
	{ 
		perror("SetupSerial 3\n");   
		return (FALSE);  
	} 
	return (TRUE);  
}
void set_speed(int fd, int speed)
{
	int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300};
	int name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200,  300};
	int   i; 
	int   status; 
	struct termios   Opt;
	tcgetattr(fd, &Opt); 
	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) { 
		if  (speed == name_arr[i]) {     
			tcflush(fd, TCIOFLUSH);     
			cfsetispeed(&Opt, speed_arr[i]);  
			cfsetospeed(&Opt, speed_arr[i]);   
			status = tcsetattr(fd, TCSANOW, &Opt);  
			if  (status != 0) {        
				perror("tcsetattr fd failed!\n");  
				return;     
			}    
			tcflush(fd,TCIOFLUSH);   
		}  
	}
}

int OpenDev(char *Dev)
{
	int fd = open( Dev, O_RDWR );         
	if (-1 == fd)   
	{           
		perror("Can't Open Serial Port");
		return -1;       
	}
	else
	{
		return fd;
	}
}
