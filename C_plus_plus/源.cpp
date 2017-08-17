#include<iostream>
#include<stdio.h>
#include "process.h"
using namespace std;
int main(int argc,char *argv[]) {
	//ll page_frames_avail[2500];//用于记录可用实页框
	//上面已封装至process的类静态全局变量中，与TLB同
	process p1(1);
	process p2(32);
	process p3(64);
	process p4(128);
	process p5(256);
	//再开n个内存为256M的进程用于测试，此参数可以进行修改
	process pp[5] = { 256,256,256,256,256};
	printf("[1M]-----低位地址-----0x0-----0x%08x\n", p1.Low_addr());
	printf("[1M]-----高位地址-----0x%08x-----0xffffffff\n", p1.High_addr());
	printf("[32M]-----低位地址-----0x0-----0x%08x\n", p2.Low_addr());
	printf("[32M]-----高位地址-----0x%08x-----0xffffffff\n", p2.High_addr());
	printf("[64M]-----低位地址-----0x0-----0x%08x\n", p3.Low_addr());
	printf("[64M]-----高位地址-----0x%08x-----0xffffffff\n", p3.High_addr());
	printf("[128M]-----低位地址-----0x0-----0x%08x\n", p4.Low_addr());
	printf("[128M]-----高位地址-----0x%08x-----0xffffffff\n", p4.High_addr());
	printf("[256M]-----低位地址-----0x0-----0x%08x\n", p5.Low_addr());
	printf("[256M]-----高位地址-----0x%08x-----0xffffffff\n", p5.High_addr());
	cout << "\n-----转换为十进制地址-----" << endl;
	printf("[1M]-----低位地址-----0-----%lld\n", p1.Low_addr());
	printf("[1M]-----高位地址-----%lld-----4294967296\n", p1.High_addr());
	printf("[32M]-----低位地址-----0-----%lld\n", p2.Low_addr());
	printf("[32M]-----高位地址-----%lld-----4294967296\n", p2.High_addr());
	printf("[64M]-----低位地址-----0-----%lld\n", p3.Low_addr());
	printf("[64M]-----高位地址-----%lld-----4294967296\n", p3.High_addr());
	printf("[128M]-----低位地址-----0-----%lld\n", p4.Low_addr());
	printf("[128M]-----高位地址-----%lld-----4294967296\n", p4.High_addr());
	printf("[256M]-----低位地址-----0-----%lld\n", p5.Low_addr());
	printf("[256M]-----高位地址-----%lld-----4294967296\n", p5.High_addr());
	int cnt = 20;
	while (cnt--) {
		//cout<<p1.tlb.Size()<<endl;
		p1.get_access_addrs(5000);//获取5000个随机访问地址
		p1.run();
		//run的过程包括载入TLB、对比并更新page_table、搜寻并载入系统页框、输出访问结果、保存系统TLB
		//所有过程封装至类process里
		p2.get_access_addrs(5000);
		p2.run();
		p3.get_access_addrs(5000);
		p3.run();
		p4.get_access_addrs(5000);
		p4.run();
		p5.get_access_addrs(5000);
		p5.run();
		//对应上面的进程对象数组大小进行修改
	for (int i = 0; i < 5; i++) {
			pp[i].get_access_addrs(5000);
			pp[i].run();
		}
	}
	//输出每个进程对应的页表
	p1.print_page_table();
	p2.print_page_table();
	p3.print_page_table();
	p4.print_page_table();
	p5.print_page_table();
	//对应上面的进程对象数组大小进行修改
	for (int i = 0; i < 5; i++) {
		pp[i].print_page_table();
	}
	//输出最终TLB
	system("pause");
}