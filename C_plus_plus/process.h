#pragma once
#include<math.h>
#include<time.h>
#include<iostream>
#include<stdio.h>
#include<fstream>
#include "page_table.h"
#include "TLB.h"
using namespace std;
typedef int MB;
typedef long long ll;
#define mb (ll)pow(2,20)
inline ll move_bits(ll ori,int bits) {
	while (ori > 0&&bits>0) {
		ori /= 2;
		bits--;
	}
	return ori;
}
class process {
public:
	process(MB size) {
		process_id++;
		process_No = process_id;
		this->size = size;
		ll  t = mb*size/2;
		high_addr[0] = (ll)pow(2, 32)-t ;
		high_addr[1] = (ll)pow(2, 32) - 1;
		low_addr[0] = 0;
		low_addr[1] = t - 1;
		run_time = 0;
		page_fault=0;
		access_times=0;
		tlb_miss=0;
		for (int i = 0; i < 8192; i++) {
			owned_p_page[i] = 0;
		}
	}
	//获取num个随机访问地址
	void get_access_addrs(int num);
	MB get_Size() const { return size; }
	void set_Size(MB val) { size = val; }
	//只设置低位地址的末端地址和高位地址的起始地址；
	//由于低位地址的起始地址默认为0x0，高位地址的末端地址默认为0xffffffff
	ll High_addr() const { return high_addr[0]; }
	void High_addr(ll val) { high_addr[0] = val; }
	ll Low_addr() const { return low_addr[1]; }
	void Low_addr(ll val) { low_addr[1] = val; }
	//void print_access_addr();
	int Run_time() const { return run_time; }
	void Run_time(int val) { run_time = val; }
	void run();
	TLB saveTLB() {
		return tlb;
	}
	//用于进程载入TLB
	page_table p_table = page_table();
	static TLB tlb;
	int Process_No() const { return process_No; }
	void Process_No(int val) { process_No = val; }
	void print_page_table();
	void update_page_table();
private:
	int run_time;//进程运行的次数
	int process_No;//进程编号
	MB size;//记录进程所占内存大小
	ll high_addr[2];
	//addr[0]为起始地址，addr[1]为末端地址
	ll low_addr[2];
	ll access_addrs[5001];//每次随机访问的5000地址数组
	static int process_id;//随进程数增加而增加
	int owned_p_page[8192];
	//独自储存的本进程页框使用情况，用于对比、更新被回收的页框及自身的页表
	int page_fault;//缺页记录
	int access_times;//地址访问总次数记录
	int tlb_miss;//TLB的miss记录
};
//静态全局变量记录已开的进程数
int process::process_id = 0;
TLB process::tlb = TLB(16);
//共用TLB并初始化
void process::get_access_addrs(int num) {
	access_times += num;
	run_time++;
	ll cnt = 0;
	int xh = num / 20;
	srand((unsigned)time(NULL));
	//高地址寻址10次，低地址寻址10次
	for (int i = 0; i < xh; i++) {
		int mid = max(16, size );
		ll temp = rand() %(max(32,size))+mid;
		//cout << "size:" <<size<<" "<< temp << endl;
		//增强随机性，引入随机因子temp，以8k=2^13为增加单位
		access_addrs[cnt++]=rand() % low_addr[1]+size*temp*pow(2,14);
		for (int j = 0; j < 9; j++) {
			access_addrs[cnt++] = access_addrs[cnt - 1] + 1;
		}
		ll t2 = rand() % (high_addr[1] - high_addr[0]) + high_addr[0];
		temp = size*temp*pow(2,13)+t2;
		//进行溢出检测及处理
		while (temp>pow(2,32)) {
			temp /= 2;
		}
		while (temp>Low_addr()&&temp<High_addr()) {
			temp /= 2;
		}
		access_addrs[cnt++] =temp;
		for (int j = 0; j < 9; j++) {
			access_addrs[cnt++] = access_addrs[cnt - 1] + 1;
		}
	}
}

void process::update_page_table() {
	for (int i = 0; i < 8192; i++) {
		if (owned_p_page[i] == process_No&&p_table.physical_pframes[i] != process_No) {
			//说明页框已被别的进程占用，需要更新页表
			for (int j = 0; j < 64; j++) {
				if (p_table.first_page[j] == 1) {
					//一级页表已载入的才更新
					for (int k = 0; k < 8192; k++) {
						if (p_table.p_table[j][k] == i) {
							p_table.p_table[j][k] = -1;
							//该虚拟地址下次访问的时候需要重寻页框
						}
					}
				}
			}
		}
	}
}

