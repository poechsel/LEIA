#!/usr/bin/env python3

import sys, inspect
from functools import reduce
import os.path
import argparse


class _Erreur:
    """
    Represents an error
    """
    def __init__(self, sender, obj, error_type):
        """
        An error is attached to an instruction and has a location (line, path). 
        """
        self.file_path = sender.path
        self.file_line = sender.file_line
        self.instr_type = type(sender).__name__.lower().replace("custom", ".")
        self.obj = obj
        self.error_type = error_type
    
    def __repr__(self):
        return "[Error] in file {} on line {} instruction {}: {}, {}".format(self.file_path, self.file_line, self.instr_type, self.obj, self.error_type)
    
    def __str__(self):
        return self.__repr__()


class _Instruction:
    """
    The global class representing an instruction. It has all the methods enabling to parse a line
    """
    def __init__(self, s, file_path, file_line, real_line, args_nb = 3, jump_line = 1):
        """
        We track informations about the location
        self.args_nb and self.jump_line are here for the parsing
        """
        self.words = s
        self.path = file_path
        self.file_line = file_line
        self.line = real_line
        self.jump_line = jump_line
        self.args_nb = args_nb
    
    def to_num(self, word):
        """
        tries to convert a word to a number. If fails, return an error
        numbers can be bin/hex/dec/char
        """
        if word[:2] == '0x':
            try:
                return int(word, 16)
            except:
                return [_Erreur(self, "Number", "hexadecimal misdefined")]
        elif word[:2] == '0b':
            try:
                return int(word, 2)
            except:
                return [_Erreur(self, "Number", "binary misdefined")]
        elif len(word) >= 3 and ((word[0] == '\'' and word[2] == '\'') or (word[0] == '"' and word[2] == '"')):
            try:    
                return ord(word[1])
            except:
                return [_Erreur(self, "Number", "character misdefined")]
        try:
            return int(word)
        except:
            return [_Erreur(self, "Number", "number misdefined")]

    def check_convert_num(self, number, bits, m, M, type_incoming = "Number"):
        """ 
        check if a number is in a range and compute the two complement if needed
        Return an error if needed
        """
        if number < m or number > M:
            return [_Erreur(self, type_incoming, "Not in bound: [{}, {}]".format(m, M))]
        if number < 0:
            return (1<<bits) + number
        return number

    def read_register(self, word, is_destination = False):
        """
        Read a register and check if it is a correct one
        """
        if word[0] != "r":
            return [_Erreur(self, "Register", "this isn't a register")]
        number = self.to_num(word[1:])
        if not isinstance(number, list):
            return self.check_convert_num(number, None, 0, 7 if is_destination else 15, "Register")
    
    def read_destination(self, word):
        """
        Read a "destination"
        """
        if len(word) < 2 or (word[0] != '[' and word[-1] != ']'):
            return [_Erreur(self, "Destination", "this isn't a correct destination")]
        temp = self.read_register(word[1:-1])
        return temp

    def getValue(self, word, number = None, register = None, destination = None):
        """ 
        tries to read a number, a register or a destination.
        Return an error if it fails to read one
        number -> (number of bits, minimum, maximum)
        register -> (is_destination)
        destination -> ()
        """
        out = []
        out_message = "("
        c = True
        if not number is None:
            out_message += "Number, "
            temp = self.to_num(word)
            #this is a trick use very often. A list here can only represents a list of error
            if not isinstance(temp, list):
                temp = self.check_convert_num(temp, number[0], number[1], number[2])
                if isinstance(temp, list):
                    out = temp
                    c = False
                else:
                    return ("n", temp)
            else:
                out += temp
        if c and not register is None:
            out_message += "Register, "
            temp = self.read_register(word, register[0])
            if isinstance(temp, list):
                out += temp
            else:
                return ("r", temp)
        if c and not destination is None:
            out_message += "Destination, "
            temp = self.read_destination(word)
            if isinstance(temp, list):
                out += temp
            else:
                return ("d", temp)
        return out + [_Erreur(self, "Type", "This argument should be a {})".format(out_message))]

    def _toOpcode(self, current, env):
        """
        if there isn't any errors we compute the opcode for this line. Otherwise output the errors
        """
        if any((isinstance(c, _Erreur) for c in current)):
            for c in current:
                if isinstance(c, _Erreur):
                    print(c)
            return None
        else:
            return reduce(lambda a, b: a | b, [a<<i for a, i in current])
    
    def getOpcodes(self, env):
        """
        return the opcode of current instruction
        Check if we have enough arguments
        """
        if len(self.words) <= self.args_nb:
            print(_Erreur(self, "Arguments number", "not enough"))
            return None
        alls = self.parse(env)
        ops = [self._toOpcode(a, env) for a in alls]
        if any (o is None for o in ops):
            return None
        return [op for op in ops]

    def addThing(self, thing, nb):
        """
        Add a part to be converted
        the parts are in the form (bitcode, offset in the opcode of this bitcode
        """
        if isinstance(thing, list):
            return thing
        if isinstance(thing, tuple):
            thing = thing[1]
        return [(thing, nb)]
    
    def parse(self, env):
        return []


