package ScProcessor
package IDU
import chisel3._
import chisel3.util._
import scala.collection.immutable
import scala.collection.mutable
import IDUInfo._
import IDUInsInfo._
import utils.tools._
import basic._
import immGet._

object IDUInsInfo {
    val instructions = Map[String, IsBitPat] (
        "addi" -> new InsStruct("0010011", "000", "-1"),
        // "slti" -> new InsStruct("0010011", "010", "-1"),
        "sltiu" -> new InsStruct("0010011", "011", "-1"),
        "andi" -> new InsStruct("0010011", "111", "-1"),
        "ori" -> new InsStruct("0010011", "110", "-1"),
        "xori" -> new InsStruct("0010011", "100", "-1"),
        "addiw" -> new InsStruct("0011011", "000", "-1"),
        "slli" -> new InsStruct("0010011", "001", "-1", "000000"),
        "srli" -> new InsStruct("0010011", "101", "-1", "000000"),
        "srai" -> new InsStruct("0010011", "101", "-1", "010000"),
        "slliw" -> new InsStruct("0011011", "001", "0000000"),
        "srliw" -> new InsStruct("0011011", "101", "0000000"),
        "sraiw" -> new InsStruct("0011011", "101", "0100000"),
        "lui" -> new InsStruct("0110111", "-1", "-1"),
        "auipc" -> new InsStruct("0010111", "-1", "-1"),
        "add" -> new InsStruct("0110011", "000", "0000000"),
        "sub" -> new InsStruct("0110011", "000", "0100000"),
        "slt" -> new InsStruct("0110011", "010", "0000000"),
        "sltu" -> new InsStruct("0110011", "011", "0000000"),
        "and" -> new InsStruct("0110011", "111", "0000000"),
        "or" -> new InsStruct("0110011", "110", "0000000"),
        "xor" -> new InsStruct("0110011", "100", "0000000"),
        "addw" -> new InsStruct("0111011", "000", "0000000"),
        "subw" -> new InsStruct("0111011", "000", "0100000"),
        "sll" -> new InsStruct("0110011", "001", "0000000"),
        "srl" -> new InsStruct("0110011", "101", "0000000"),
        // "sra" -> new InsStruct("0110011", "101", "0100000"),
        "sllw" -> new InsStruct("0111011", "001", "0000000"),
        "srlw" -> new InsStruct("0111011", "101", "0000000"),
        "sraw" -> new InsStruct("0111011", "101", "0100000"),
        "jal" -> new InsStruct("1101111", "-1", "-1"),
        "jalr" -> new InsStruct("1100111", "000", "-1"),
        "beq" -> new InsStruct("1100011", "000", "-1"),
        "bne" -> new InsStruct("1100011", "001", "-1"),
        "blt" -> new InsStruct("1100011", "100", "-1"),
        "bltu" -> new InsStruct("1100011", "110", "-1"),
        "bge" -> new InsStruct("1100011", "101", "-1"),
        "bgeu" -> new InsStruct("1100011", "111", "-1"),
        "ld" -> new InsStruct("0000011", "011", "-1"),
        "lw" -> new InsStruct("0000011", "010", "-1"),
        "lwu" -> new InsStruct("0000011", "110", "-1"),
        "lh" -> new InsStruct("0000011", "001", "-1"),
        "lhu" -> new InsStruct("0000011", "101", "-1"),
        "lb" -> new InsStruct("0000011", "000", "-1"),
        "lbu" -> new InsStruct("0000011", "100", "-1"),
        "sd" -> new InsStruct("0100011", "011", "-1"),
        "sw" -> new InsStruct("0100011", "010", "-1"),
        "sh" -> new InsStruct("0100011", "001", "-1"),
        "sb" -> new InsStruct("0100011", "000", "-1"),
        // control insts
        "ebreak" -> new InsStruct("1110011", "000", "0000000"),
        "ecall"  -> new BitPatDec("b000000000000_00000_000_00000_1110011"),
        "mret"   -> new BitPatDec("b0011000_00010_00000_000_00000_1110011"),

        "mul" -> new InsStruct("0110011", "000", "0000001"),
        // "mulh" -> new InsStruct("0110011", "001", "0000001"),
        // "mulhu" -> new InsStruct("0110011", "011", "0000001"),
        // "mulhsu" -> new InsStruct("0110011", "010", "0000001"),
        "div" -> new InsStruct("0110011", "100", "0000001"),
        "divu" -> new InsStruct("0110011", "101", "0000001"),
        // "rem" -> new InsStruct("0110011", "110", "0000001"),
        "remu" -> new InsStruct("0110011", "111", "0000001"),
        "mulw" -> new InsStruct("0111011", "000", "0000001"),
        "divw" -> new InsStruct("0111011", "100", "0000001"),
        "divuw" -> new InsStruct("0111011", "101", "0000001"),
        "remw" -> new InsStruct("0111011", "110", "0000001"),
        "remuw" -> new InsStruct("0111011", "111", "0000001"),
        "ebreak"-> new BitPatDec("b000000000001_00000_000_00000_1110011"),
        // zicsr instructions
        "csrrw" -> new BitPatDec("b????????????_?????_001_?????_1110011"),
        "csrrs" -> new BitPatDec("b????????????_?????_010_?????_1110011"),
    )

