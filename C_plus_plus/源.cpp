#include<iostream>
#include<stdio.h>
#include "process.h"
using namespace std;
int main(int argc,char *argv[]) {
	//ll page_frames_avail[2500];//���ڼ�¼����ʵҳ��
	//�����ѷ�װ��process���ྲ̬ȫ�ֱ����У���TLBͬ
	process p1(1);
	process p2(32);
	process p3(64);
	process p4(128);
	process p5(256);
	//�ٿ�n���ڴ�Ϊ256M�Ľ������ڲ��ԣ��˲������Խ����޸�
	process pp[5] = { 256,256,256,256,256};
	printf("[1M]-----��λ��ַ-----0x0-----0x%08x\n", p1.Low_addr());
	printf("[1M]-----��λ��ַ-----0x%08x-----0xffffffff\n", p1.High_addr());
	printf("[32M]-----��λ��ַ-----0x0-----0x%08x\n", p2.Low_addr());
	printf("[32M]-----��λ��ַ-----0x%08x-----0xffffffff\n", p2.High_addr());
	printf("[64M]-----��λ��ַ-----0x0-----0x%08x\n", p3.Low_addr());
	printf("[64M]-----��λ��ַ-----0x%08x-----0xffffffff\n", p3.High_addr());
	printf("[128M]-----��λ��ַ-----0x0-----0x%08x\n", p4.Low_addr());
	printf("[128M]-----��λ��ַ-----0x%08x-----0xffffffff\n", p4.High_addr());
	printf("[256M]-----��λ��ַ-----0x0-----0x%08x\n", p5.Low_addr());
	printf("[256M]-----��λ��ַ-----0x%08x-----0xffffffff\n", p5.High_addr());
	cout << "\n-----ת��Ϊʮ���Ƶ�ַ-----" << endl;
	printf("[1M]-----��λ��ַ-----0-----%lld\n", p1.Low_addr());
	printf("[1M]-----��λ��ַ-----%lld-----4294967296\n", p1.High_addr());
	printf("[32M]-----��λ��ַ-----0-----%lld\n", p2.Low_addr());
	printf("[32M]-----��λ��ַ-----%lld-----4294967296\n", p2.High_addr());
	printf("[64M]-----��λ��ַ-----0-----%lld\n", p3.Low_addr());
	printf("[64M]-----��λ��ַ-----%lld-----4294967296\n", p3.High_addr());
	printf("[128M]-----��λ��ַ-----0-----%lld\n", p4.Low_addr());
	printf("[128M]-----��λ��ַ-----%lld-----4294967296\n", p4.High_addr());
	printf("[256M]-----��λ��ַ-----0-----%lld\n", p5.Low_addr());
	printf("[256M]-----��λ��ַ-----%lld-----4294967296\n", p5.High_addr());
	int cnt = 20;
	while (cnt--) {
		//cout<<p1.tlb.Size()<<endl;
		p1.get_access_addrs(5000);//��ȡ5000��������ʵ�ַ
		p1.run();
		//run�Ĺ��̰�������TLB���ԱȲ�����page_table����Ѱ������ϵͳҳ��������ʽ��������ϵͳTLB
		//���й��̷�װ����process��
		p2.get_access_addrs(5000);
		p2.run();
		p3.get_access_addrs(5000);
		p3.run();
		p4.get_access_addrs(5000);
		p4.run();
		p5.get_access_addrs(5000);
		p5.run();
		//��Ӧ����Ľ��̶��������С�����޸�
	for (int i = 0; i < 5; i++) {
			pp[i].get_access_addrs(5000);
			pp[i].run();
		}
	}
	//���ÿ�����̶�Ӧ��ҳ��
	p1.print_page_table();
	p2.print_page_table();
	p3.print_page_table();
	p4.print_page_table();
	p5.print_page_table();
	//��Ӧ����Ľ��̶��������С�����޸�
	for (int i = 0; i < 5; i++) {
		pp[i].print_page_table();
	}
	//�������TLB
	system("pause");
}