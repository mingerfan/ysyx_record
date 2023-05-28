package ScProcessor
package PC
import chisel3._
import chisel3.util._
import topInfo._
import PCInfo._
import tools._

object PCInfo {
    val OPS_NUM = IDU.IDUInsInfo.pcOps.length
}

class PC extends Module {
    val pc_out = IO(Output(UInt(XLEN.W)))
    val pc_next = IO(Output(UInt(XLEN.W)))
    val pcOp = IO(Input(UInt(OPS_NUM.W)))
    val in = IO(new Bundle {
        val imm = Input(UInt(XLEN.W))
        val rs1 = Input(UInt(XLEN.W))
        val rs2 = Input(UInt(XLEN.W))
        val exu = Input(UInt(XLEN.W))
    })

    val pc_reg = RegInit(PC_INIT.U(XLEN.W))
    val pcOps = IDU.IDUInsInfo.pcOps
    val hit = U_HIT_CURRYING(pcOp, pcOps)_

    pc_next := pc_reg + (INS_LEN/8).U
    pc_reg := Mux1H(Seq(
        hit("Inc") -> (pc_reg + (INS_LEN/8).U),
        hit("Jal") -> (pc_reg + in.imm),
        hit("Jalr") -> ((in.imm + in.rs1) & ~(1.U(XLEN.W)))
    ))

    pc_out := pc_reg
}