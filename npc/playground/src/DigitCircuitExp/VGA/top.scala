package DCE_VGA
import chisel3._
import chisel3.util._
import chisel3.util.experimental.loadMemoryFromFileInline
import math._

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
    val mymem = Module(new MemRom(19, 24, "/home/xs/ysyx/ysyx-workbench/nvboard/picture.hex"))
    mymem.io.addr := Cat(vgaCtrl.io.hAddr, vgaCtrl.io.vAddr)
    vgaCtrl.io.pclk := clock
    vgaCtrl.io.vgaData := mymem.io.out
    io.vgaHsync := vgaCtrl.io.hsync
    io.vgaVsync := vgaCtrl.io.vsync
    io.vgaBlank := vgaCtrl.io.valid
    io.vgaR := vgaCtrl.io.vgaR
    io.vgaG := vgaCtrl.io.vgaG
    io.vgaB := vgaCtrl.io.vgaB
}

class MemRom(addr_width: Int, out_width: Int, init_file: String) extends AbsRom(addr_width, out_width) {
    val mem = SyncReadMem(max_num, UInt(out_width.W))
    loadMemoryFromFileInline(mem, init_file)
    // This prevents deduping this memory module
    // Ref. https://github.com/chipsalliance/firrtl/issues/2168
    val dedupBlock = WireInit(mem.hashCode.U)

    io.out := mem.read(io.addr)
}

class AbsRom(addr_width: Int, out_width: Int) extends Module {
    val io = IO(new RomBundle(addr_width, out_width))
    val max_num = pow(2, addr_width).toInt
    val DAC_max = 255
}

class RomBundle(addr_width: Int, out_width: Int) extends Bundle {
    val addr = Input(UInt(addr_width.W))
    val out = Output(UInt(out_width.W))
}