class CustomInclude(_Instruction):
    """
    represents an include
    """
    def __init__(self, *args):
        super(CustomInclude, self).__init__(*args)
        self.args_nb = 1

class CustomReserve(_Instruction):
    """
    represents a .reserve instruction
    syntax: .reserve n -> put n null words in memory
    """
    def __init__(self, *args):
        super(CustomReserve, self).__init__(*args)
        self.jump_line = max(0, self.to_num(self.words[1]))

    def getOpcodes(self, env):
        return [0] * self.jump_line


#code of the print function we added for debugging purpose
class Print(_Instruction):
    def __init__(self, *args):
        super(Print, self).__init__(*args)
        self.jump_line = len(self.words[1]) - 1
        if self.words[1][0] == 'r':
            self.jump_line = 1
        self.args_nb = 1
    def parse(self, env):
        if self.words[1][0] == 'r':
            return [self.addThing(0b1110, 12) + self.addThing(1, 10) + self.addThing(self.read_register(self.words[1]), 0)]
        else:
            o = []
            for c in self.words[1][1:-1]:
                o += [self.addThing(0b1110, 12) + self.addThing(2, 10) + self.addThing(ord(c), 0)]
            return o + [self.addThing(0b1110, 12) + self.addThing(2, 10) + self.addThing(ord('\n'), 0)]


class CustomString(_Instruction):
    """
    represents a .string instruction
    syntax: .string "string"
    """
    def __init__(self, *args):
        super(CustomString, self).__init__(*args)
        self.word = " ".join(self.words[1:]) # a word can have some spaces in it. /!\ that means a ';' is put into the word
        self.jump_line = len(self.word) - 2 + 1 #a word has n+2 letter (because of \" to mark the begin and the end of the word) + 1 for the \0 char 
    
    def getOpcodes(self, env):
        if not (self.word[0] == "\"" and self.word[-1] == "\""):
            print(_Erreur(self, "String", "Incorrect format"))
            return []
        return [ord(c) for c in self.word[1:-1]] + [0]


class CustomWord(_Instruction):
    """
    represents a .word instruction
    syntax: .word number
    """
    def __init__(self, *args):
        super(CustomWord, self).__init__(*args)
        self.args_nb = 1
    
    def parse(self, env):
        return [self.addThing(self.to_num(self.words[1]), 0)]


class CustomAlign16(_Instruction):
    """
    .align16 instruction
    """
    def __init__(self, *args):
        super(CustomAlign16, self).__init__(*args)
        self.args_nb = 0
        self.jump_line = 0
        while ((self.line + self.jump_line) % 16) != 0:
            self.jump_line += 1
    
    def parse(self, env):
        c = []
        for _ in range(self.jump_line):
            c += Copy(["copy", "r0", "r0"], "", "", "").parse(env)
        return c

class CustomLet(_Instruction):
    """
    .let instruction
    enables to affect a value to any register
    """
    def __init__(self, *args):
        super(CustomLet, self).__init__(*args)
        self.args_nb = 2
        #we could optimize the let by only adding a leth when needed. 
        self.jump_line = 2
    def parse(self, env):
        n = self.to_num(self.words[2])
        r = self.read_register(self.words[1])
        if not isinstance(n, list):
            return [self.addThing(0b1100, 12) + self.addThing(r, 8) + self.addThing(n & 0x00FF, 0), \
                    self.addThing(0b1101, 12) + self.addThing(r, 8) + self.addThing((n & 0xFF00) >> 8, 0)]
        return [n]


class CustomSet(_Instruction):
    """
    .set instruction
    Syntax: .set register label -> r0 = adress of label
    """
    def __init__(self, *args):
        super(CustomSet, self).__init__(*args)
        self.args_nb = 2
        self.jump_line = 2
    
    def parse(self, env):
        reg = self.read_register(self.words[1])
        if self.words[2] in env.labels:
            lbl = env.labels[self.words[2]]
            return [self.addThing(0b1100, 12) + self.addThing(reg, 8) + self.addThing(lbl & 0x00FF, 0), \
                    self.addThing(0b1101, 12) + self.addThing(reg, 8) + self.addThing((lbl & 0xFF00) >> 8, 0)]
        else:
            return [[_Erreur(self, "Label", "not defined")]]


