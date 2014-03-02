#include <u.h>
#include <libc.h>
#include <stdio.h>
#include "processor.h"

void main() {
   
   processor proc;
   setupprocessor(&proc);
   installprogram(&proc);
   while(cycle(&proc));
   shutdownprocessor(&proc);
   exits(0);
}
void shutdownprocessor(processor* proc) {
   free(proc->memory);
}
instruction retrieveinstruction(processor* proc) {
   return (instruction)proc->memory[proc->gpr[ProgramCounter]];
}
void incrementprogramcounter(processor* proc) {
   add(proc, ProgramCounter, ProgramCounter, TrueRegister);
   modop(proc, ProgramCounter, ProgramCounter, CellCountRegister);
}
int instructionexecutable(processor* proc, instruction inst) {
   return proc->gpr[inst.predicate];
}
int cycle(processor* proc) {
   int shouldincrementprogramcounter;
   instruction a, b;
   shouldincrementprogramcounter = 1;
   a = retrieveinstruction(proc);
   /* check to see if we should execute */
   if(instructionexecutable(proc, a)) {
      switch(a.id) {
         case NopInstruction:
            nop(proc);
            break;
         case AddInstruction:
            add(proc, a.destination0, a.source0, a.source1);
            break;
         case SubInstruction:
            sub(proc, a.destination0, a.source0, a.source1);
            break;
         case MulInstruction:
            mul(proc, a.destination0, a.source0, a.source1);
            break;
         case DivInstruction:
            divop(proc, a.destination0, a.source0, a.source1);
            break;
         case RightShiftInstruction:
            rightshift(proc, a.destination0, a.source0, a.source1);
            break;
         case LeftShiftInstruction:
            leftshift(proc, a.destination0, a.source0, a.source1);
            break;
         case BinaryOrInstruction:
            binaryor(proc, a.destination0, a.source0, a.source1);
            break;
         case BinaryAndInstruction:
            binaryand(proc, a.destination0, a.source0, a.source1);
            break;
         case BinaryNotInstruction:
            binarynot(proc, a.destination0, a.source0);
            break;
         case EqualsInstruction:
            equals(proc, a.destination0, a.destination1, a.source0, a.source1);
            break;
         case NotEqualsInstruction:
            notequals(proc, a.destination0, a.destination1, a.source0, a.source1);
            break;
         case GreaterThanInstruction:
            greaterthan(proc, a.destination0, a.destination1, a.source0, a.source1);
            break;
         case LessThanInstruction:
            lessthan(proc, a.destination0, a.destination1, a.source0, a.source1);
            break;
         case LoadInstruction:
            load(proc, a.destination0, a.source0);
            break;
         case StoreInstruction:
            store(proc, a.destination0, a.source0);
            break;
         case BranchInstruction:
            /* we need to grab the next cell */
            incrementprogramcounter(proc);
            b = retrieveinstruction(proc);
            branch(proc, b.value);
            shouldincrementprogramcounter = 0;
            break;
         case SetInstruction:
            /* this is an interesting case */
            incrementprogramcounter(proc);
            b = retrieveinstruction(proc);
            set(proc, a.destination0, b.value);
            shouldincrementprogramcounter = 0;
            break;
         case ModInstruction:
            modop(proc, a.destination0, a.source0, a.source1);
            break;
         case CallInstruction:
            call(proc, a.source0);
            shouldincrementprogramcounter = 0;
            break;
         case RetInstruction:
            ret(proc);
            break;
         case PlatformCallInstruction:
            platformcall(proc);
            break;
         case TerminateInstruction:
         default:
            return 0;
      }
   } 
   if(shouldincrementprogramcounter) {
      incrementprogramcounter(proc);
      switch(a.id) {
         /* make sure that we don't interpret the second byte */
         case BranchInstruction:
         case SetInstruction:
            incrementprogramcounter(proc);
         default:
            break;
      }
   }
   return 1;
}
void add(processor* proc, uchar dest, uchar src0, uchar src1) {
   proc->gpr[dest] = proc->gpr[src0] + proc->gpr[src1];	
}

