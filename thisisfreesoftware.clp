;-------------------------------------------------------------------------------
;thisisfreesoftware.clp
;Copyright (c) 2013, Joshua Scoggins 
;All rights reserved.
;
;Redistribution and use in source and binary forms, with or without
;modification, are permitted provided that the following conditions are met:
;    * Redistributions of source code must retain the above copyright
;      notice, this list of conditions and the following disclaimer.
;    * Redistributions in binary form must reproduce the above copyright
;      notice, this list of conditions and the following disclaimer in the
;      documentation and/or other materials provided with the distribution.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
;ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
;WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;DISCLAIMED. IN NO EVENT SHALL Joshua Scoggins BE LIABLE FOR ANY
;DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
;(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
;ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
;SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;-------------------------------------------------------------------------------
; So Many Ways....So Little Time!!!!
; Requires Electron and file-system....WELCOME TO MULTI LICENSE HELL!!!!
;-------------------------------------------------------------------------------
(defgeneric fuck-you)
(defmethod fuck-you
  ((?target LEXEME)
   (?rtr SYMBOL))
  (printout ?rtr "fuck you " ?target crlf))

(defmethod fuck-you
  ((?target LEXEME))
  (fuck-you ?target t))

(deffacts what-the-fuck
	  (message (to mendez)
		   (from airen)
		   (action fuck-you)))

(defrule statement:fuck-you 
	 ?f <- (message (to ?a)
			(from ?b)
			(action fuck-you))
	 =>
	 (printout t (format nil "%s : " ?b))
	 (fuck-you ?a)
	 (modify ?f (to ?b)
		 (from ?a)
		 (action no-fuck-you)
		 (contents 0)))

(defrule statement:no-fuck-you
	 ?f <- (message (to ?a)
			(from ?b)
			(action no-fuck-you)
			(contents ?counter&:(< ?counter 100)))
	 (test (< ?counter 100))
	 =>
	 (printout t (format nil "%s : no, " ?b))
	 (fuck-you ?a)
	 (modify ?f (to ?b)
		 (from ?a)
		 (contents (+ ?counter 1))))

(defrule statement:finished-conversation
	 ?f <- (message (action no-fuck-you)
			(contents 100))
	 =>
	 (retract ?f))

(defrule statement:ending-conversations
	 (declare (salience -1))
	 =>
	 (printout t "Ending Conversation" crlf))



; Execution command :D
; $ElectronFSShell/bin/electron -f2 $ElectronFSShell/sys/fs.clp -f2 $ElectronFSShell/logic/thisisfreesoftware.clp -f2 $ElectronFSShell/sys/chunks/reset-run-exit.clp
