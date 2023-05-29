package ScProcessor
package EXU
import chisel3._
import chisel3.util._
import ALUInfo._
import tools._

object ALUInfo {
    val OPS_NUM = IDU.IDUInsInfo.aluOps.length
}

class ALU extends Module {
    val io = IO(new Bundle {
        val in1 = Input(UInt(topInfo.XLEN.W))
        val in2 = Input(UInt(topInfo.XLEN.W))
        val out = Output(UInt(topInfo.XLEN.W))
        val aluOp = Input(UInt(OPS_NUM.W))
    })
    
    val aluOps = IDU.IDUInsInfo.aluOps
    val hit = U_HIT_CURRYING(io.aluOp, aluOps)_

    val in1pin2 = io.in1 + io.in2
    val in1sin2 = io.in1 - io.in2
    val in1xorin2 = io.in1 ^ io.in2
    val in1andin2 = io.in1 & io.in2

    io.out := Mux1H(Seq(
        hit("SUM") -> (in1pin2),
        hit("UCMP")-> (io.in1 < io.in2),
        hit("WSUM")-> (Fill(topInfo.XLEN - 11, in1pin2(31)) ## 
        in1pin2(30, 0)),
        hit("SUB") -> (in1sin2),
        hit("XOR") -> (in1xorin2),
        hit("AND") -> (in1andin2),
        hit("ULSW")-> U_SEXT64((io.in1<<(io.in2(4, 0)))(31, 0), 32)
    ))
}

object ALUMain extends App {
    val s = getVerilogString(new ALU)
    println(s)
}