void sub(processor* proc, uchar dest, uchar src0, uchar src1) {
   proc->gpr[dest] = proc->gpr[src0] - proc->gpr[src1];	
}
void mul(processor* proc, uchar dest, uchar src0, uchar src1) {
   proc->gpr[dest] = proc->gpr[src0] * proc->gpr[src1];	
}
void divop(processor* proc, uchar dest, uchar src0, uchar src1) {
   proc->gpr[dest] = proc->gpr[src0] / proc->gpr[src1];	
}
void rightshift(processor* proc, uchar dest, uchar src0, uchar src1) {
   proc->gpr[dest] = proc->gpr[src0] >> proc->gpr[src1];	
}
void leftshift(processor* proc, uchar dest, uchar src0, uchar src1) {
   proc->gpr[dest] = proc->gpr[src0] << proc->gpr[src1];	
}
void binaryor(processor* proc, uchar dest, uchar src0, uchar src1) {
   proc->gpr[dest] = proc->gpr[src0] | proc->gpr[src1];	
}
void binaryand(processor* proc, uchar dest, uchar src0, uchar src1) {
   proc->gpr[dest] = proc->gpr[src0] & proc->gpr[src1];	
}
void binaryxor(processor* proc, uchar dest, uchar src0, uchar src1) {
   proc->gpr[dest] = proc->gpr[src0] ^ proc->gpr[src1];	
}
void binarynot(processor* proc, uchar dest, uchar src0) {
   proc->gpr[dest] = !(proc->gpr[src0]);	
}
void equals(processor* proc, uchar desttrue, uchar destfalse, uchar src0, uchar src1) {
   uvlong tmp;
   tmp = proc->gpr[src0] == proc->gpr[src1];	
   proc->gpr[desttrue] = tmp;
   proc->gpr[destfalse] = !tmp;
}
void notequals(processor* proc, uchar desttrue, uchar destfalse, uchar src0, uchar src1) {
   equals(proc, destfalse, desttrue, src0, src1);
}
void greaterthan(processor* proc, uchar desttrue, uchar destfalse, uchar src0, uchar src1) {
   uvlong tmp;
   tmp = proc->gpr[src0] > proc->gpr[src1];
   proc->gpr[desttrue] = tmp;
   proc->gpr[destfalse] = !tmp;
}
void lessthan(processor* proc, uchar desttrue, uchar destfalse, uchar src0, uchar src1) {
   uvlong tmp;
   tmp = proc->gpr[src0] < proc->gpr[src1];
   proc->gpr[desttrue] = tmp;
   proc->gpr[destfalse] = !tmp;
}

void load(processor* proc, uchar dest, uchar src) {
   proc->gpr[dest] = proc->memory[proc->gpr[src]];
}
void store(processor* proc, uchar dest, uchar src) {
   proc->memory[proc->gpr[dest]] = proc->gpr[src];
}

void branch(processor* proc, uvlong dest) {
   set(proc, ProgramCounter, dest % proc->gpr[CellCountRegister]);
}

void set(processor* proc, uchar dest, uvlong value) {
   proc->gpr[dest] = value;
}
void nop(processor* proc) { }

void modop(processor* proc, uchar dest, uchar src0, uchar src1) {
   proc->gpr[dest] = proc->gpr[src0] % proc->gpr[src1];	
}

void call(processor* proc, uchar dest) {
   add(proc, ReturnRegister, ProgramCounter, FalseRegister);
   branch(proc, proc->gpr[dest]);
}

void ret(processor* proc) {
   add(proc, ProgramCounter, ReturnRegister, FalseRegister);
}
void setupprocessor(processor* proc) {

   /* Upper registers have a special purpose */
   proc->gpr[FalseRegister] = 0;
   proc->gpr[TrueRegister] = 1;
   proc->gpr[ProcessorIdRegister] = processor_count++;
   proc->gpr[RegisterCountRegister] = RegisterCount;
   proc->gpr[CellCountRegister] = MemorySize;
   proc->gpr[ReturnRegister] = 0;
   /* program-counter */
   proc->gpr[ProgramCounter] = 2048;
   proc->memory = malloc(MemorySize * sizeof(uvlong));
   installexitcall(proc);
   installplatformcallhandler(proc);
   installprocessorloop(proc);
}
void installprocessorloop(processor* proc) {
   int offset;
   
   offset = proc->gpr[ProgramCounter];
   offset = encodeprintstring(proc, offset, TrueRegister, "This is simulated free software\n");
   offset = encodeprintstring(proc, offset, TrueRegister, "===============\n");
   offset = encodeprintstring(proc, offset, TrueRegister, "Fuck you Mendez.\n");
   offset = encodeprintstring(proc, offset, TrueRegister, "Fuck you Airen.\n");
   encodebranchinstruction(proc, offset, TrueRegister, 0);
}
void installplatformcallhandler(processor* proc) {
   int a;
   a = encodesetinstruction(proc, PlatformHandlerLocation, TrueRegister, PlatformScratch0, platformexit);
   a = encodeeqinstruction(proc, a, TrueRegister, PlatformTrue, PlatformFalse, PlatformFunctionCallIndex, PlatformScratch0);
   a = encodebranchinstruction(proc, a, PlatformTrue, TerminateLocation);
   a = encodesetinstruction(proc, a, PlatformFalse, PlatformScratch0, platformputc);
   a = encodeeqinstruction(proc, a, PlatformFalse, PlatformTrue, PlatformFalse, PlatformFunctionCallIndex, PlatformScratch0);
   a = encodesetinstruction(proc, a, PlatformFalse, PlatformScratch0, platformgetc);
   a = encodeeqinstruction(proc, a, PlatformFalse, PlatformTrue, PlatformFalse, PlatformFunctionCallIndex, PlatformScratch0);
   a = encodesetinstruction(proc, a, PlatformFalse, PlatformScratch0, platformerror);
   a = encodeplatforminstruction(proc, a, TrueRegister);
   encoderetinstruction(proc, a, TrueRegister);
}
void installexitcall(processor* proc) {
   instruction terminate;
   terminate.value = 0;
   terminate.id = TerminateInstruction;
   terminate.predicate = TrueRegister;
   proc->memory[TerminateLocation] = terminate.value;
}

