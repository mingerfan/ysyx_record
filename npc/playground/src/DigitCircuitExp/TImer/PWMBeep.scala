package HomeWorkHDL
import chisel3._
import chisel3.util._

class Beep(srcFrq: Int) extends Module {
    val io = IO(new Bundle {
        val beepOut = Output(Bool())
    })

    def PWM(cycles: Int, din: UInt) = {
        val width = log2Ceil(cycles)
        val cnt_r = RegInit(0.U(width.W))
        cnt_r := Mux(cnt_r === (cycles-1).U, 0.U, cnt_r + 1.U)
        din > cnt_r
    }

    val clk2Hz = Module(new ClockGen(srcFrq, 2))

    val PWM_Vec = Wire(Vec(3, Bool()))
    PWM_Vec(0) := PWM(srcFrq/523, (srcFrq/(523*2)).U) // 523Hz for Do
    PWM_Vec(1) := PWM(srcFrq/659, (srcFrq/(659*2)).U) // 659Hz for Mi
    PWM_Vec(2) := PWM(srcFrq/784, (srcFrq/(784*2)).U) // 784Hz for So

    // Only for test
    // PWM_Vec(0) := PWM(10, 5.U) // 523Hz for Do
    // PWM_Vec(1) := PWM(10, 2.U) // 659Hz for Mi
    // PWM_Vec(2) := PWM(10, 8.U) // 784Hz for So

    val cnt = Module(new ReCntRising(3))
    cnt.io.reClk := clk2Hz.io.outClk
    cnt.io.load := false.B
    cnt.io.loadNum := 0.U

    io.beepOut := PWM_Vec(cnt.io.cntOut)
}