class Push(_Instruction):
    """
    push instruction
    syntax: push register -> push register on the stack
    """
    def __init__(self, *args):
        super(Push, self).__init__(*args)
        self.args_nb = 1
        self.jump_line = 1
    
    def parse(self, env):
        return [self.addThing(0b00001, 11) + self.addThing(self.read_register(self.words[1], False), 0)]


class Pop(_Instruction):
    """
    pop instruction
    syntax: pop register -> pop the top of the stack in register
    """
    def __init__(self, *args):
        super(Pop, self).__init__(*args)
        self.args_nb = 1
        self.jump_line = 1
    
    def parse(self, env):
        return [self.addThing(0b1111, 12) + self.addThing(0b1000, 4) + self.addThing(self.read_register(self.words[1], False), 0)]

class CustomPush(_Instruction):
    """
    pop instruction
    syntax: pop register -> pop the top of the stack in register
    """
    def __init__(self, *args):
        super(CustomPush, self).__init__(*args)
        self.args_nb = 1
        self.jump_line = 2
    
    def parse(self, env):
        op = "add" if env.reverse_stack else "sub"
        return Wmem(["wmem", self.words[1], "[r7]"], "", "", "").parse(env) + Add([op, "r7", "r7", "1"], "", "", "").parse(env)

class CustomPop(_Instruction):
    def __init__(self, *args):
        super(CustomPop, self).__init__(*args)
        self.args_nb = 1
        self.jump_line = 2
    
    def parse(self, env):
        op = "sub" if env.reverse_stack else "add"
        return Sub([op, "r7", "r7", "1"], "", "", "").parse(env) + Rmem(["rmem", self.words[1], "[r7]"], "", "", "").parse(env)


class Letl(_Instruction):
    """
    letl instruction, refer to the isa
    """
    def __init__(self, *args):
        super(Letl, self).__init__(*args)
        self.args_nb = 2
    
    def parse(self, env):
        r = self.read_register(self.words[1], False)
        v = self.getValue(self.words[2], (8, -127, 255))
        return [self.addThing(0b1100, 12) + self.addThing(r, 8) + self.addThing(v, 0)]


class Leth(_Instruction):
    """
    leth instruction, refer to the isa
    """
    def __init__(self, *args):
        super(Leth, self).__init__(*args)
        self.args_nb = 2

    def parse(self, env):
        r = self.read_register(self.words[1], False)
        v = self.getValue(self.words[2], (8, -127, 255))
        return [self.addThing(0b1101, 12) + self.addThing(r, 8) + self.addThing(v, 0)]


class Rmem(_Instruction):
    """
    rmem instruction, refer to the isa
    """
    def __init__(self, *args):
        super(Rmem, self).__init__(*args)
        self.args_nb = 2

    def parse(self, env):
        r = self.read_register(self.words[1], False)
        v = self.read_destination(self.words[2])
        return [self.addThing(0b1111, 12) + self.addThing(r, 8) + self.addThing(0, 4) + self.addThing(v, 0)]


class Copy(_Instruction):
    """
    copy instruction, refer to the isa
    """
    def __init__(self, *args):
        super(Copy, self).__init__(*args)
        self.args_nb = 2

    def parse(self, env):
        r = self.read_register(self.words[1], False)
        v = self.read_register(self.words[2])
        return [self.addThing(0b1111, 12) + self.addThing(r, 8) + self.addThing(1, 4) + self.addThing(v, 0)]


class Refresh(_Instruction):
    """
    refresh instruction
    ask the screen refresh and wait for the end of the frame
    """
    def __init__(self, *args):
        super(Refresh, self).__init__(*args)
        self.args_nb = 0
    
    def parse(self, env):
        return [self.addThing(0b1110, 12) + self.addThing(0, 0)]


class Wmem(_Instruction):
    """
    wmem instruction, refer to the isa
    """
    def __init__(self, *args):
        super(Wmem, self).__init__(*args)
        self.args_nb = 2

    def parse(self, env):
        r = self.read_register(self.words[1], False)
        v = self.read_destination(self.words[2])
        return [self.addThing(0b0000, 12) + self.addThing(0, 8) + self.addThing(r, 4) + self.addThing(v, 0)]


