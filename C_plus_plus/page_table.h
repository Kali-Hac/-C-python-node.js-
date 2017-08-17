#pragma once
#include "process.h"
#include<queue>
using namespace std;
#define p_table_miss -1
typedef int MB;
typedef long long ll;
//32位虚拟地址、每个页面8kb对应的虚页面有524288个
//实际64M内存页框数8192个
/*int physical_pframes[2500];*///用于记录物理内存页面是否存入页表
class page_table {
public:
	//虚页面数默认为524288
	page_table(int num=524288) {
		for (int i = 0; i < 64; i++) {
			first_page[i] = 0;//一级页面未载入，初始化标记为0
		}
		//for (int i = 0; i < num; i++) {
		//	p_table[i] = -1;
		//}
		for (int i = 0; i < 8192; i++) {
			physical_pframes[i] = 0;
		}
		//while (!allocate_ord.empty()) {
		//	allocate_ord.pop();
		//}
		v_page_seek = 0;
	}
	static int physical_pframes[8192];//用于记录物理内存页框是否存入页表，且进行进程占用标记
	static int LRU_flag;//用于标记最先分配出去的页框
	int first_page[64];//判断一级页面是否已载入
	//此处二级页表不考虑页面权限位的占用,若要考虑则可减少设定为4096
	ll **p_table = new ll*[64];//对应二级页表结构，页目录(外层页号)有64个，每个页表项(外层页内地址)有8192个(对应8k的偏移量)
	ll find_Ppage(ll Vpage);
	ll get_p_page() { return v_p_reflect[1]; }
	//void copy_Pframes_available(ll Pframes_available[]);//将系统中内存页框可用信息导出
	//void get_Pframes_available(ll Pframes_available[]);//将系统中内存页框可用信息载入
private:
	//ll p_table[524288];//对应一级页表结构
	//此处二级页表不考虑页面权限位的占用,若要考虑则可减少设定为4096
	//queue<long long>allocate_ord;
	int v_page_seek;
	ll v_p_reflect[2];
};
//静态全局变量初始化
//包括可用页框标记和最先分配的页框标记
int page_table::physical_pframes[8192] = { 0 };
int page_table::LRU_flag = 0;

int seek_page_frames(int &v_page_seek) {
	int cnt = 0;
	while (page_table::physical_pframes[v_page_seek] != 0 && v_page_seek < 8192) {
		v_page_seek++;
		cnt++;
		v_page_seek %= 8192;
		if (cnt == 8192) {
			return cnt = -1;
			break;
		}//找了一个回合找不到可分配的页框,即所有页框已被分配，此时需要回收
	}
	return v_page_seek;
}

ll page_table::find_Ppage(ll Vpage) {
	//先定位对应的一级页面以及二级页面(一级页面地址偏移量)
	int first_p = Vpage / 8192;
	int second_p = Vpage % 8192;
	int Available_frame=-1;
	v_p_reflect[0] = Vpage;
	//在页表找不到对应实页项，则从内存中读取并放入页表+TLB
	if (first_page[first_p] == 1) {
		if (p_table[first_p][second_p] == -1) {
			Available_frame = seek_page_frames(v_page_seek);
			if (Available_frame == -1) {
				//设置第一个被分配的页框为0页框，然后在进程中更新已占用0页框的页表
				//将其标记为改进程占用，然后在各自维护的私人页框占用表中进行比对更新
				p_table[first_p][second_p] = LRU_flag;
				v_p_reflect[1] = LRU_flag;
				LRU_flag = (LRU_flag + 1) % 8192;
				//把页表中最先分配的页框回收重复利用，此处采用队列保存先后顺序并取出
				//但实际上是错的 因为同一进程占用的页框不能回收！！
					//ll v_page = allocate_ord.front();
					//int pre_first = v_page / 8192;
					//int pre_second = v_page % 8192;
					//p_table[first_p][second_p] = p_table[pre_first][pre_second];
					//v_p_reflect[1] = p_table[first_p][second_p];
					//allocate_ord.push(Vpage);
					//allocate_ord.pop();
					//p_table[pre_first][pre_second] = -1;
				
				//最先分配页框的虚页面此时已经被回收页框，要初始化为-1
			}
			else {
				//将页框标记为已用
				physical_pframes[v_page_seek] = 1;
				v_p_reflect[1] = v_page_seek;
				p_table[first_p][second_p] = v_page_seek++;
				//allocate_ord.push(Vpage);
			}
			return p_table_miss;
		}
		else {
			v_p_reflect[1] = p_table[first_p][second_p];
			return p_table[first_p][second_p];
		}
	}
	else {
		p_table[first_p] = new ll[8192];
		for (int i = 0; i < 8192; i++)
			p_table[first_p][i] = -1;
		first_page[first_p] = 1;
		Available_frame = seek_page_frames(v_page_seek);
		if (Available_frame == -1) {
			p_table[first_p][second_p] = LRU_flag;
			v_p_reflect[1] = LRU_flag;
			LRU_flag = (LRU_flag + 1) % 8192;
			//把页表中最先分配的页框回收重复利用，此处采用队列保存先后顺序并取出
			//但实际上是错的 因为同一进程占用的页框不能回收！！
			/*ll v_page = allocate_ord.front();
			int pre_first = v_page / 8192;
			int pre_second = v_page % 8192;
			p_table[first_p][second_p] = p_table[pre_first][pre_second];
			v_p_reflect[1] = p_table[first_p][second_p];
			allocate_ord.push(Vpage);
			allocate_ord.pop();
			p_table[pre_first][pre_second] = -1;*/
			//最先分配页框的虚页面此时已经被回收页框，要初始化为-1
		}
		else {
			//将页框标记为已用
			physical_pframes[v_page_seek] = 1;
			//在process.h中再进一步标记为进程号
			v_p_reflect[1] = v_page_seek;
			p_table[first_p][second_p] = v_page_seek++;
			//allocate_ord.push(Vpage);
		}
		return p_table_miss;
	}
}

//void page_table:: copy_Pframes_available(ll Pframes_available[]) {
//	for (int i = 0; i < 2500; i++)
//		Pframes_available[i] = physical_pframes[i];
//}
//void page_table::get_Pframes_available(ll Pframes_available[]) {
//	for (int i = 0; i < 2500; i++)
//		physical_pframes[i] = Pframes_available[i];
//}