; Theoretical Architecture Platform Caller
; @ index 2
; Let's check the PlatformFunctionCallIndexRegister (pfci)
; If we hit zero then exit

(true) set ps0 <- 0
(true) eq ptrue pfalse <- pfci ps0
(ptrue) branch 0
(pfalse) set ps0 <- 1
(pfalse) eq ptrue pfalse <- pfci ps0
(pfalse) set ps0 <- 2
(pfalse) eq ptrue pfalse <- pfci ps0
(pfalse) set pfci <- 255
(true) platform
(true) ret
