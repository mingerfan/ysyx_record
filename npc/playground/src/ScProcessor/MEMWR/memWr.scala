package ScProcessor
package MEMWR
import chisel3._
import chisel3.util._
import topInfo._
import tools._
import MEMWRInfo._

object MEMWRInfo {
    val OPS_NUM = IDU.IDUInsInfo.memOps.length
}

class MEM_WR extends BlackBox {
    val io = IO(new Bundle {
        val clk   = Input(Bool())
        val raddr = Input(UInt(XLEN.W))
        val rdata = Output(UInt(XLEN.W))
        val waddr = Input(UInt(XLEN.W))
        val wdata = Input(UInt(XLEN.W))
        val wmask = Input(UInt((XLEN/8).W))
        val en   = Input(Bool())
    })
}

class MEMWR extends Module {
    val io = IO(new Bundle {
        val rs1 = Input(UInt(XLEN.W))
        val rs2 = Input(UInt(XLEN.W))
        val rd  = Output(UInt(XLEN.W))
        val imm = Input(UInt(XLEN.W))
        val memOps = Input(UInt(OPS_NUM.W))
    })
    val hit = U_HIT_CURRYING(io.memOps, IDU.IDUInsInfo.memOps)_

    val mem_wr_in = Module(new MEM_WR)

    val taddr = WireDefault(io.rs1 + io.imm)
    val taddr_t = taddr& (~"h7".U(64.W))

    val read_data = mem_wr_in.io.rdata

    val align = taddr-taddr_t
    val dword_mask = "b1111_1111".U
    val word_mask = "b1111".U << align(4, 0)
    val hword_mask = "b11".U << align(4, 0)
    val byte_mask = "b1".U << align(4, 0)
    
    val word_vec = Wire(Vec(2, Bool()))
    word_vec := VecInit((0 until 2).map(i => align === (i*4).U))
    val hword_vec = Wire(Vec(4, Bool()))
    hword_vec := VecInit((0  until 4).map(i => align === (i*2).U))
    val byte_vec = Wire(Vec(8, Bool()))
    byte_vec := VecInit((0 until 8).map(i => align === i.U))

    val word_data = Mux1H(Seq(
        word_vec(0) -> read_data(31, 0),
        word_vec(1) -> read_data(63, 32)
    ))
    val hword_data = Mux1H(Seq(
        hword_vec(0) -> read_data(15, 0),
        hword_vec(1) -> read_data(31, 16),
        hword_vec(2) -> read_data(47, 32),
        hword_vec(3) -> read_data(63, 48)
    ))
    val byte_data = Mux1H(Seq(
        byte_vec(0) -> read_data(7, 0),
        byte_vec(1) -> read_data(15, 8),
        byte_vec(2) -> read_data(23, 16),
        byte_vec(3) -> read_data(31, 24),
        byte_vec(4) -> read_data(39, 32),
        byte_vec(5) -> read_data(47, 40),
        byte_vec(6) -> read_data(55, 48),
        byte_vec(7) -> read_data(63, 56)
    ))

    io.rd := Mux1H(Seq(
        hit("ld") -> (read_data),
        hit("lw") -> (Fill(topInfo.XLEN - 11, word_data(31)) ## 
        word_data(30, 0))
    ))

    mem_wr_in.io.clk   := clock.asBool
    mem_wr_in.io.raddr := taddr_t
    mem_wr_in.io.waddr := taddr_t
    mem_wr_in.io.wdata := Mux1H(Seq(
        hit("sd") -> io.rs2,
        hit("sh") -> io.rs2(15, 0),
        hit("sb") -> io.rs2(7, 0)
    ))
    mem_wr_in.io.wmask := Mux1H(Seq(
        hit("sd") -> dword_mask,
        hit("sh") -> hword_mask,
        hit("sb") -> byte_mask,
        hit("ld") -> 0.U,
        hit("lw") -> 0.U
    ))
    mem_wr_in.io.en := io.memOps.asTypeOf(Vec(OPS_NUM, Bool())).reduceTree(_ | _)
}