class _AluInstr(_Instruction):
    """
    represents a alu instruction
    """
    def __init__(self, *args, opcode, m = 0, M = 15):
        super(_AluInstr, self).__init__(*args)
        self.args_nb = 3
        self.opcode = opcode
        self.m = m
        self.M = M
    
    def parse(self, env):
        op = self.opcode
        t = 0
        r_dest = self.read_register(self.words[1], True)
        r_f = self.read_register(self.words[2])
        r_l = self.getValue(self.words[3], (4, self.m, self.M), (False,))
        if isinstance(r_l, tuple):
            t = (r_l[0] == "n")
            r_l = r_l[1]
        return [self.addThing(self.opcode, 12) + self.addThing(t, 11) + self.addThing(r_dest, 8) + self.addThing(r_f, 4) + self.addThing(r_l, 0)]
        

class Jump(_Instruction):
    """
    jump instruction, refer to the isa
    """
    def __init__(self, *args):
        super(Jump, self).__init__(*args)
        self.args_nb = 1
    
    def parse(self, env):
        temp = self.to_num(self.words[1])
        if isinstance(temp, list):
            if not self.words[1] in env.labels:
                return [[_Erreur(self, "Label", "not defined")]]
            temp = env.labels[self.words[1]] - self.line           
        temp = self.check_convert_num(temp, 12, -2048, 2047, type_incoming = "Jump")
        if isinstance(temp, list):
            return [[_Erreur(self, "destination", "out of reach")]]
        return [self.addThing(0b1011, 12) + self.addThing(temp, 0)]


class Return(_Instruction):
    """
    return instruction, refer to the isa
    """
    def __init__(self, *args):
        super(Return, self).__init__(*args)
        self.args_nb = 0
    
    def parse(self, env):
        return [self.addThing(0b1011, 12) + self.addThing(1, 0)]


class Call(_Instruction):
    """ 
    call instruction, refer to the isa
    """
    def __init__(self, *args):
        super(Call, self).__init__(*args)	
        self.args_nb = 1

    def parse(self, env):
        temp = self.to_num(self.words[1])
        if isinstance(temp, list):
            try:
                temp = env.labels[self.words[1]]
                env.called_labels.append(self.words[1])
            except:
                return [[_Erreur(self, "Label", "not defined")]]
        return [self.addThing(0b1010, 12) + self.addThing(temp >> 4, 0)]


class Snif(_Instruction):
    """
    snif instruction, refer to the isa
    """
    def __init__(self, *args):
        super(Snif, self).__init__(*args)
        self.args_nb = 3

    def parse(self, env):
        comp = ["eq", "neq", "sgt", "slt", "gt", "ge", "lt", "le"]
        try:
            comp = comp.index(self.words[2])
        except:
            comp = [_Erreur(self, "Comparison", "this operator doesn't exist")]
        t = 0
        r_dest = self.read_register(self.words[1], False)
        r_l = self.getValue(self.words[3], (4, -8, 7), (False,))
        if isinstance(r_l, tuple):
            t = r_l[0] == "n"
            r_l = r_l
        return [self.addThing(0b0011, 12) + self.addThing(t, 11) + self.addThing(comp, 8) + self.addThing(r_dest, 4) + self.addThing(r_l, 0)]


class Add(_AluInstr):
    def __init__(self, *args):
        super(Add, self).__init__(*args, opcode = 0b0001)


class Sub(_AluInstr):
    def __init__(self, *args):
        super(Sub, self).__init__(*args, opcode = 0b0010)
class And(_AluInstr):
    def __init__(self, *args):
        super(And, self).__init__(*args, opcode = 0b0100, m = -8, M = 7)


class Or(_AluInstr):
    def __init__(self, *args):
        super(Or, self).__init__(*args, opcode = 0b0101, m = -8, M = 7)


class Xor(_AluInstr):
    def __init__(self, *args):
        super(Xor, self).__init__(*args, opcode = 0b0110, m = -8, M = 7)


class Lsl(_AluInstr):
    def __init__(self, *args):
        super(Lsl, self).__init__(*args, opcode = 0b0111)


class Lsr(_AluInstr):
    def __init__(self, *args):
        super(Lsr, self).__init__(*args, opcode = 0b1000)


class Asr(_AluInstr):
    def __init__(self, *args):
        super(Asr, self).__init__(*args, opcode = 0b1001)