    val aluOpsMap = immutable.Map(
        // you must obey the rule: [String] -> Array([String],...)
        // if there is nothing in the Array, emmmm, it is invalid
        // "SUM" -> Array("addi", "add")
        "SUM" -> Array("addi", "lui", "auipc", "add"),
        "UCMP"-> Array("sltiu", "sltu"),
        "SCMP"-> Array("slt"),
        "WSUM"-> Array("addiw", "addw"),
        "WSUB"-> Array("subw"),
        "SUB" -> Array("sub", "beq", "bne"),
        "XOR" -> Array("xori", "xor"),
        "AND" -> Array("and", "andi"),
        "OR"  -> Array("ori", "or"),
        "ULSW"-> Array("slliw", "sllw"),
        "URSW"-> Array("srliw", "srlw"),
        "SRSW"-> Array("sraiw", "sraw"),
        "SRS" -> Array("srai"),
        "ULS" -> Array("slli", "sll"),
        "URS" -> Array("srli", "srl"),
        "MULW"-> Array("mulw"),
        "MUL" -> Array("mul"),
        "DIV" -> Array("div"),
        "DIVU"-> Array("divu"),
        "REMU"-> Array("remu"),
        "DIVW"-> Array("divw"),
        "DIVUW"-> Array("divuw"),
        "REMW"-> Array("remw"),
        "REMUW"->Array("remuw"),
    )
    val aluOps = MapKeyToArray(aluOpsMap)

    val exuOpsMap = immutable.Map(
        "r1Im" -> Array("addi", "sltiu", "andi", "ori", "xori", 
        "addiw", "slli", "srli", "srai", "slliw", "srliw", "sraiw"),
        "imX0" -> Array("lui"),
        "imPc" -> Array("auipc"),
        "r1R2" -> Array("add", "sub", "slt", "sltu", "and", "or", "xor",
        "addw", "subw", "sll", "srl",
        "sllw", "srlw", "sraw", "beq", "bne", "blt",
        "mul", "div", "divu", "remu", 
        "mulw", "divw", "divuw", "remw", "remuw")
    )
    val exuOps = MapKeyToArray(exuOpsMap)

    val rfOpsMap = immutable.Map(
        "pcn" -> Array("jal", "jalr"),
        "mem" -> Array("ld", "lw", "lwu", "lh", "lhu", "lbu", "lb"),
        "csr" -> Array("csrrw", "csrrs"),
    )
    val rfOps = MapKeyToArray(rfOpsMap)

    // todo: delete Inc
    val pcOpsMap = immutable.Map(
        "Jal" -> Array("jal"),
        "Jalr"-> Array("jalr"),
        "beq" -> Array("beq"),
        "bne" -> Array("bne"),
        "bltu"-> Array("bltu"),
        "bge" -> Array("bge"),
        "blt" -> Array("blt"),
        "bgeu"-> Array("bgeu"),
        "ecall"-> Array("ecall"),
        "mret"-> Array("mret"),
    )
    val pcOps = MapKeyToArray(pcOpsMap)

    val ctrlsMap = immutable.Map(
        "nwrEn" -> Array("beq", "bne", "bge", "bgeu", "blt", "bltu",
        "sw", "sd", "sh", "sb", "ecall", "mret"),
        "csrWr" -> Array("csrrw", "csrrs"),
    )
    val ctrls = MapKeyToArray(ctrlsMap)

    val immSwitchMap = immutable.Map(
        "immI"  -> Array("addi", "sltiu", "andi", "ori", "xori", 
        "addiw", "slli", "srli", "srai", "slliw", "sraiw", "srliw", "jalr", 
        "ld", "lw", "lwu", "lh", "lhu", "lb", "lbu"),
        "immU"  -> Array("lui", "auipc"),
        "immJ"  -> Array("jal"),
        "immS"  -> Array("sd", "sw", "sh", "sb"),
        "immB"  -> Array("beq", "bne", "blt", "bltu", "bge", "bgeu"),
        "immCsr"-> Array("csrrw", "csrrs"),
    )
    val immSwitch = MapKeyToArray(immSwitchMap)

    val memOpsMap = immutable.Map(
        "ld"    -> Array("ld"),
        "lw"    -> Array("lw"),
        "lwu"   -> Array("lwu"),
        "lh"    -> Array("lh"),
        "lhu"   -> Array("lhu"),
        "lb"    -> Array("lb"),
        "lbu"   -> Array("lbu"),
        "sd"    -> Array("sd"),
        "sw"    -> Array("sw"),
        "sh"    -> Array("sh"),
        "sb"    -> Array("sb")
    )
    val memOps = MapKeyToArray(memOpsMap)

    val csrOpsMap = immutable.Map(
        "csrrw" -> Array("csrrw"),
        "csrrs" -> Array("csrrs"),
        "ecall" -> Array("ecall"),
        "mret"  -> Array("mret"),
    )
    val csrOps = MapKeyToArray(csrOpsMap)
}