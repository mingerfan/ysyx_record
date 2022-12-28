package HomeWorkHDL
import chisel3._
import chisel3.util._

class Decoder extends Module {
    val io = IO(new Bundle {
        val inNum = Input(UInt(4.W))
        val out = Output(UInt(7.W))
    })

    io.out := "b0000_000".U

    switch (io.inNum) {
        is (0.U) {io.out := "b1111_110".U}
        is (1.U) {io.out := "b0110_000".U}
        is (2.U) {io.out := "b1101_101".U}
        is (3.U) {io.out := "b1111_001".U}
        is (4.U) {io.out := "b0110_011".U}
        is (5.U) {io.out := "b1011_011".U}
        is (6.U) {io.out := "b1011_111".U}
        is (7.U) {io.out := "b1110_000".U}
        is (8.U) {io.out := "b1111_111".U}
        is (9.U) {io.out := "b1111_011".U}
    }
}

class Decoder2 extends Module {
    val io = IO(new Bundle {
        val inNum = Input(UInt(8.W))
        val out1 = Output(UInt(7.W))
        val out2 = Output(UInt(7.W))
    })

    val d1 = Module(new Decoder)
    val d2 = Module(new Decoder)

    d1.io.inNum := io.inNum(3,0)
    io.out1 := d1.io.out

    d2.io.inNum := io.inNum(7,4)
    io.out2 := d2.io.out
}

object DecoderMain extends App {
    val s = getVerilogString(new Decoder2())
    println(s)
}