class _Environment:
    """
    represents the current environment during parsing
    Contain the current line, the instructions already parsed and the label set
    """
    def __init__(self):
        self.line = 0
        self.labels = {}
        self.called_labels = []
        self.instr = []
        self.instr_set = {}
        self.already_loaded = {}
        self.reverse_stack = False
        #a small introspection trick. We get all the instruction on the fly. 
        #If the name begins with _ we don't add it. Then, the name of the 
        #instructions correspond to the name of the class lowered and when 
        #their is 'custom' in it we replace it by '.'
        for name, obj in inspect.getmembers(sys.modules[__name__]):
            if inspect.isclass(obj) and "_" != name[0]:
                self.instr_set[name.lower().replace("custom", ".")] = obj
    
import re
def load_file(file_path):
    """ 
    read a file and return the list of lines as words
    """
    print("assembling", file_path)
    with open(file_path) as file:
        #we remove blank chars from the begin and end of the string
        #then lower the string to avoid ambiguities
        #then put a space between comments to avoid problems
        #then split it to get the words
        PATTERN = re.compile(r'''((?:[^\s"']|"[^"]*"|'[^']*')+)''')
        o = []
        for l, line in enumerate(file):
            nl = PATTERN.split(line.strip().replace(";", " ;"))[1::2]
            if line.strip() != '':
                nl[0] = nl[0].lower()
                if len(nl) and nl[0] not in [".string", ".word"]:
                    for i in range(1, len(nl)):
                        nl[i] = nl[i].lower()
                o.append((l, nl))
        #return [(l, PATTERN.split(line.strip().lower().replace(";", " ;"))[1::2]) for l, line in enumerate(file) if line.strip() != '']
        return o
    return None



def first_pass(path, f, env):
    #in the first pass we compute the label offsets, add the instruction to the set and load included files
    dir_path = os.path.dirname(path)
    env.already_loaded[os.path.abspath(path)] = True;
    for l, line in f:
        if line[0][-1] == ":":
            if line[0] in env.labels:
                print("[Error], Label alread defined (line {}) in file {}".format(l+1, path))
            else:
                env.labels[line[0][:-1]] = env.line
        elif line[0] in env.instr_set:
            env.instr += [env.instr_set[line[0]](line, path, l+1,  env.line)]
            env.line += env.instr[-1].jump_line
        elif line[0] == "#include":
            abs_path_loaded = os.path.abspath(os.path.join(dir_path, line[1]))
            if abs_path_loaded not in env.already_loaded:
                first_pass(abs_path_loaded, load_file(abs_path_loaded), env)
        else:
            if line[0][0] != ';':
                print("[Error], Undefined instruction (ligne {}) in file {}: {}".format(l+1, path, line))
            pass

def second_pass(env):
    """
    compile the instructions to their opcodes and return a list of text
    """
    instr = [l.getOpcodes(env) for l in env.instr]
    return None if None in instr else [j for i in instr for j in i]

def write_debug_file (env, path):
    f, _ = os.path.splitext(path)
    debug_file = f + ".debug"
    labels = []
    seen = {}
    """
    for l in env.called_labels :
        if not l in seen :
            labels.append((l, env.labels[l]))
            seen[l] = True
    """
    labels = [(k, env.labels[k]) for k in env.labels]
    labels.sort(key = lambda x : x[1])
    with open(debug_file, "w") as out:
        for l, i in labels:
            out.write(l + " " + str(i) + "\n")

if __name__ == '__main__':
    stack_direction = 0
    parser = argparse.ArgumentParser(description='Assemble a LEIA asm file')
    parser.add_argument("path", help = "path of the file to assemble")
    parser.add_argument("-r", "--reverse_stack", action="store_true", help  = "reverse the direction in which the stack grows")
    parser.add_argument("-o", "--out", help = "select output file")
    parser.add_argument("-d", "--debug", action = "store_true", help = "write a debuging file")
    args = parser.parse_args()

    if args.path != "":
        f = load_file(os.path.abspath(args.path))
        if f is None:
            print("Input file wasn't able to be loaded")
        env = _Environment()
        env.reverse_stack = args.reverse_stack
        first_pass(args.path, f, env)
        code = second_pass(env)
        if code is None:
            print("No code was generated")
        else:
            out_path = args.out
            if out_path is None:
                file_name, _ = os.path.splitext(os.path.basename(args.path))
                out_path = file_name + ".obj"
            with open(out_path, "w") as out:
                for c in code:
                    out.write(str(hex(c))[2:].zfill(4) + "\n")
            if args.debug:
                write_debug_file(env, out_path)

    else:
        print("Please input a file")
             
