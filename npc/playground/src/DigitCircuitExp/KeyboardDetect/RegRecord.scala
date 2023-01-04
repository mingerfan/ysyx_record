package KeyboardDetect
import chisel3._
import chisel3.util._

class RegRecord extends Module {
    val io = IO(new Bundle {
        val fifo = Flipped(new DecoupledIO(UInt(8.W)))
        val keyCode = Output(UInt(8.W))
        val ascii = Output(UInt(8.W))
        val backspace = Output(Bool())
    })
    val charReg = RegInit(0.U(8.W))
    val ctrlReg = RegInit(0.U(8.W))

    val fifoReg = RegInit(0.U(8.W))
    val isRec = RegInit(false.B)

    // FIFO data receive fsm
    io.fifo.ready := false.B
    isRec := false.B
    when (io.fifo.valid) {
        isRec := true.B
        fifoReg := io.fifo.bits
        io.fifo.ready := true.B
    }

    val idle :: releaseJudge :: release :: twoByte :: Nil = Enum(4)
    val fsmState = RegInit(idle)
    val done = RegInit(false.B)

    switch (fsmState) {
        is (idle) {
            when (isRec && !done) {
                fsmState := releaseJudge
            }
        }
        is (releaseJudge) {
            fsmState := idle
            switch (fifoReg) {
                is ("hF0".U) { fsmState := release }
                is ("hE0".U) { fsmState := twoByte }
            }
        }
        is (release) {
            when (isRec) {
                when (fifoReg === "hE0".U) {
                    fsmState := twoByte
                } .otherwise {
                    fsmState := idle
                }
            }
        }
        is (twoByte) {
            when (isRec) {
                fsmState := idle
            }
        }
    }
    
    val dataReg = RegInit(0.U(24.W))

    switch (fsmState) {
        is (releaseJudge) {
            when (fifoReg =/= "hF0".U && fifoReg =/= "hE0".U) {
                done := true.B
                dataReg := Cat("h0000".U, fifoReg)
            }
        }
        is (release) {
            when (isRec && fifoReg =/= "hE0".U) {
                done := true.B
                dataReg := Cat("hF000".U, fifoReg)
            }
        }
        is (twoByte) {
            when (isRec) {
                done := true.B
                dataReg := Cat("hF0E0".U, fifoReg)
            }
        }
    }

    val ctrlDec = Module(new CtrlDec)
    val ctrlDec1 = Module(new CtrlDec)
    val charDec = Module(new CharDec)
    val finalDec = Module(new FinalDec)

    ctrlDec.io.In := dataReg(15,0)
    ctrlDec1.io.In := ctrlReg
    charDec.io.In := charReg

    when (done) {
        done := false.B
        when (dataReg(23,16) === "hF0".U) {
            when (charReg === dataReg(15,0)) {
                charReg := 0.U
            } .elsewhen (ctrlReg === dataReg(15,0)) {
                ctrlReg := 0.U
            }
        } .otherwise {
            when (ctrlDec.io.isCtrl) {
                ctrlReg := dataReg(15,0)
            } .otherwise {
                charReg := dataReg(15,0)
            }
        }
    }

    finalDec.io.In := charDec.io.dec_step1
    finalDec.io.shift := ctrlDec1.io.shift
    finalDec.io.enter := ctrlDec1.io.enter
    finalDec.io.space := ctrlDec1.io.space

    io.keyCode := charReg
    io.ascii := finalDec.io.dec
    io.backspace := ctrlDec1.io.backspace
}