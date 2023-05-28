package ScProcessor
package EXU
import chisel3._
import chisel3.util._
import EXUInfo._
import tools._

object EXUInfo {
    val OPS_NUM = IDU.IDUInsInfo.exuOps.length
}

class EXU extends Module {
    val io = IO(new Bundle {
        val exuOp = Input(UInt(OPS_NUM.W))
        val aluOp = Input(UInt(ALUInfo.OPS_NUM.W))
        val imm = Input(UInt(topInfo.XLEN.W))
        // different from IDU, rs in EXU is the value but not index of register
        val rs1 = Input(UInt(topInfo.XLEN.W))
        val rs2 = Input(UInt(topInfo.XLEN.W))
        val pc = Input(UInt(topInfo.XLEN.W))
        val out = Output(UInt(topInfo.XLEN.W))
    })
    val alu = Module(new ALU)
    alu.io.aluOp := io.aluOp
    io.out := alu.io.out

    val exuOps = IDU.IDUInsInfo.exuOps
    val hit = U_HIT_CURRYING(io.exuOp, exuOps)_

    alu.io.in1 := Mux1H(Seq(
        hit("r1Im") -> io.rs1,
        hit("imX0") -> io.imm,
        hit("imPc") -> io.imm
    ))

    alu.io.in2 := Mux1H(Seq(
        hit("r1Im") -> io.imm,
        hit("imX0") -> 0.U,
        hit("imPc") -> io.pc
    ))
}

object EXUMain extends App {
    val s = getVerilogString(new EXU)
    println(s)
}