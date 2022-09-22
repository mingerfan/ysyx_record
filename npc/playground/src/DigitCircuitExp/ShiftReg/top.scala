package ShiftReg
import chisel3._
import chisel3.util._
import HomeWorkHDL.ClockGen

class top extends Module {
    val io = IO(new Bundle {
        val led = Output(UInt(8.W))
        val seg = Output(Vec(2, UInt(7.W)))
    })

    val clockGen = Module(new ClockGen(50000000, 1))
    val risingEdge = WireDefault(!RegNext(clockGen.io.outClk) & clockGen.io.outClk)
    
    val shiftreg = RegInit(1.U(8.W))
    val x8 = WireDefault(shiftreg(4)^shiftreg(3)^shiftreg(2)^shiftreg(0))
    
    when (risingEdge) {
        shiftreg := Cat(x8, shiftreg(7, 1))
    }

    val dec1 = Module(new Decoder)
    dec1.io.inNum := shiftreg(3, 0)
    io.seg(0) := dec1.io.out

    val dec2 = Module(new Decoder)
    dec2.io.inNum := shiftreg(7, 4)
    io.seg(1) := dec2.io.out

    io.led := shiftreg
}

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
        is (10.U) { seg_wire := "b111_0111".U }
        is (11.U) { seg_wire := "b111_1111".U }
        is (12.U) { seg_wire := "b011_1001".U }
        is (13.U) { seg_wire := "b011_1111".U }
        is (14.U) { seg_wire := "b111_1001".U }
        is (15.U) { seg_wire := "b111_0001".U }
    }
}