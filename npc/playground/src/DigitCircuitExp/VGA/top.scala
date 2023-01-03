package DCE_VGA
import chisel3._
import chisel3.util._
import HomeWorkHDL.ClockGen

class top extends Module {
    val io = IO(new Bundle {
        val vgaVsync = Output(Bool())
        val vgaHsync = Output(Bool())
        val vgaR = Output(UInt(8.W))
        val vgaG = Output(UInt(8.W))
        val vgaB = Output(UInt(8.W))
    })

    val vgaCtrl = Module(new VGA_Ctrl)
    val clockGen = Module(new ClockGen(100000000, 25000000))
    vgaCtrl.io.pclk := clockGen.io.outClk.asBool.asClock
    vgaCtrl.io.vgaData := "hFFFFFF".U
    io.vgaHsync := vgaCtrl.io.hsync
    io.vgaHsync := vgaCtrl.io.vsync
    io.vgaR := vgaCtrl.io.vgaR
    io.vgaG := vgaCtrl.io.vgaG
    io.vgaB := vgaCtrl.io.vgaB
}
