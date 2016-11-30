//IPアドレスを偽装してIPパケットを送ることに成功しました。
// 下の画像で100.100.100.100のIPアドレスから200.200.200.200に向かって
// IPプロトコルのパケットが送られたことになっているのが確認できます
// （実際には192.168.0.20から192.168.0.2へ送っています）。
//　・Windows上でコンパイル・実行する場合はCygwin等で行ってください（このソースはLinux用です）。
// ・RAWソケットを使っているので実行時に権限のエラーに抵触するかもです。
// 現在はフラグメントに対応していません＾＾
//（http://d.hatena.ne.jp/KAINS/20071112より引用）

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <netdb.h>

#include <unistd.h>

#include <string.h>

union IpHeader {

	unsigned char c[20];
	unsigned short s[10];
	unsigned int i[5];

};

int rawsock(); unsigned short checksum(unsigned short*); 
void iptoint(char*, int*); void sends(char*, char*, int, char*); 
void printiph(IpHeader);

extern int errno;

int main() {

	char srcIP = "";
	char destIP = "";
	char prot = "";
	char realdestIP = "";
	char YorN = "";


	printf("--------START\n");
	
	printf("（偽装したい）送信元IPアドレスは？\n");
	scanf("%s\n", &srcIP);
	printf("（偽装したい）宛先IPアドレスは？\n");
	scanf("%s\n", &destIP);
	printf("通信プロトコルの種類は？？(def=0)\n");
	scanf("%s\n", &prot);
	printf("宛先IPアドレスは？\n");
	scanf("%s\n", &realdestIP);

	printf("入力内容はこれでよろしいですか？\n");
	printf("偽装送信元IP　・・・%s\n", srcIP);
	printf("偽装宛先IP　　・・・%s\n", destIP);
	printf("通信プロトコル・・・%s\n", prot);
	printf("宛先IPアドレス・・・%s\n", realdestIP);
	printf("YもしくはN");
	scanf("%s\n", YorN);

	if(YorN == "Y"){
//		sends("100.100.100.100", "200.200.200.200", 0, "192.168.0.2");
		sends(srcIP, destIP, prot, realdestIP);
	}
	if(YorN == "N"){
		printf("もう一度起動しなおしてください\n");
	}
	else{
		printf("YかN以外が入力されたため終了します。\n");
	}

	printf("\n--------END\n");
	return 0;

}

void printiph(IpHeader iph) {

	printf("\tVerionAndLength: %d\n", *iph.c);
	printf("\tTOS: %d\n", *(iph.c+1));
	printf("\tDatagramLength: %d\n", *(iph.s+1));
	printf("\tID: %X\n", *(iph.s+2));
	printf("\tFlag&Offset: %X\n", *(iph.s+3));
	printf("\tTTL: %d\n", *(iph.c+8));
	printf("\tProtocol: %d\n", *(iph.c+9));
	printf("\tChecksum: %X\n", *(iph.s+5));
	printf("\tSrcIP: %d.%d.%d.%d\n", *(iph.c+12), *(iph.c+13), *(iph.c+14), *(iph.c+15));
	printf("\tDestIP: %d.%d.%d.%d\n", *(iph.c+16), *(iph.c+17), *(iph.c+18), *(iph.c+19));

}

int rawsock() {

	int sock, on = 1;
	sock = socket(PF_INET, SOCK_RAW, IPPROTO_RAW);
	if (sock < 0) {
		perror("socket()");
		exit(1);
	}
	if ((setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on))) <0) {
		perror("setsockopt()");
		exit(1);
	}
	return sock;

}

void sends(char *srcaddress, char *destaddress, int protocol, char *realdestip) {

	IpHeader iph;
	srand(1686);
	*iph.c = 69;	//バージョンとヘッダ長（1オクテット）
	*(iph.c+1) = 0;	//TOS（１オクテット）
	*(iph.s+1) = 20;	//データグラム長（２オクテット）
	*(iph.s+2) = rand()%65536;	//ID（２オクテット）
	*(iph.s+3) = 0;		//フラグメント(２オクテット)
	*(iph.c+8) = 64;	//TTL（１オクテット）
	*(iph.c+9) = (unsigned char)protocol;	//プロトコル番号（１オクテット）
	*(iph.s+5) = 0;	//チェックサム（２オクテット）
	int srcip[4] = {}, destip[4] = {};
	iptoint(srcaddress, srcip);
	iptoint(destaddress, destip);
	*(iph.c+12) = *srcip;	//srcIP（４オクテット）
	*(iph.c+13) = *(srcip+1);
	*(iph.c+14) = *(srcip+2);
	*(iph.c+15) = *(srcip+3);
	*(iph.c+16) = *destip;	//destIP（４オクテット）
	*(iph.c+17) = *(destip+1);
	*(iph.c+18) = *(destip+2);
	*(iph.c+19) = *(destip+3);
	*(iph.s+5) = checksum(iph.s);
	printiph(iph);
	printf("\t(realdestip: %s)\n", realdestip);

	int sock = rawsock();
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(realdestip);
	addr.sin_port = 0;
	int err = sendto(sock, iph.c, 20, 0, (struct sockaddr *) &addr, sizeof(addr));
	if (err < 1) {
		perror("sendto()");
		exit(1);
	}

}

unsigned short checksum(unsigned short *c) {

	unsigned int checksum = 0x0;
	for (int i=0; i<10; i++) {
		checksum += *(c+i);
		if (checksum > 0xFFFF) {
			checksum -= 0x10000;
			checksum += 0x0001;
		}
	}
	return (~(checksum) & 0xFFFF);

}

void iptoint(char *ip, int *result) {

	int n[3], pos=0;
	char buf[4][4] = {};
	for (int i=0; *(ip+i) != '\0'; i++) {
		if (*(ip+i) == '.') {
			*(n+pos) = i;
			pos++;
		}
	}
	memcpy(buf, ip, *(n+1) - *n - 1);
	memcpy((buf+1), (ip + *n + 1), *(n+1) - *n - 1);
	memcpy((buf+2), (ip + *(n+1) + 1), *(n+2) - *(n+1) - 1);	
	memcpy((buf+3), (ip + *(n+2) + 1), strlen(ip) - *(n+2) - 1);
	*result = atoi(*buf);
	*(result+1) = atoi(*(buf+1));
	*(result+2) = atoi(*(buf+2));
	*(result+3) = atoi(*(buf+3));

}
