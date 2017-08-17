#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
☆*°☆*°(∩^o^)~━━  2017/8/15 11:49        
      (ˉ▽￣～) ~~ 一捆好葱 (*˙︶˙*)☆*°
      Fuction：        √ ━━━━━☆*°☆*°
"""
from process import process
if __name__=='__main__':
	pp=[]
	for i in range(10):
		pp.append(process(pow(2,i)))
	cnt=3
	while cnt>0:
		cnt-=1
		for i in range(10):
			pp[i].run(500)
	for i in range(10):
		pp[i].output_table()
