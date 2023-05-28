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

    io.out := Mux1H(Seq(
        hit("SUM") -> (io.in1 + io.in2),
        hit("UCMP")-> (io.in1 < io.in2),
        hit("WSUM")-> (Fill(topInfo.XLEN - 11, (io.in1 + io.in2)(31)) ## 
        (io.in1 + io.in2)(30, 0)),
    ))
}

object ALUMain extends App {
    val s = getVerilogString(new ALU)
    println(s)
}