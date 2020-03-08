define load

shell /home/user/ftp -u ftp://root:"signetics"@192.168.0.30/spgate.tmp ./spgate
shell ssh -f -T root@192.168.0.30 "killall spgate || killall -9 spgate ;  killall gdbserver || killall -9 gdbserver ; mv ~/spgate.tmp ~/spgate ; chmod +x ~/spgate ; gdbserver :2332 ~/spgate"
shell sleep 2
tar rem 192.168.0.30:2332
set solib-abs /opt/compiler_platform/gcc-linaro-arm-linux-gnueabihf-4.8-2014.04_linux/arm-linux-gnueabihf/libc
break main
cont
end

handle SIG34 nostop noprint
handle SIG35 nostop noprint