void process::run() {
	update_page_table();
	//进行页表的更新
	char id[20];
	sprintf_s(id, "visit_seq_%d.txt", process_No);
	ofstream outfile(id,ios::app);
	//已切换运行次数含本次运行
	cout<< "-----进程信息-----编号-----"<<process_No<<"-----内存：" << get_Size() << "M-----" << "运行结束-----已切换运行次数：" << run_time << "-----" << endl;
	outfile << "-----进程信息-----内存：" << get_Size() << "M-----" << "已切换运行次数：" << run_time <<"-----"<< endl << "-----500次随机地址访问如下-----"<<endl;
	outfile << "Address       VP number    PF number    TLB    PageTable\n";
	char addr_str[20],v_page[20];
	for (int i = 0; i < 500; i++) {
		ll t = move_bits(access_addrs[i], 13);
		int tlb_result=tlb.find_Ppage(t),p_table_result=-2;
		char *TLB_hit = "hit";
		char *p_table_hit = "hit";
		char LRU_frames[100];
		//页框全部用完需要回收，LRU_frames为LRU规则回收页框的提示语句
		ll p_page;
		if (tlb_result == TLB_miss) {
			p_table_result = p_table.find_Ppage(t);
			p_page = p_table.get_p_page();
			if (p_table_result != -1) {
				p_page = p_table_result;
			}
			tlb.add_TLB_item(t, p_page);
		}
		else {
			p_page = tlb.find_Ppage(t);
		}
		if (tlb_result == -1) {
			TLB_hit = "miss";
			tlb_miss++;
		}
		if (p_table_result == -1) {
			p_table_hit = "miss";
			page_fault++;
			if (p_table.physical_pframes[p_page] != 0) {
				sprintf_s(LRU_frames, "-----已无可用页框，将下面最先分配的页框：%lld从进程id：%d回收，将更新该进程的页表-----", p_page,p_table.physical_pframes[p_page]);
				outfile << LRU_frames << endl;
			}
			p_table.physical_pframes[p_page] = process_No;
			owned_p_page[p_page] = process_No;
			//已用了系统的页框，记录在自身的存储对照表中
		}
		//cout << t << endl;
		sprintf_s(addr_str,"0x%08x",access_addrs[i]);
		//sprintf_s(v_page, "0x%05x", t);
		outfile << addr_str <<"\t"<<t<<"\t      "<<p_page<<"\t       "<<TLB_hit<<"\t"<<p_table_hit<< '\n';
		//cout << access_addrs[i] << endl;
	}
	outfile.close();
}
//进程运行时载入系统TLB
//此过程已不需要 TLB为所有进程所共有

void process::print_page_table() {
	char id[20];
	sprintf_s(id, "page_table_%d.txt", process_No);
	ofstream outfile(id, ios::app);
	outfile << "-----[只显示被访问和载入/更新的页表项，不显示实页偏移量]-----" << endl;
	for (int i = 0; i < 64; i++) {
		if (p_table.first_page[i] == 1) {
			outfile << "-----[一级页表]-----" << "-----第" << i << "页-----" << endl;
			outfile << "-----[二级页表]-----虚页项-----实页项-----" << endl;
			for (int j = 0; j < 8192; j++) {
				if (p_table.p_table[i][j] != -1) {
					ll v_page = i * 8192 + j;
					outfile << "-------------------" << v_page << "-----" << p_table.p_table[i][j] << endl;
				}
			}
		}
	}
	outfile << "\n-----Page Fault Rate-----" << (double)page_fault / (double)access_times *100.0<<"%"<< endl;
	outfile << "\n-----TLB Miss Rate-----" << (double)tlb_miss / (double)access_times *100.0 << "%" << endl;
	outfile.close();
}
//int binary_search(ll v_page,ll page_table[]) {
//	int l = 0; 
//	int r = 524287;//虚页面最多有524288个
//	int mid = (l + r) / 2;
//	while (l != r) {
//		if (page_table[mid] == v_page)
//			return mid;
//		if (page_table[mid] < v_page)
//			l = mid + 1;
//		else
//			r = mid;
//	}
//	return -1;
//}