package ScProcessor
package MEMWR
import chisel3._
import chisel3.util._
import topInfo._
import tools._
import MEMWRInfo._

object MEMWRInfo {
    val OPS_NUM = IDU.IDUInsInfo.memOps.length
}

class MEM_WR extends BlackBox {
    val io = IO(new Bundle {
        val raddr = Input(UInt(XLEN.W))
        val rdata = Output(UInt(XLEN.W))
        val waddr = Input(UInt(XLEN.W))
        val wdata = Input(UInt(XLEN.W))
        val wmask = Input(UInt((XLEN/8).W))
    })
}

class MEMWR extends Module {
    val io = IO(new Bundle {
        val rs1 = Input(UInt(XLEN.W))
        val rs2 = Input(UInt(XLEN.W))
        val rd  = Output(UInt(XLEN.W))
        val imm = Input(UInt(XLEN.W))
        val memOps = Input(UInt(OPS_NUM.W))
    })
    val hit = U_HIT_CURRYING(io.memOps, IDU.IDUInsInfo.memOps)_

    val mem_wr_in = Module(new MEM_WR)

    val taddr = WireDefault(io.rs1 + io.imm)

    io.rd := mem_wr_in.io.rdata

    mem_wr_in.io.raddr := taddr
    mem_wr_in.io.waddr := taddr
    mem_wr_in.io.wdata := Mux1H(Seq(
        hit("sd") -> io.rs2
    ))
    mem_wr_in.io.wmask := Mux1H(Seq(
        hit("sd") -> "b1111_1111".U
    ))
}