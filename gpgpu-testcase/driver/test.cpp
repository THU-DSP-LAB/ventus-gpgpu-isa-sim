#include "ventus.h"
struct meta_data{  // 这个metadata是供驱动使用的，而不是给硬件的
    uint64_t kernel_id;
    uint64_t kernel_size[3];///> 每个kernel的workgroup三维数目
    uint64_t wf_size; ///> 每个warp的thread数目
    uint64_t wg_size; ///> 每个workgroup的warp数目
    uint64_t metaDataBaseAddr;///> CSR_KNL的值，
    uint64_t ldsSize;///> 每个workgroup使用的local memory的大小
    uint64_t pdsSize;///> 每个thread用到的private memory大小
    uint64_t sgprUsage;///> 每个workgroup使用的标量寄存器数目
    uint64_t vgprUsage;///> 每个thread使用的向量寄存器数目
    uint64_t pdsBaseAddr;///> private memory的基址，要转成每个workgroup的基地址， wf_size*wg_size*pdsSize
    meta_data(uint64_t arg0,uint64_t arg1[],uint64_t arg2,uint64_t arg3,uint64_t arg4,uint64_t arg5,\
      uint64_t arg6,uint64_t arg7,uint64_t arg8,uint64_t arg9) \
      :kernel_id(arg0),wf_size(arg2),wg_size(arg3),metaDataBaseAddr(arg4),ldsSize(arg5),pdsSize(arg6),\
      sgprUsage(arg7),vgprUsage(arg8),pdsBaseAddr(arg9)
      {
        kernel_size[0]=arg1[0];kernel_size[1]=arg1[1];kernel_size[2]=arg1[2];
      }
};
int main(){
    uint64_t num_warp=2;
    uint64_t num_thread=8;
    uint64_t num_workgroups[3]={2,1,1};
    uint64_t num_workgroup=num_workgroups[0]*num_workgroups[1]*num_workgroups[2];
    uint64_t num_processor=num_warp*num_workgroup;
    uint64_t ldssize=0x1000;
    uint64_t pdssize=0x1000;
    uint64_t pdsbase=0x7a000000;
    uint64_t start_pc=0x80000000;
    uint64_t knlbase=0x90000000;
    meta_data meta(0,num_workgroups,num_thread,num_warp,knlbase,ldssize,pdssize,\
        32,32,pdsbase);
    char filename[]="saxpy.riscv";

    //uint64_t buffer_num=3;
    uint64_t size_0=0x10000000;
    uint64_t data_0[]={8,12,4,4,6};
    uint64_t vaddr_0;
    uint64_t size_1=0x10000000;
    uint64_t data_1[]={8,12,4,4,6};
    uint64_t vaddr_1;

    vt_device_h p=nullptr;
    vt_dev_open(&p);
    vt_buf_alloc(p,size_0,&vaddr_0,0,0,0);
    vt_buf_alloc(p,size_0,&vaddr_1,0,0,0);
    vt_copy_to_dev(p,vaddr_1,data_1,40,0,0);

    int64_t* data_moveback=new int64_t[5];
    for(int i=0;i<5;i++) data_moveback[i]= -1;

    vt_copy_from_dev(p,vaddr_1+8,data_moveback,32,0,0);
    for(int i=0;i<5;i++) printf("%ld ",data_moveback[i]);
    //再搬回来看一下是否成功

    vt_upload_kernel_file(p,filename,0);
    vt_start(p,&meta,0);
    printf("finish running");

    vt_buf_free(p,0,nullptr,0,0);


    return 0;
}