int encodesetinstruction(processor* proc, int offset, uchar pred, uchar reg, uvlong value) {
   return encodevalue(proc, encodeinstruction(proc, offset, pred, SetInstruction, reg, 0, 0, 0, 0, 0), value);
}
int encodebranchinstruction(processor* proc, int offset, uchar pred, uvlong value) {
   return encodevalue(proc, encodeinstruction(proc, offset, pred, BranchInstruction, 0, 0, 0, 0, 0, 0), value);
}

int encodeeqinstruction(processor* proc, int offset, uchar pred, uchar d0, uchar d1, uchar s0, uchar s1) {
   return encodeinstruction(proc, offset, pred, EqualsInstruction, d0, d1, s0, s1, 0, 0);
}

int encodecallinstruction(processor* proc, int offset, uchar pred, uchar dest) {
   return encodeinstruction(proc, offset, pred, CallInstruction, 0, 0, dest, 0, 0, 0);
}

int encodeplatforminstruction(processor* proc, int offset, uchar pred) {
   return encodeinstruction(proc, offset, pred, PlatformCallInstruction, 0, 0, 0, 0, 0, 0);
}


int encoderetinstruction(processor* proc, int offset, uchar pred) {
   return encodeinstruction(proc, offset, pred, RetInstruction, 0, 0, 0, 0, 0, 0);
}

void platformcall(processor* proc) {
   switch(proc->gpr[PlatformFunctionCallIndex]) {
      case platformputc:
         proc->gpr[PlatformOutputRegister0] = putchar((char)proc->gpr[PlatformInputRegister0]);
         break;
      case platformgetc:
         proc->gpr[PlatformOutputRegister0] = getchar();
         break;
      case platformexit:
         sysfatal("Somehow platformexit was called!");
      case platformerror:
         sysfatal("A platform error occurred!");
      default:
         sysfatal("Invalid platform call occurred");
   }
}

int encodeprintchar(processor* proc, int offset, uchar pred, char value) {
   offset = encodesetinstruction(proc, offset, TrueRegister, PlatformInputRegister0, (uvlong)value);
   offset = encodesetinstruction(proc, offset, TrueRegister, PlatformFunctionCallIndex, platformputc);
   return encodeplatforminstruction(proc, offset, TrueRegister);
}

int encodeprintstring(processor* proc, int offset, uchar pred, char* value) {
   for(;*value != '\0'; ++value) {
      offset = encodeprintchar(proc, offset, pred, *value);
   }
   return offset;
}


int encodeinstruction(processor* proc, int offset, uchar predicate, uchar id, uchar dest0, uchar dest1, uchar source0, uchar source1, uchar byte6, uchar byte7) {
   instruction a;
   a.predicate = predicate;
   a.id = id;
   a.destination0 = dest0;
   a.destination1 = dest1;
   a.source0 = source0;
   a.source1 = source1;
   a.byte6 = byte6;
   a.byte7 = byte7;
   proc->memory[offset] = a.value;
   return offset + 1;
}

int encodevalue(processor* proc, int offset, uvlong value) {
   proc->memory[offset] = value;
   return offset + 1;
}
void installprogram(processor* proc) { }
