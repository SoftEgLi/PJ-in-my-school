
import sys
from PyQt5.QtWidgets import *
# from UI import Ui_MainWindow
from PyQt5.QtGui import *
from UI import Ui_MainWindow
from PyQt5.QtCore import *


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
        self.line_idx = 0
        self.mem = {}  # 数字
        self.output = {"PC": self.PC, "REG": self.Res, "MEM": self.mem, "CC": self.CC, "STAT": self.stat}
        self.lines = file
        self.Build_env()

    def trans_small_end(self, order):  # 小端转成大端,依据lab1 bitReverse的解题思想,通过分治法来实现字节位置的交换
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
        if x >= 9223372036854775808:  # 2的63次方
            x = x - 18446744073709551616
        return x


    def runline(self):              #运行一条指令
        if self.stat >= 2:
            return
        if self.line_idx >= len(self.lines):
            return
        line = self.lines[self.line_idx]
        if line[0] == ' ':
            self.line_idx = self.line_idx + 1
            self.runline()
            return
        idx2 = line.find(':')
        s_add = line[:idx2]
        add = int(s_add, 0)
        if add == self.PC and line[7] == ' ':
            self.line_idx = self.line_idx + 1
            self.runline()
            return
        if add == self.PC:
            self.process_line(line)
            self.line_idx = self.line_idx + 1
        else:
            for idx,line2 in enumerate(self.lines):
                if line2[0] == ' ' or line2[7] == ' ':
                    continue
                idx2 = line2.find(':')
                s_add2 = line2[:idx2]
                add2 = int(s_add2, 0)
                if add2 == self.PC:
                    self.line_idx = idx
                    self.runline()
                    break

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
        # print(mem)
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
            # str2 = "0x"
            for i in range(0,16-count % 16):
                str += '0'
            # str2 += str[2:]
            count = count - count % 16
            value = self.trans_small_end(str)
            if value != 0:
                self.mem[int(count / 2)] = value


    def process_line(self, line):           #处理一条指令
        if line[7] == ' ':
            return
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
            self.stat = 4              #遇到非法指令

class MyUi(QMainWindow,Ui_MainWindow):              #前端类
    def __init__(self,parent = None):
        super(MyUi,self).__init__(parent)
        self.cpu = None
        self.setupUi(self)
        self.file_text.setAcceptDrops(True)
        self.register_text.setVisible(True)
        self.mem_text.setVisible(True)
        self.set_reg.clicked.connect(self.showReg)      #设置槽函数
        self.set_mem.clicked.connect(self.showMem)
        self.fontselect.triggered.connect(self.getFont)
        self.actionopen.triggered.connect(self.openMsg)
        self.run_botton.clicked.connect(self.runline)
        self.pos_botton.clicked.connect(self.runto)
        self.file_text.textChanged.connect(self.gettext)
        self.left_picture.setPixmap(QPixmap("left2.png"))
        self.right_picture.setPixmap(QPixmap("right2.png"))
        self.file_text.setStyleSheet("background-color:rgb(255,255,255,60)")

        self.file_text.setTextColor(Qt.white)
        self.right_picture.lower()
        self.left_picture.lower()
        self.setWindowIcon(QIcon("icon.jpeg"))

    def runto(self):
        if self.pos_input.text() == "":
            return
        address = self.pos_input.text()
        while 1:
            if self.cpu.stat != 1:
                self.run_error()
                return
            self.runline()
            pc = "0x{:x}".format(self.cpu.PC)
            if address == pc:
                break


    def showReg(self):                  #展示寄存器文本框
        if self.register_text.isVisible():
            self.register_text.hide()
        else:
            self.register_text.show()

    def showMem(self):                  #展示内存文本框
        if self.mem_text.isVisible():
            self.mem_text.hide()
        else:
            self.mem_text.show()

    def getFont(self):                  #修改字体
        font,ok = QFontDialog.getFont()
        if ok:
            self.mem_text.setFont(font)
            self.register_text.setFont(font)
            self.pc_text.setFont(font)
            self.register_text.setFont(font)

    def gettext(self):                 #读取yo文本框的内容，并转化为列表，用于初始化CPU
        lines = self.file_text.toPlainText()
        lines = lines.split('\n')
        lines.pop()

        self.cpu = CPU(lines)
        self.showtext()
    def openMsg(self):                  #打开文件
        file,ok = QFileDialog.getOpenFileName(self,"打开","C:/Users/Lenovo/Desktop/ICS/ICSPJ/test","yo Files (*.yo)")
        if ok:
            with open(file) as f:
                lines = f.read()
                self.file_text.setText(lines)


    def run_error(self):                #报错
        content = ""
        if self.cpu.stat == 2:
            content = "运行已结束     "
        elif self.cpu.stat == 3:
            content = "遇到非法地址     "
        elif self.cpu.stat == 4:
            content = "遇到非法指令     "
        dialog = QMessageBox.about(self,"错误",content)

    def showtext(self):                 #展示yo文件内容
        pc = "0x{:x}".format(self.cpu.PC)
        self.pc_text.setText(pc)
        stat = "{:d}".format(self.cpu.stat)
        self.state_text.setText(stat)
        self.mem_text.clear()
        for idx, key in enumerate(self.cpu.mem):
            self.mem_text.append(f"{key}:{self.cpu.mem[key]}")

        self.register_text.clear()
        for idx, key in enumerate(self.cpu.Res):
            self.register_text.append(f"{key}:{self.cpu.Res[key]}")

    def runline(self):              #运行一条指令
        if self.cpu.stat >= 2 :
            self.run_error()
        self.cpu.runline()
        self.showtext()


if __name__ == '__main__':

    app = QApplication(sys.argv)
    Mywin = MyUi()
    Mywin.show()
    sys.exit(app.exec_())

