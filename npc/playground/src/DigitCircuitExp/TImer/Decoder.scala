package HomeWorkHDL
import chisel3._
import chisel3.util._

class Decoder extends Module {
    val io = IO(new Bundle {
        val inNum = Input(UInt(4.W))
        val out = Output(UInt(7.W))
    })

    val seg_wire = Wire(UInt(7.W))

    seg_wire := 0.U

    io.out := ~seg_wire

    switch (io.inNum) {
        is (0.U) { seg_wire := "b011_1111".U }
        is (1.U) { seg_wire := "b000_0110".U }
        is (2.U) { seg_wire := "b101_1011".U }
        is (3.U) { seg_wire := "b100_1111".U }
        is (4.U) { seg_wire := "b110_0110".U }
        is (5.U) { seg_wire := "b110_1101".U }
        is (6.U) { seg_wire := "b111_1101".U }
        is (7.U) { seg_wire := "b000_0111".U }
        is (8.U) { seg_wire := "b111_1111".U }
        is (9.U) { seg_wire := "b110_1111".U }
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