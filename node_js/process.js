'use strict';
var fs=require('fs');
var TLB=require('./TLB');
var page_table=require('./page_table');
var m=Math.pow(2,20);
var top=Math.pow(2,32);
var process_number=0;
var tlb=new TLB();
var j,i,k;
class process{
        constructor(size=256){
            this._size=size;
            process_number+=1;
            this._process_No=process_number;
            this._run_times=0;
            this._low_addr_h=size*m/2-1;
            this._high_addr_l=top-size*m/2;
            this._accadds=[];
            this._p_table=new page_table();
            this._p_table_miss=0;
            this._TLB_miss=0;
        }
    des_str(){
		var des_str='';
		des_str+='-----进程信息-----编号-----'+this._process_No+'-----内存：'+this._size+'M----开始运行-----已切换运行次数：'+this._run_times.toString()+'-----\n';
		des_str+='-----Address-----VP number---PF number---TLB---PageTable\n';
        return  des_str;
    }
    run(accadd_num){
        this.update_table();
        this._run_times+=1;
        var f=fs.createWriteStream('./visit_seq_'+this._process_No+'.txt','utf-8');
        f.write(this.des_str());
        f.write('----------进程访问数据地址个数：'+ accadd_num.toString()+'----------\n');
        console.log(this.des_str());
        console.log('----------进程访问数据地址个数：'+ accadd_num.toString()+'----------');
        for(j=0;j<accadd_num/2;j++){
            var accadd= Math.floor(Math.random()*this._low_addr_h);
            this._accadds.push(accadd);
            // for(i=0;i<4;i++){
            //     this._accadds.push(this._accadds[-1]+1);
            // }
            accadd =  Math.floor(Math.random()*(top-this._high_addr_l+1)+this._high_addr_l);
			this._accadds.push(accadd)
			// for i in range(4):
			//      this._accadds.push(this._accadds[-1] + 1)
        }
        for(i=0;i<this._accadds.length;i++){
            var accadd_v_page=Math.floor(this._accadds[i]/Math.pow(2,13));
            if(accadd_v_page<0){
                accadd_v_page=0;
            }
            var tlb_str='hit';
			var p_table_str='hit';
			var tlb_result=tlb.find_page(accadd_v_page);
            var p_page=tlb_result;
			if (tlb_result===-1){
				this._TLB_miss+=1;
				tlb_str='miss';
                var p_table_result=this._p_table.find_page(accadd_v_page);
                p_page=p_table_result;
				tlb.add_item(accadd_v_page,p_page);
				if(p_table_result===-1){
                    p_page=this._p_table._v_p_mapper[1];
					this._p_table_miss+=1;
                    p_table_str='miss';
					if(this._p_table.get_physical_frames(p_page)!==0){
                        f.write('-----已无可用页框，将下面最先分配的页框：'+this._p_table.get_first_alloc_flag().toString()+'从进程id：'+this._p_table.get_physical_frames(p_page).toString()+'回收，将更新该进程的页表-----\n');
                        // console.log('-----已无可用页框，将下面最先分配的页框：'+this._p_table.get_first_alloc_flag().toString()+'从进程id：'+this._p_table.get_physical_frames(p_page).toString()+'回收，将更新该进程的页表-----');
                    }
                    this._p_table.set_physical_frames(p_page,this._process_No);
                }
            }
            f.write('---0x'+this._accadds[i].toString(16)+'----'+accadd_v_page.toString()+'-----'+p_page.toString()+'----'+tlb_str+'----'+p_table_str+'\n');
            // console.log('---0x'+this._accadds[i].toString(16)+'----'+accadd_v_page.toString()+'-----'+p_page.toString()+'----'+tlb_str+'----'+p_table_str+'\n');
        }
        f.write('进程id：'+this._process_No +'内存：'+this._size+'M 获取数据地址数量：'+accadd_num+'完毕，已完成第'+this._run_times+'次运行\n');
        // f.end();
        console.log('进程id：'+this._process_No +'内存：'+this._size+'M 获取数据地址数量：'+accadd_num+'完毕，已完成第'+this._run_times+'次运行\n');
    }
        
    update_table(){
        for(k=0;k<this._p_table.get_physical_size();k++){
            if (this._p_table._used_frames[k]===1 && this._p_table.get_physical_frames(k)!== this._process_No){
				this._p_table._used_frames[k]=0;
				for(i=0;i<64;i++){
					if (this._p_table._p_table[i].length!== 0){
						for(j=0;j<8192;j++){
							if(this._p_table._p_table[i][j] ===k){
								this._p_table._p_table[i][j]=-1;
                                this._p_table._used_frames[k]=0;
                            }
                        }
                    }
                }
            }
        }   
    }
	
    output_table(){
        this.update_table();
        fs.writeFile('./page_table_'+this._process_No+'.txt',this._p_table.des_str(), function (err) {
            if (err) {
                console.log(err);
            } else {
                console.log('第 '+this._process_No+' 个页表已成功输出(异步输出)\n');
            }
        });
        // fs.writeFileSync('./page_table_'+this._process_No+'.txt',str(this._p_table));
    }
}
module.exports=process;
