package ScProcessor
package RF
import chisel3._
import chisel3.util._
import topInfo._
import utils.tools._
import rfInfo._
import RFMInfo._
import topInfo._

object RFMInfo {
    val OPS_NUM = IDU.IDUInsInfo.rfOps.length
}

class RFModule extends Module {
    val io = IO(new DualPortsIO(REGS_NUM, REGS_WIDTH))
    val rfOp = IO(Input(UInt(OPS_NUM.W)))
    val in = IO(new Bundle {
        val exu = Input(UInt(XLEN.W))
        val pc_next = Input(UInt(XLEN.W))
        val mem = Input(UInt(XLEN.W))
        val csr = Input(UInt(RF.CSRInfo.CSR_WIDTH.W))
    })
    val dbg_regs_w = if (NPC_SIM) REGS_NUM*REGS_WIDTH else 0
    val dbg_regs = IO(Output(UInt(dbg_regs_w.W)))

    val rfOps = IDU.IDUInsInfo.rfOps
    val hit = U_HIT_CURRYING(rfOp, rfOps)_

    val rf_in = Module(new RegFile)

    dbg_regs := 0.U
    if (NPC_SIM) {
        dbg_regs := rf_in.dbg_regs
    } 

    rf_in.io <> io
    rf_in.io.wrData := MuxCase(in.exu, Seq(
        hit("pcn")  -> in.pc_next,
        hit("mem")  -> in.mem,
        hit("csr")  -> in.csr,
    ))
}