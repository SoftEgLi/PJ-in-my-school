
import operator

import yaml
import ruamel_yaml
import sys


class CPU:
    def __init__(self, file):
        self.NumRes = {'0': "rax", '1': "rcx", '2': "rdx", "3": "rbx", "4": 'rsp', "5": "rbp",
                       "6": "rsi", "7": "rdi", "8": 'r8', "9": "r9", "a": 'r10', "b": 'r11', "c": 'r12',
                       "d": 'r13', "e": 'r14'}
        self.Res = {'rax': 0, 'rcx': 0, 'rdx': 0, 'rbx': 0, 'rsp': 0, 'rbp': 0, 'rsi': 0,
                    'rdi': 0, 'r8': 0, 'r9': 0, 'r10': 0, 'r11': 0, 'r12': 0, 'r13': 0, 'r14': 0}
        self.CC = {'ZF': 1, 'SF': 0, 'OF': 0}
        self.PC = 0
        self.stat = 1
        self.mem = {}  # 数字
        self.output = {"PC": self.PC, "REG": self.Res, "MEM": self.mem, "CC": self.CC, "STAT": self.stat}
        self.lines = file
        self.writer = ruamel_yaml.YAML()
        self.writer.indent(mapping=2, sequence=2, offset=0)

    def trans_small_end(self, order):  # 小端转成大端,依据lab1 bitReverse的解题思想,采用分治法，通过位运算实现
        mask0 = 0xffffffffffffffff
        mask1 = 0x00000000ffffffff
        mask2 = 0x0000ffff0000ffff
        mask3 = 0x00ff00ff00ff00ff
        x = int(order, 0)
        x = ((x >> 32) & mask1) | ((x & mask1) << 32)
        x = x & mask0
        x = ((x >> 16) & mask2) | ((x & mask2) << 16)
        x = x & mask0
        x = ((x >> 8) & mask3) | ((x & mask3) << 8)
        x = x & mask0
        if x >= 9223372036854775808:  # x大于2的63次方，就转为负数
            x = x - 18446744073709551616
        return x

    def create_yaml(self):          #输出yaml文件
        self.mem = dict(sorted(self.mem.items(), key=operator.itemgetter(0)))
        self.output = {"PC": self.PC, "REG": self.Res, "MEM": self.mem, "CC": self.CC, "STAT": self.stat}
        output_list = [self.output]
        self.writer.dump(output_list, sys.stdout)       #sys.stdout是重定向输出文件

    def run(self):              #开始运行
        self.Build_env()
        i = 0
        while 1:
            if self.stat >= 2:          #发现异常
                break
            if i >= len(self.lines):
                break
            line = self.lines[i]
            if line[0] == ' ':          #空行
                i = i + 1
                continue
            idx2 = line.find(':')
            s_add = line[:idx2]
            address = int(s_add, 0)
            if address == self.PC and line[7] == ' ':   #空行
                i = i + 1
                continue
            if address == self.PC:          #PC与该行一致，则正常执行
                self.process_line(line)
                self.create_yaml()
                i = i + 1
            else:                       #遇到call等指令导致PC与该行不一致，定位到PC所在位置
                j = 0
                if address < self.PC:   #如果PC大于当前的指令地址，就从该行向后寻找，否则从头寻找
                    j = i+1

                temp = j
                for line2 in self.lines[temp:]:
                    if line2[0] == ' ' or line2[7] == ' ':
                        j = j + 1
                        continue
                    idx2 = line2.find(':')
                    s_add2 = line2[:idx2]
                    address2 = int(s_add2, 0)
                    if address2 == self.PC:
                        i = j
                        break
                    j = j + 1

    def Build_env(self):            #创建环境，把字节序列存储到内存中。
        mem = ""
        for line in self.lines:
            if line[0] == ' ':
                continue
            idx1 = line.find(":")
            line_mem = line[:idx1]
            address = int(line_mem, 0)
            if 2 * address > len(mem):  # 一个字节对应2个数字
                for i in range(len(mem), 2 * address):
                    mem += '0'
            for j in range(7, 27):  # 添加内存
                if line[j] == ' ':
                    break
                mem += line[j]
        count = 0
        str = "0x"
        for c in mem:
            str += c
            count = count + 1
            if count % 16 == 0 and count != 0:
                value = self.trans_small_end(str)
                # print(value)
                str = "0x"
                if value != 0:
                    self.mem[int(count / 2) - 8] = value

        if count % 16 !=0:
            for i in range(0,16-count % 16):
                str += '0'
            count = count - count % 16
            value = self.trans_small_end(str)
            if value != 0:
                self.mem[int(count / 2)] = value

    def process_line(self, line):           #处理一条指令
        idx1 = line.find(':')
        idx2 = line.find('|')
        order = line[idx1 + 2:idx2]
        if order[0] == '0':
            self.stat = 2
            return
        if order[0] == '1':
            self.PC = self.PC + 1
            return
        if order[0] == '2':
            if order[1] == '0':  # rrmoveq
                self.Res[self.NumRes[order[3]]] = self.Res[self.NumRes[order[2]]]
                self.PC = self.PC + 2
                return

            if order[1] == '1':  # Cmovle
                if self.CC['ZF'] == 1 or self.CC['SF'] == 1:
                    self.Res[self.NumRes[order[3]]] = self.Res[self.NumRes[order[2]]]
                self.PC = self.PC + 2
                return

            if order[1] == '2':  # cmovl
                if self.CC['SF'] == 1:
                    self.Res[self.NumRes[order[3]]] = self.Res[self.NumRes[order[2]]]
                self.PC = self.PC + 4
                return

            if order[1] == '3':   #cmove
                if self.CC['ZF'] == 1:
                    self.Res[self.NumRes[order[3]]] = self.Res[self.NumRes[order[2]]]
                self.PC = self.PC + 2
                return

            if order[1] == '4':     #cmovne
                if self.CC['ZF'] == 0:
                    self.Res[self.NumRes[order[3]]] = self.Res[self.NumRes[order[2]]]
                self.PC = self.PC + 2
                return

            if order[1] == '5':     #comvge
                if self.CC['SF'] == 0:
                    self.Res[self.NumRes[order[3]]] = self.Res[self.NumRes[order[2]]]
                self.PC = self.PC + 2
                return

            if order[1] == '6':             #cmovg
                if self.CC['SF'] == 0 and self.CC['ZF'] == 0 :
                    self.Res[self.NumRes[order[3]]] = self.Res[self.NumRes[order[2]]]
                self.PC = self.PC + 2
                return


        if order[0] == '3':  # irmovq
            s_instance = "0x"
            s_instance = s_instance + order[4:20]
            instance = self.trans_small_end(s_instance)
            self.Res[self.NumRes[order[3]]] = instance
            self.PC = self.PC + 10
            return

        if order[0] == '4':  # rmmovq
            s_instance = "0x"
            s_instance = s_instance + order[4:20]
            instance = self.trans_small_end(s_instance)
            if order[3] == 'f':
                address = '0x{:x}'.format(instance)
            else:
                address = instance + self.Res[self.NumRes[order[3]]]

            self.mem[address] = self.Res[self.NumRes[order[2]]]

            self.PC = self.PC + 10
            return

        if order[0] == '5':  # mrmovq
            s_offset = "0x"
            s_offset += order[4:20]
            offset = self.trans_small_end(s_offset)
            address = offset
            if order[3] != 'f':
                address += self.Res[self.NumRes[order[3]]]
            # print("Here")
            # print(address)
            self.Res[self.NumRes[order[2]]] = self.mem.get(address,0)

            self.PC = self.PC + 10
            return

        if order[0] == '6':
            if order[1] == '0':  # addq
                sum = self.Res[self.NumRes[order[2]]] + self.Res[self.NumRes[order[3]]]
                if sum < 0:
                    self.CC['SF'] = 1
                else:
                    self.CC['SF'] = 0
                if self.Res[self.NumRes[order[2]]] < 0 and self.Res[self.NumRes[order[3]]] < 0:
                    self.CC['OF'] = 1
                    sum = sum + 18446744073709551616
                # elif sum >= 18446744073709551616:  # 2的64次方
                #     self.CC['CF'] = 1
                #     sum = sum - 18446744073709551616
                else:
                    self.CC['OF'] = 0
                if sum == 0:
                    self.CC['ZF'] = 1
                else:
                    self.CC['ZF'] = 0
                self.Res[self.NumRes[order[3]]] = sum
                self.PC = self.PC + 2
                return

            if order[1] == '1':  # subq
                sub = self.Res[self.NumRes[order[3]]] - self.Res[self.NumRes[order[2]]]

                if sub < 0:
                    self.CC['SF'] = 1
                else:
                    self.CC['SF'] = 0
                if sub == 0:
                    self.CC['ZF'] = 1
                else:
                    self.CC['ZF'] = 0
                if (self.Res[self.NumRes[order[2]]] < 0 and self.Res[self.NumRes[order[3]]] < 0 and sub < 0) \
                        or (self.Res[self.NumRes[order[3]]] > 0 and self.Res[self.NumRes[order[2]]] < 0):
                    self.CC['OF'] = 1
                else:
                    self.CC['OF'] = 0
                self.Res[self.NumRes[order[3]]] = sub
                self.PC = self.PC + 2
                return

            if order[1] == '2':  # andq
                andq = self.Res[self.NumRes[order[3]]] & self.Res[self.NumRes[order[2]]]

                if andq == 0:
                    self.CC['ZF'] = 1
                else:
                    self.CC['ZF'] = 0
                if andq < 0:
                    self.CC['SF'] = 1
                else:
                    self.CC['SF'] = 0

                self.CC['OF'] = 0
                self.Res[self.NumRes[order[3]]] = andq
                self.PC = self.PC + 2
                return

            if order[1] == '3':  # xorq
                xorq = self.Res[self.NumRes[order[3]]] ^ self.Res[self.NumRes[order[2]]]

                if xorq == 0:
                    self.CC['ZF'] = 1
                else:
                    self.CC['ZF'] = 0
                if xorq < 0:
                    self.CC['SF'] = 1
                else:
                    self.CC['SF'] = 0
                self.CC['OF'] = 0
                self.Res[self.NumRes[order[3]]] = xorq
                self.PC = self.PC + 2
                return

        if order[0] == '7':
            if order[1] == '0':  # jmp
                s_address = "0x"
                s_address += order[2:18]
                address = self.trans_small_end(s_address)
                self.PC = address
                return

            if order[1] == '1':  # jle jl je jne jge jg
                if ((self.CC['SF'] ^ self.CC['OF']) or self.CC['ZF']):
                    s_address = "0x"
                    s_address += order[2:18]
                    address = self.trans_small_end(s_address)
                    self.PC = address
                    return
                else:
                    self.PC = self.PC + 9
                    return

            if order[1] == '2':  # jl
                if self.CC['SF'] ^ self.CC['OF']:
                    s_address = "0x"
                    s_address += order[2:18]
                    address = self.trans_small_end(s_address)
                    self.PC = address
                    return
                else:
                    self.PC = self.PC + 9
                    return

            if order[1] == '3':  # je
                if self.CC['ZF']:
                    s_address = "0x"
                    s_address += order[2:18]
                    address = self.trans_small_end(s_address)
                    self.PC = address
                    return
                else:
                    self.PC = self.PC + 9
                    return

            if order[1] == '4':  # jne
                if not self.CC['ZF']:
                    s_address = "0x"
                    s_address += order[2:18]
                    address = self.trans_small_end(s_address)
                    self.PC = address
                    return
                else:
                    self.PC = self.PC + 9
                    return

            if order[1] == '5':  # jge
                if not (self.CC['SF'] ^ self.CC['OF']):
                    s_address = "0x"
                    s_address += order[2:18]
                    address = self.trans_small_end(s_address)
                    self.PC = address
                    return
                else:
                    self.PC = self.PC + 9
                    return

            if order[1] == '6':  # jg
                if not (self.CC['SF'] ^ self.CC['OF']) & (not self.CC['ZF']):
                    s_address = "0x"
                    s_address += order[2:18]
                    address = self.trans_small_end(s_address)
                    self.PC = address
                    return
            else:
                self.PC = self.PC + 9
                return

        if order[0] == '8':  # call
            self.Res["rsp"] = self.Res["rsp"] - 8
            self.mem[self.Res["rsp"]] = self.PC + 9
            s_address = "0x"
            s_address += order[2:18]
            address = self.trans_small_end(s_address)
            # print(address)
            self.PC = address
            return

        if order[0] == '9':
            self.PC = self.mem.get(self.Res["rsp"],0)
            self.Res["rsp"] = self.Res["rsp"] + 8
            return

        if order[0] == 'a':         #push
            if self.Res["rsp"] - 8 < 0 :
                self.stat = 3
                self.Res["rsp"] = self.Res["rsp"] - 8
                return

            if self.Res[self.NumRes[order[2]]] != 0:
                self.mem[self.Res["rsp"] - 8] = self.Res[self.NumRes[order[2]]]
            self.Res["rsp"] = self.Res["rsp"] - 8

            self.PC = self.PC + 2
            return

        if order[0] == 'b':         #pop
            self.Res["rsp"] = self.Res["rsp"] + 8
            self.Res[self.NumRes[order[2]]] = self.mem.get(self.Res["rsp"] - 8, 0)

            self.PC = self.PC + 2
            return

        if order[0] == 'c':             #iaddq
            s_instance = "0x"
            s_instance += order[4:20]
            instance = self.trans_small_end(s_instance)
            sum = self.Res[self.NumRes[order[3]]] + instance
            if sum == 0 :
                self.CC['ZF'] = 1
            else:
                self.CC['ZF'] = 0

            if sum < 0 :
                self.CC['SF'] = 1
            else:
                self.CC['SF'] = 0

            if self.Res[self.NumRes[order[3]]] < 0 and instance < 0 :
                self.CC['OF'] = 1
            else:
                self.CC['OF'] = 0
            self.Res[self.NumRes[order[3]]] = self.Res[self.NumRes[order[3]]] + instance
            self.PC = self.PC + 10

            return
        else:
            self.stat = 4               #遇到非法指令




if __name__ == '__main__':

    cpu = CPU(sys.stdin.readlines())
    cpu.run()

