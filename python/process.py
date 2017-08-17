#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
☆*°☆*°(∩^o^)~━━  2017/8/15 12:40        
      (ˉ▽￣～) ~~ 一捆好葱 (*˙︶˙*)☆*°
      Fuction：(虚拟)进程的类的构造 √ ━━━━━☆*°☆*°
"""
import random
from page_table import page_table
from TLB import TLB
m=pow(2,20)
top=pow(2,32)
class process(object):
	process_number=0
	tlb=TLB()
	__slots__=('_size','_process_No','_run_times','_low_addr_h','_high_addr_l','_accadds','_p_table','_p_table_miss','_TLB_miss')#使用_name表示是私有变量但要尽量避免被随意访问
	def __init__(self,size=256):
		self._size=size
		process.process_number+=1
		self._process_No=self.process_number
		self._run_times=0
		self._low_addr_h=size*m/2-1
		self._high_addr_l=top-size*m/2
		self._accadds=[]
		self._p_table=page_table()
		self._p_table_miss=0
		self._TLB_miss=0

	def __str__(self):
		des_str=''
		des_str+='-----进程信息-----编号-----%s-----内存：%dM----开始运行-----已切换运行次数：%d-----\n' % (self._process_No,self._size,self._run_times)
		des_str+='-----Address-----VP number---PF number---TLB---PageTable'
		return  des_str

	def run(self,accadd_num):
		f=open('visit_seq_'+str(self._process_No)+'.txt','w')
		self.update_table()
		self._run_times+=1
		f.write(str(self))
		f.write('----------进程访问数据地址个数：%d----------\n' % accadd_num)
		# print self
		# print '----------进程访问数据地址个数：%d----------' % accadd_num
		for j in range(accadd_num/2):
			accadd=random.randint(0,self._low_addr_h)
			self._accadds.append(accadd)
			# for i in range(4):
			# 	self._accadds.append(self._accadds[-1]+1)
			accadd = random.randint(self._high_addr_l,top)
			self._accadds.append(accadd)
			# for i in range(4):
			# 	self._accadds.append(self._accadds[-1] + 1)
		for i in range(len(self._accadds)):
			accadd_v_page=self._accadds[i]>>13
			tlb_str='hit'
			p_table_str='hit'
			tlb_result=process.tlb.find_page(accadd_v_page)
			p_page=-1
			if tlb_result==-1:
				self._TLB_miss+=1
				tlb_str='miss'
				p_table_result=self._p_table.find_page(accadd_v_page)
				p_page=self._p_table._v_p_mapper[1]
				process.tlb.add_item(self._accadds[i],p_page)
				if p_table_result==-1:
					self._p_table_miss+=1
					p_table_str='miss'
					if not page_table.physical_frames[p_page]==0:
						f.write('-----已无可用页框，将下面最先分配的页框：%d从进程id：%d回收，将更新该进程的页表-----' % (page_table.first_alloc_flag,page_table.physical_frames[p_page]))
						# print '-----已无可用页框，将下面最先分配的页框：%d从进程id：%d回收，将更新该进程的页表-----' % (page_table.first_alloc_flag,page_table.physical_frames[p_page])
					self._p_table.physical_frames[p_page]=self._process_No
			f.write('---0x%08x----%8d-----%7d----%4s----%4s\n' % (self._accadds[i],accadd_v_page,p_page,tlb_str,p_table_str))
			# print '---0x%08x----%8d-----%7d----%4s----%4s\n' % (self._accadds[i],accadd_v_page,p_page,tlb_str,p_table_str)
		f.close()
		print '进程id：%d 内存：%dM 获取数据地址数量：%d完毕，已完成第%d次运行' % (self._process_No,self._size,accadd_num,self._run_times)

	def update_table(self):
		for k in range(self._p_table.physical_size):
			if self._p_table._used_frames[k]==1 and self._p_table.physical_frames[k] is not self._process_No:
				self._p_table._used_frames[k]=0
				for i in range(64):
					if not len(self._p_table._p_table[i]) == 0:
						for j in range(8192):
							if self._p_table._p_table[i][j] == k:
								self._p_table._p_table[i][j]=-1

	def output_table(self):
		self.update_table()
		with open('page_table_'+str(self._process_No)+'.txt','w') as f:
			f.write(str(self._p_table))