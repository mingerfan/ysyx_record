package HomeWorkHDL
import chisel3._
import chisel3.util._

abstract class REdgeCnt(maxNum: Int) extends Module {
    val w = log2Ceil(maxNum)
    val io = IO(new Bundle {
        val reClk = Input(Bool())
        val load = Input(Bool())
        val loadNum = Input(UInt(w.W))
        val tick = Output(Bool())
        val cntOut = Output(UInt(w.W))
    })
    // re aka rising edge
    val reDet = Wire(Bool())
    reDet := io.reClk & !RegNext(io.reClk) 
    when (RegNext(reset.asBool())||reset.asBool()) {
        reDet := false.B
    }
    val cntReg = RegInit(0.U(w.W))
}

class ReCntRising(maxNum: Int) extends REdgeCnt(maxNum) {
    val loadNumReg = Reg(UInt(w.W))
    val maxCnt = maxNum - 1

    when (reDet && cntReg < maxCnt.U) {
        cntReg := cntReg + 1.U
    } .elsewhen (reDet && cntReg === maxCnt.U) {
        cntReg := 0.U
    }

    when (io.load) {
        cntReg := io.loadNum
    }

    io.tick := cntReg === 0.U
    io.cntOut := cntReg
}

trait UDCaculate {
    def Caculate(MaxShi: Int, MaxGe: Int, isUp: Boolean, data: UInt) = {
        var MaxShi_ = MaxShi
        var MaxGe_ = MaxGe
        if (MaxGe == 0) {
            MaxShi_ -= 1
            MaxGe_ = 9
        } else {
            MaxGe_ -= 1
        }
        val MaxShiHalf = MaxShi_ / 2

        val Shi = data(7, 4)
        val Ge = data(3, 0)
        val outWire = Wire(UInt(8.W))
        outWire := data
        if (isUp) {
            when (Shi < MaxShi_.U && Ge > 9.U) {
                outWire := Cat((Shi + 1.U), 0.U(4.W))
            } .elsewhen ((Shi === MaxShi_.U && Ge > MaxGe_.U) || (Shi > MaxShi_.U && (Shi <= MaxShiHalf.U + 5.U))) {
                // given that this module is inserted after an adder, 
                // before a reg, so use Ge > MaxGe_ instead of >=
                outWire := 0.U
            } .elsewhen (Shi > MaxShiHalf.U + 5.U) {
                outWire := Cat(MaxShi_.U(4.W), MaxGe_.U(4.W))
            }
        } else {
            when ((Shi > MaxShi_.U && (Shi <= MaxShiHalf.U + 5.U)) || (Shi === MaxShi_.U && Ge > MaxGe_.U)) {
                outWire := 0.U
            } .elsewhen (Shi > MaxShiHalf.U + 5.U) {
                outWire := Cat(MaxShi_.U(4.W), MaxGe_.U(4.W))
            } .elsewhen (Shi < MaxShi_.U && Ge > 9.U) {
                outWire := Cat(Shi, 9.U(4.W))
            }
        }
        outWire
    }
}

class ReCntRising10(MaxShi: Int, MaxGe: Int) extends REdgeCnt(255) with UDCaculate {

    val res = Caculate(MaxShi, MaxGe, true, cntReg + 1.U)
    val loadReg = RegInit(false.B)

    when (reDet) {
        cntReg := res
        loadReg := false.B
    }

    when (io.load) {
        cntReg := Caculate(MaxShi, MaxGe, true, io.loadNum)
        loadReg := true.B
    }

    io.tick := cntReg === 0.U;
    when (loadReg) {
        io.tick := false.B
    }

    io.cntOut := cntReg
}

class ReCntDecreasing10(MaxShi: Int, MaxGe: Int) extends REdgeCnt(255) with UDCaculate {
    val res = Caculate(MaxShi, MaxGe, false, cntReg - 1.U)

    val loadReg = RegInit(false.B)

    when (reDet) {
        cntReg := res
        loadReg := false.B
    }

    when (io.load) {
        cntReg := Caculate(MaxShi, MaxGe, false, io.loadNum)
        loadReg := true.B
    }
    var MaxShi_ = MaxShi
    var MaxGe_ = MaxGe 
    if (MaxGe == 0) {
        MaxShi_ -= 1
        MaxGe_ = 9
    } else {
        MaxGe_ -= 1
    }
    io.tick := cntReg === Cat(MaxShi_.U(4.W), MaxGe_.U(4.W));
    when (loadReg) {
        io.tick := false.B
    }

    io.cntOut := cntReg
}

class ReCntDecreasing(maxNum: Int) extends REdgeCnt(maxNum) {
    val loadNumReg = Reg(UInt(w.W))
    val maxCnt = maxNum - 1

    when (reDet && cntReg > 0.U) {
        cntReg := cntReg - 1.U
    } .elsewhen (reDet && cntReg === 0.U) {
        cntReg := maxCnt.U
    }

    when (io.load) {
        cntReg := io.loadNum
    }

    io.tick := cntReg === maxCnt.U
    io.cntOut := cntReg
}
