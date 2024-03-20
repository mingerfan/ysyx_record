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
    
    val idu = Module(new IDU.IDU)
    val exu = Module(new EXU.EXU)
    val rf = Module(new RF.RFModule)    
    val pc = Module(new PC.PC)
    val ifu = Module(new IFU.IFU)
    val mem_wr = Module(new MEMWR.MEMWR)
    val csr = Module(new RF.CSR)

    dbg_regs := 0.U
    if (NPC_SIM) {
        dbg_regs := rf.dbg_regs
        dbg_csrs  := csr.dbg_csrs
    }

    val hit = U_HIT_CURRYING(idu.dpCtrl.ctrls_out, IDU.IDUInsInfo.ctrls)_

    dontTouch(idu.dataOut)
    dontTouch(idu.dpCtrl)
    dontTouch(exu.io)
    dontTouch(rf.io)

    pc.pcOp     := idu.dpCtrl.pcOp
    pc.in.imm   := idu.dataOut.imm
    pc.in.rs1   := rf.io.rdData1
    pc.in.rs2   := rf.io.rdData2
    pc.in.exu   := exu.io.out
    pc.in.csr   := csr.io.rdData

    ifu.in.bits.pc := pc.pc_out
    ifu.in.valid := true.B
    ifu.out.ready := true.B

    val if_id_in = Wire(DecoupledIO(new IFU.IFUBundleOut))
    val if_id_out = Wire(DecoupledIO(new IFU.IFUBundleOut))
    if_id_in <> ifu.out

    basic.MyStageConnect(if_id_in, if_id_out)

    if_id_out.ready := true.B

    assert(io.inst === ifu.out.bits.inst)

    // IFU is simple, so we don't write it in a single module
    // it seems that it is not neccesary to cache the inst to the register
    io.pc := pc.pc_out

    idu.inst := if_id_out.bits.inst
    
    rf.io.rdAddr1   := Mux(idu.ebreak, 10.U, idu.dataOut.rs1)
    rf.io.rdAddr2   := idu.dataOut.rs2
    rf.io.wrAddr    := idu.dataOut.rd
    rf.io.wrData    := 1.U  // actually we do not use it
    rf.io.wrEn      := ~hit("nwrEn")
    rf.rfOp         := idu.dpCtrl.rfOp
    rf.in.exu       := exu.io.out
    rf.in.pc_next   := pc.pc_next
    rf.in.mem       := mem_wr.io.rd
    rf.in.csr       := csr.io.rdData

    exu.io.exuOp    := idu.dpCtrl.exuOp
    exu.io.aluOp    := idu.dpCtrl.aluOp
    exu.io.imm      := idu.dataOut.imm
    exu.io.rs1      := rf.io.rdData1
    exu.io.rs2      := rf.io.rdData2
    exu.io.pc       := pc.pc_out

    mem_wr.io.rs1   := rf.io.rdData1
    mem_wr.io.rs2   := rf.io.rdData2
    mem_wr.io.imm   := idu.dataOut.imm
    mem_wr.io.memOps:= idu.dpCtrl.memOp
    mem_wr.io.mem_wr_flag := idu.mem_wr

    // csr input
    csr.io.rdAddr := idu.dataOut.csr
    csr.io.wrEn   := hit("csrWr")
    csr.io.wrAddr := idu.dataOut.csr
    csr.io.rsIn   := rf.io.rdData1
    csr.io.immIn  := idu.dataOut.imm
    csr.io.pc     := pc.pc_out
    csr.io.csrOps := idu.dpCtrl.csrOp

    // ebreak and invalid instruction detection
    val ebreak_detect_ = Module(new ebreak_detect)
    val inv_inst_ = Module(new inv_inst)

    ebreak_detect_.io.ebreak    := idu.ebreak
    ebreak_detect_.io.clk       := clock.asBool
    ebreak_detect_.io.a0        := rf.io.rdData1
    ebreak_detect_.io.pc        := pc.pc_out

    inv_inst_.io.inv    := idu.inv_inst
    inv_inst_.io.clk    := clock.asBool
    inv_inst_.io.pc     := pc.pc_out
}