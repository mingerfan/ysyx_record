package ScProcessor
import chisel3._
import chisel3.util._
import topInfo._
import scala.math._
import chisel3.dontTouch
import utils.tools._

object topInfo {
    val INSTRUCTION = "riscv64"
    val XLEN = 64
    val R_NUM = 32
    val INS_LEN = 32
    val PC_INIT = BigInt("80000000", 16)
    val NPC_SIM = true
    val ARCH = "single"
}

class top extends Module {
    val dbg_regs_w = if (NPC_SIM) RF.rfInfo.REGS_NUM*RF.rfInfo.REGS_WIDTH else 0
    val io = IO(new Bundle {
        val inst = Input(UInt(INS_LEN.W))   // Get Input Instruction
        val pc = Output(UInt(XLEN.W))   // PC output to read ins in memory
    })
    val dbg_regs = IO(Output(UInt(dbg_regs_w.W)))
    val dbg_csrs  = IO(Output(UInt(RF.CSRInfo.DBG_CSR_W.W)))
    
    val idu = Module(new IDU.IDUWrapper)
    val exu = Module(new EXU.EXUWrapper)
    val rf = Module(new RF.RFModule)    
    val pc = Module(new PC.PC)
    val ifu = Module(new IFU.IFU)
    val mem_wr = Module(new MEMWR.MEMWRWrapper)
    val csr = Module(new RF.CSR)

    dbg_regs := 0.U
    if (NPC_SIM) {
        dbg_regs := rf.dbg_regs
        dbg_csrs  := csr.dbg_csrs
    }   

    // dontTouch(idu.dataOut)
    // dontTouch(idu.dpCtrl)
    // dontTouch(exu.io)
    dontTouch(rf.io)

    // 因为PC需要用到这个，所以在前面声明了
    val ex_mem_in = Wire(DecoupledIO(new basic.EX_MEM_Bundle))
    val ex_mem_out = Wire(DecoupledIO(new basic.EX_MEM_Bundle))

    // 由于RF需要写回，所以在此声明
    val mem_wb_in = Wire(DecoupledIO(new basic.MEM_WB_Bundle))
    val mem_wb_out = Wire(DecoupledIO(new basic.MEM_WB_Bundle))

    /***************IFU****************/
    pc.pcOp     := ex_mem_out.bits.idu.pcOp
    pc.in.imm   := ex_mem_out.bits.idu.dataOut.imm
    pc.in.rs1   := ex_mem_out.bits.rf.rdData1
    pc.in.rs2   := ex_mem_out.bits.rf.rdData2
    pc.in.exu   := ex_mem_out.bits.exu
    pc.in.csr   := ex_mem_out.bits.csr

    ifu.in.bits.pc := pc.pc_out
    ifu.in.valid := true.B
    ifu.out.ready := true.B

    val if_id_in = Wire(DecoupledIO(new IFU.IFUBundleOut))
    val if_id_out = Wire(DecoupledIO(new IFU.IFUBundleOut))
    if_id_in <> ifu.out

    basic.MyStageConnect(if_id_in, if_id_out)

    if_id_out.ready := true.B

    assert(io.inst === ifu.out.bits.inst)

    io.pc := pc.pc_out

    /***************IFU****************/


    /***************IDU****************/
    idu.in <> if_id_out

    val id_ex_bits = Wire(new basic.ID_EX_Bundle)
    id_ex_bits.idu <> idu.out.bits
    id_ex_bits.rf.rdData1 := rf.io.rdData1
    id_ex_bits.rf.rdData2 := rf.io.rdData2
    id_ex_bits.ifu <> if_id_out.bits
    id_ex_bits.csr <> csr.io.rdData

    val id_ex_in = Wire(DecoupledIO(new basic.ID_EX_Bundle))
    val id_ex_out = Wire(DecoupledIO(new basic.ID_EX_Bundle))

    val hit = U_HIT_CURRYING(id_ex_out.bits.idu.ctrls_out, IDU.IDUInsInfo.ctrls)_
    
    rf.io.rdAddr1   := Mux(mem_wb_out.bits.idu.ebreak, 10.U, mem_wb_out.bits.idu.dataOut.rs1)
    rf.io.rdAddr2   := mem_wb_out.bits.idu.dataOut.rs2
    rf.io.wrAddr    := mem_wb_out.bits.idu.dataOut.rd
    rf.io.wrData    := 1.U  // actually we do not use it
    rf.io.wrEn      := ~hit("nwrEn")
    rf.rfOp         := mem_wb_out.bits.idu.rfOp
    rf.in.exu       := mem_wb_out.bits.exu
    rf.in.pc_next   := pc.pc_next// Todo: 在实现流水线处理器的时候需要修改
    rf.in.mem       := mem_wb_out.bits.mem
    rf.in.csr       := mem_wb_out.bits.csr

    // csr input
    csr.io.rdAddr := idu.out.bits.dataOut.csr
    csr.io.wrEn   := hit("csrWr")
    csr.io.wrAddr := idu.out.bits.dataOut.csr
    csr.io.rsIn   := rf.io.rdData1
    csr.io.immIn  := idu.out.bits.dataOut.imm
    csr.io.pc     := pc.pc_out
    csr.io.csrOps := idu.out.bits.csrOp

    id_ex_in.bits <> id_ex_bits
    id_ex_in.valid := idu.out.valid && if_id_out.valid
    idu.out.ready := id_ex_in.ready

    basic.MyStageConnect(id_ex_in, id_ex_out)
    /***************IDU****************/


    /***************EXU****************/
    exu.in <> id_ex_out

    ex_mem_in.bits.exu_apply(exu.out.bits)
    ex_mem_in.bits.idu_apply(id_ex_out.bits.idu)
    ex_mem_in.bits.rf_apply(id_ex_out.bits.rf.rdData1, id_ex_out.bits.rf.rdData2)
    ex_mem_in.bits.ifu_apply(id_ex_out.bits.ifu)
    ex_mem_in.bits.csr_apply(id_ex_out.bits.csr)

    ex_mem_in.valid := id_ex_out.valid && exu.out.valid
    exu.out.ready := ex_mem_in.ready

    basic.MyStageConnect(ex_mem_in, ex_mem_out)

    /***************EXU****************/


    /***************MEM****************/
    mem_wr.in <> ex_mem_out

    mem_wb_in.bits.ex_mem_apply(ex_mem_out.bits)
    mem_wb_in.bits.mem_apply(mem_wr.out.bits)

    mem_wb_in.valid := ex_mem_out.valid && mem_wr.out.valid
    mem_wr.out.ready := mem_wb_in.ready

    basic.MyStageConnect(mem_wb_in, mem_wb_out)

    // 这个是额外的保证ready的连接
    mem_wb_out.ready := true.B
    /***************MEM****************/


    // 这两个暂时不用连接到流水线中，可以直接连接IDU
    // ebreak and invalid instruction detection
    val ebreak_detect_ = Module(new ebreak_detect)
    val inv_inst_ = Module(new inv_inst)

    ebreak_detect_.io.ebreak    := idu.out.bits.ebreak
    ebreak_detect_.io.clk       := clock.asBool
    ebreak_detect_.io.a0        := rf.io.rdData1
    ebreak_detect_.io.pc        := pc.pc_out

    inv_inst_.io.inv    := idu.out.bits.inv_inst
    inv_inst_.io.clk    := clock.asBool
    inv_inst_.io.pc     := pc.pc_out
}