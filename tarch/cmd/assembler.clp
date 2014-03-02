(load* (proton: /lib/core.clp))
(defglobal MAIN
           ?*instruction-list* = (create$ nop add sub mul div mod rightshift 
                                          leftshift binaryand binaryor 
                                          binarynot eq neq gt lt load store 
                                          branch set call ret)

           ?*registers* = (create$ r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13
                                   r14 r15 r16 r17 r18 r19 r20 r21 r22 r23 r24 
                                   r25 r26 r27 r28 r29 r30 r31 r32 r33 r34 r35 
                                   r36 r37 r38 r39 r40 r41 r42 r43 r44 r45 r46 
                                   r47 r48 r49 r50 r51 r52 r53 r54 r55 r56 r57 
                                   r58 r59 r60 r61 r62 r63 r64 r65 r66 r67 r68 
                                   r69 r70 r71 r72 r73 r74 r75 r76 r77 r78 r79 
                                   r80 r81 r82 r83 r84 r85 r86 r87 r88 r89 r90 
                                   r91 r92 r93 r94 r95 r96 r97 r98 r99 r100 
                                   r101 r102 r103 pi3 pi2 pi1 pi0 pfalse ptrue 
                                   ps7 ps6 ps5 ps4 ps3 ps2 ps1 ps0 po0 po1 pfci 
                                   ra pc cc rc pid true false))
(defgeneric translate/instruction)
(defgeneric translate/register)

(defmethod translate/instruction
  ((?index INTEGER (= ?index 254)))
  platform)
(defmethod translate/instruction
  ((?index INTEGER (= ?index 255)))
  terminate)


(defmethod translate/instruction
  ((?index INTEGER))
  (nth$ ?index ?*instruction-list*))

(defmethod translate/instruction
  ((?symbol LEXEME (eq ?symbol platform)))
  254)
(defmethod translate/instruction
  ((?symbol LEXEME (eq ?symbol terminate)))
  255)

(defmethod translate/instruction
  ((?symbol LEXEME))
  (member$ ?symbol ?*instruction-list*))

(defmethod translate/register
  ((?index INTEGER))
  (nth$ ?index ?*registers*))

(defmethod translate/register
  ((?symbol LEXEME))
  (member$ ?symbol ?*registers*))

; syntax of an instruction is 
; predicate op dest0 dest1 <- src0 src1
; predicate op dest0 <- src0 src1
; predicate op dest0 <- value
; predicate op dest0
; predicate op <- value
; predicate op

(defrule parse-input
         (initial-fact)
         =>
         (assert (parse input 1 (readline))))

(defrule done-parsing
         ?f <- (parse input ? EOF)
         =>
         (retract ?f))

(defrule continue-parsing
         ?f <- (parse input ?n ?line&~EOF)
         =>
         (retract ?f)
         (assert (instruction ?n (explode$ ?line))
                 (parse input (+ ?n 1) (readline))))

(defrule eliminate-parens
         (declare (salience 3))
         ?f <- (instruction ?n "(" ?predicate ")" $?rest)
         =>
         (retract ?f)
         (assert (instruction ?n ?predicate $?rest)))
(defrule parse-dual-result-instruction
         (declare (salience 2))
         ?f <- (instruction ? ?predicate ?op ?d0 ?d1 <- ?s0 ?s1)
         (test (not (neq ?op eq neq gt lt)))
         =>
         (retract ?f)
         (put-char t (translate/register ?predicate))
         (put-char t (translate/instruction ?op))
         (put-char t (translate/register ?d0))
         (put-char t (translate/register ?d1))
         (put-char t (translate/register ?s0))
         (put-char t (translate/register ?s1))
         (put-char t 0)
         (put-char t 0))

(defrule parse-single-result-instruction:dual-input
         (declare (salience 2))
         ?f <- (instruction ? ?predicate ?op ?d0 <- ?s0 ?s1)
         (test (not (neq ?op add sub mul div mod rightshift 
                         leftshift binaryor binaryand)))
         =>
         (retract ?f)
         (put-char t (translate/register ?predicate))
         (put-char t (translate/instruction ?op))
         (put-char t (translate/register ?d0))
         (put-char t 0)
         (put-char t (translate/register ?s0))
         (put-char t (translate/register ?s1))
         (put-char t 0)
         (put-char t 0))

(defrule parse-single-result:single-input:binarynot
         (declare (salience 2))
         ?f <- (instruction ? ?predicate ?op ?dest <- ?value)
         (test (not (neq ?op load store binarynot)))
         =>
         (retract ?f)
         (put-char t (translate/register ?predicate))
         (put-char t (translate/instruction binarynot))
         (put-char t (translate/register ?dest))
         (put-char t 0)
         (put-char t (translate/register ?value))
         (put-char t 0)
         (put-char t 0)
         (put-char t 0))

(defrule parse-single:set
         (declare (salience 2))
         ?f <- (instruction ? ?predicate set ?dest <- ?value)
         =>
         (retract ?f)
         (put-char t (translate/register ?predicate))
         (put-char t (translate/instruction set))
         (put-char t (translate/register ?dest))
         (put-char t 0)
         (put-char t 0)
         (put-char t 0)
         (put-char t 0)
         (put-char t 0)
         (progn$ (?a (slice8 ?value)) 
                 (put-char t ?a)))

(defrule parse-none
         (declare (salience 2))
         ?f <- (instruction ? ?predicate ?op)
         (test (not (neq ?op nop ret platform terminate)))
         =>
         (retract ?f)
         (put-char t (translate/register ?predicate))
         (put-char t (translate/instruction ?op))
         (put-char t 0)
         (put-char t 0)
         (put-char t 0)
         (put-char t 0)
         (put-char t 0)
         (put-char t 0))

(defrule parse-call
         (declare (salience 2))
         ?f <- (instruction ? ?predicate call ?reg)
         =>
         (retract ?f)
         (put-char t (translate/register ?predicate))
         (put-char t (translate/instruction call))
         (put-char t 0)
         (put-char t 0)
         (put-char t (translate/register ?reg))
         (put-char t 0)
         (put-char t 0)
         (put-char t 0))

(defrule parse-branch
         (declare (salience 2))
         ?f <- (instruction ? ?predicate branch ?value)
         =>
         (retract ?f)
         (put-char t (translate/register ?predicate))
         (put-char t (translate/instruction branch))
         (put-char t 0)
         (put-char t 0)
         (put-char t 0)
         (put-char t 0)
         (put-char t 0)
         (put-char t 0)
         (progn$ (?a (slice8 ?value)) 
                 (put-char t ?a)))
(defrule skip-empty-lines
         (declare (salience 3))
         ?f <- (instruction ?)
         =>
         (retract ?f))
(defrule unknown-instruction 
         (declare (salience 1))
         ?f <- (instruction ?line $?symbols) 
         =>
         (retract ?f)
         (format werror "ERROR: unknown instruction on line %d%n%t%s%n" ?line 
                 (implode$ $?symbols))
         (exit 1))


;TODO: continue
(batch* (proton: /lib/reset-run-exit.clp))
