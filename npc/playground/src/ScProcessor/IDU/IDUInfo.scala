package ScProcessor
package IDU
import chisel3._
import chisel3.util._
import scala.collection.immutable
import scala.collection.mutable
import IDUInfo._
import IDUInsInfo._
import tools._
import basic._
import myutil._
import immGet._

object IDUInsInfo {
    val instructions = Map(
        "addi" -> new InsStruct("0010011", "000", "-1"),
        // "slti" -> new InsStruct("0010011", "010", "-1"),
        "sltiu" -> new InsStruct("0010011", "011", "-1"),
        // "andi" -> new InsStruct("0010011", "111", "-1"),
        // "ori" -> new InsStruct("0010011", "110", "-1"),
        // "xori" -> new InsStruct("0010011", "100", "-1"),
        "addiw" -> new InsStruct("0011011", "000", "-1"),
        // "slli" -> new InsStruct("0010011", "001", "-1", "000000"),
        // "srli" -> new InsStruct("0010011", "101", "-1", "000000"),
        // "srai" -> new InsStruct("0010011", "101", "-1", "010000"),
        // "slliw" -> new InsStruct("0011011", "001", "0000000"),
        // "srliw" -> new InsStruct("0011011", "101", "0000000"),
        // "sraiw" -> new InsStruct("0011011", "101", "0100000"),
        "lui" -> new InsStruct("0110111", "-1", "-1"),
        "auipc" -> new InsStruct("0010111", "-1", "-1"),
        "add" -> new InsStruct("0110011", "000", "0000000"),
        "sub" -> new InsStruct("0110011", "000", "0100000"),
        // "slt" -> new InsStruct("0110011", "010", "0000000"),
        // "sltu" -> new InsStruct("0110011", "011", "0000000"),
        // "and" -> new InsStruct("0110011", "111", "0000000"),
        // "or" -> new InsStruct("0110011", "110", "0000000"),
        // "xor" -> new InsStruct("0110011", "100", "0000000"),
        "addw" -> new InsStruct("0111011", "000", "0000000"),
        // "subw" -> new InsStruct("0111011", "000", "0100000"),
        // "sll" -> new InsStruct("0110011", "001", "0000000"),
        // "srl" -> new InsStruct("0110011", "101", "0000000"),
        // "sra" -> new InsStruct("0110011", "101", "0100000"),
        // "sllw" -> new InsStruct("0111011", "001", "0000000"),
        // "srlw" -> new InsStruct("0111011", "101", "0000000"),
        // "sraw" -> new InsStruct("0111011", "101", "0100000"),
        "jal" -> new InsStruct("1101111", "-1", "-1"),
        "jalr" -> new InsStruct("1100111", "000", "-1"),
        "beq" -> new InsStruct("1100011", "000", "-1"),
        "bne" -> new InsStruct("1100011", "001", "-1"),
        // "blt" -> new InsStruct("1100011", "100", "-1"),
        // "bltu" -> new InsStruct("1100011", "110", "-1"),
        // "bge" -> new InsStruct("1100011", "101", "-1"),
        // "bgeu" -> new InsStruct("1100011", "111", "-1"),
        "ld" -> new InsStruct("0000011", "011", "-1"),
        "lw" -> new InsStruct("0000011", "010", "-1"),
        // "lwu" -> new InsStruct("0000011", "110", "-1"),
        // "lh" -> new InsStruct("0000011", "001", "-1"),
        // "lhu" -> new InsStruct("0000011", "101", "-1"),
        // "lb" -> new InsStruct("0000011", "000", "-1"),
        // "lbu" -> new InsStruct("0000011", "100", "-1"),
        "sd" -> new InsStruct("0100011", "011", "-1"),
        // "sw" -> new InsStruct("0100011", "010", "-1"),
        "sh" -> new InsStruct("0100011", "001", "-1"),
        "sb" -> new InsStruct("0100011", "000", "-1"),
        "ebreak" -> new InsStruct("1110011", "000", "0000000"),
        // "mul" -> new InsStruct("0110011", "000", "0000001"),
        // "mulh" -> new InsStruct("0110011", "001", "0000001"),
        // "mulhu" -> new InsStruct("0110011", "011", "0000001"),
        // "mulhsu" -> new InsStruct("0110011", "010", "0000001"),
        // "div" -> new InsStruct("0110011", "100", "0000001"),
        // "divu" -> new InsStruct("0110011", "101", "0000001"),
        // "rem" -> new InsStruct("0110011", "110", "0000001"),
        // "remu" -> new InsStruct("0110011", "111", "0000001"),
        // "mulw" -> new InsStruct("0111011", "000", "0000001"),
        // "divw" -> new InsStruct("0111011", "100", "0000001"),
        // "divuw" -> new InsStruct("0111011", "101", "0000001"),
        // "remw" -> new InsStruct("0111011", "110", "0000001"),
        // "remuw" -> new InsStruct("0111011", "111", "0000001")
    )

    val aluOpsMap = immutable.Map(
        // you must obey the rule: [String] -> Array([String],...)
        // if there is nothing in the Array, emmmm, it is invalid
        // "SUM" -> Array("addi", "add")
        "SUM" -> Array("addi", "lui", "auipc", "add"),
        "UCMP"-> Array("sltiu"),
        "WSUM"-> Array("addiw", "addw"),
        "SUB" -> Array("sub", "beq", "bne")
    )
    val aluOps = MapKeyToArray(aluOpsMap)

    val exuOpsMap = immutable.Map(
        "r1Im" -> Array("addi", "sltiu", "addiw"),
        "imX0" -> Array("lui"),
        "imPc" -> Array("auipc"),
        "r1R2" -> Array("add", "sub", "addw", "beq", "bne")
    )
    val exuOps = MapKeyToArray(exuOpsMap)

    val rfOpsMap = immutable.Map(
        "exu" -> Array("addi", "sltiu", "addiw", "lui", "auipc",
        "add", "sub", "addw"),
        "pcn" -> Array("jal", "jalr"),
        "mem" -> Array("ld", "lw")
    )
    val rfOps = MapKeyToArray(rfOpsMap)

    // todo: delete Inc
    val pcOpsMap = immutable.Map(
        "Inc" -> Array("addi", "sltiu", "addiw", "lui", "auipc", 
        "add", "sub", "addw", "ld", "lw", "sd", "sh", "sb"),
        "Jal" -> Array("jal"),
        "Jalr"-> Array("jalr"),
        "beq" -> Array("beq"),
        "bne" -> Array("bne")
    )
    val pcOps = MapKeyToArray(pcOpsMap)

    val ctrlsMap = immutable.Map(
        "nwrEn" -> Array("beq", "bne")
    )
    val ctrls = MapKeyToArray(ctrlsMap)

    val immSwitchMap = immutable.Map(
        "immI"  -> Array("addi", "sltiu", "addiw", "jalr", "ld", 
        "lw"),
        "immU"  -> Array("lui", "auipc"),
        "immJ"  -> Array("jal"),
        "immR"  -> Array("sub", "addw"),
        "immS"  -> Array("sd"),
        "immB"  -> Array("beq", "bne")
    )
    val immSwitch = MapKeyToArray(immSwitchMap)

    val memOpsMap = immutable.Map(
        "ld"    -> Array("ld"),
        "lw"    -> Array("lw"),
        "sd"    -> Array("sd"),
        "sh"    -> Array("sh"),
        "sb"    -> Array("sb")
    )
    val memOps = MapKeyToArray(memOpsMap)
}