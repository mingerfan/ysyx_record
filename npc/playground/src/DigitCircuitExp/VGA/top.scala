package DCE_VGA
import chisel3._
import chisel3.util._

class top extends Module {
    val io = IO(new Bundle {
        val vgaVsync = Output(Bool())
        val vgaHsync = Output(Bool())
        val vgaBlank = Output(Bool())
        val vgaR = Output(UInt(8.W))
        val vgaG = Output(UInt(8.W))
        val vgaB = Output(UInt(8.W))
    })

    val vgaCtrl = Module(new VGA_Ctrl)
    vgaCtrl.io.pclk := clock
    vgaCtrl.io.vgaData := Mux(vgaCtrl.io.hAddr < 300, "hFFFFFF".U, "h3F8F5F".U)
    io.vgaHsync := vgaCtrl.io.hsync
    io.vgaVsync := vgaCtrl.io.vsync
    io.vgaBlank := vgaCtrl.io.valid
    io.vgaR := vgaCtrl.io.vgaR
    io.vgaG := vgaCtrl.io.vgaG
    io.vgaB := vgaCtrl.io.vgaB
}
