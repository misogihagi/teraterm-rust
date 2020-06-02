#!/bin/sh

CSI() {
  printf "\033[%s" "$1"
}

Col() {
  CSI "$1G"
}

Pos() {
  CSI "$1;$2H"
}

SGR() {
  CSI "$1m"
}

DECFRA() {
  CSI "$1;$2;$3;$4;$5\$x"
}

Clear() {
  Pos 1 1
  CSI 2J
}

InitScreen() {
#  CSI "8;24;80t"
  Clear
}

B0="0;1;4"
B1="0;32"
B2="0;45"
B3="0;38:5:49;48:5:67"

b0=$(SGR $B0)
b1=$(SGR $B1)
b2=$(SGR $B2)
b3=$(SGR $B3)

C0="0;7"
C1="0;36"
C2="0;41"
C3="0;38:5:93;48:5:49"

c0=$(SGR $C0)
c1=$(SGR $C1)
c2=$(SGR $C2)
c3=$(SGR $C3)

R=$(SGR)

InitScreen

Pos 3 6
SGR
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

Col 6
SGR $B0
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

Col 6
SGR $B1
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

Col 6
SGR $B2
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

Col 6
SGR $B3
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

Col 6
SGR
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"

echo

SGR
DECFRA 42 2 11 9 15

SGR $C0
DECFRA 45 2 21 9 25

SGR $C1
DECFRA 39 2 31 9 35

SGR $C2
DECFRA 46 2 41 9 45

SGR $C3
DECFRA 35 2 51 9 55

SGR
DECFRA 42 2 61 9 65

cat <<_EoF_

=== 正しい出力は以下 ===

          *****     $c0-----$R     $c1'''''$R     $c2.....$R     $c3#####$R     $c4*****$R
     %%%%%*****%%%%%$c0-----$R%%%%%$c1'''''$R%%%%%$c2.....$R%%%%%$c3#####$R%%%%%$c4*****$R%%%%%
     $b0%%%%%$R*****$b0%%%%%$c0-----$b0%%%%%$c1'''''$b0%%%%%$c2.....$b0%%%%%$c3#####$b0%%%%%$R*****$b0%%%%%$R
     $b1%%%%%$R*****$b1%%%%%$c0-----$b1%%%%%$c1'''''$b1%%%%%$c2.....$b1%%%%%$c3#####$b1%%%%%$R*****$b1%%%%%$R
     $b2%%%%%$R*****$b2%%%%%$c0-----$b2%%%%%$c1'''''$b2%%%%%$c2.....$b2%%%%%$c3#####$b2%%%%%$R*****$b2%%%%%$R
     $b3%%%%%$R*****$b3%%%%%$c0-----$b3%%%%%$c1'''''$b3%%%%%$c2.....$b3%%%%%$c3#####$b3%%%%%$R*****$b3%%%%%$R
     %%%%%*****%%%%%$c0-----$R%%%%%$c1'''''$R%%%%%$c2.....$R%%%%%$c3#####$R%%%%%$c4*****$R%%%%%
          *****     $c0-----$R     $c1'''''$R     $c2.....$R     $c3#####$R     $c4*****$R
_EoF_

echo
