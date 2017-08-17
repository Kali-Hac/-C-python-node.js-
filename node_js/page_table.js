'use strict';
var physical_size=8192;
var i;
var physical_frames=[];
var fs=require('fs');
for(i=0;i<physical_size;i++){
    physical_frames.push(0);
}
var seek_frames_flag=0;
var first_alloc_flag=0;
class page_table{
    constructor(f_size=64,s_size=8192){
        this._f_size=f_size;
        this._p_table=[];
        var temp=[];
        this._used_frames=[];
        this._v_p_mapper=[0,0];
        for(i=0;i<f_size;i++){
            this._p_table.push(temp);
        }
        for(i=0;i<physical_size;i++){
            this._used_frames.push(0);
        }
        this._s_size=s_size;
    }
    get_seek_frames_flag(){
        return seek_frames_flag;
    }
    get_first_alloc_flag(){
        return first_alloc_flag;
    }
    get_physical_frames(i){
        return physical_frames[i];
    }
    set_physical_frames(i,value){
        physical_frames[i]=value;
    }
    get_physical_size(){
        return physical_size;
    }
    des_str(){
        var op_str='';
        this._p_table.forEach(function(f_table) {
            if(f_table.length!==0){
                var f_num=this._p_table.indexOf(f_table);
                op_str+='-----[一级页表]----------第 '+f_num+' 页-----\n';
                op_str+= "-----[二级页表]-----虚页项-----实页项-----\n";
                f_table.forEach(function(v_page) {
                    if(v_page!==-1){
                        //转换为字符串
                        op_str+='-------------------'+(f_table.indexOf(v_page)+f_num*64).toString()+'-----'+v_page.toString()+'-----\n';
                    }
                })
            }   
        })
        var des_str='-----[只显示被访问和载入/更新的页表项，不显示实页偏移量]-----';
        return des_str+op_str;
    }
   
    find_page(v_page){
        var f_number=Math.floor(v_page/8192);
        var s_number=v_page%8192;
        if (this._p_table[f_number].length!==0){
            if(this._p_table[f_number][s_number]!==-1)
                return this._p_table[f_number][s_number];
            else
                this._p_table[f_number][s_number] =this.seek_avail_frames();
        }
        else{
            for(i=0;i<8192;i++){
                this._p_table[f_number].push(-1);
            }
            this._p_table[f_number][s_number] = this.seek_avail_frames();
        }
        this._v_p_mapper[0]=v_page;
        this._v_p_mapper[1]=this._p_table[f_number][s_number];
        this._used_frames[this._v_p_mapper[1]]=1;
        return -1;
    }
    seek_avail_frames(){
        var cnt=0;
        while(physical_frames[seek_frames_flag]!==0){
            seek_frames_flag=(seek_frames_flag+1)%physical_size;
            cnt+=1;
            if(cnt>physical_size){
                first_alloc_flag=(first_alloc_flag+1)%physical_size;
                return (first_alloc_flag-1+physical_size)%physical_size;
                break;
            }
        }
        return seek_frames_flag;
    }
}
module.exports=page_table;
