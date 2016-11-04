#
scp piyush@condesa.cc.gt.atl.ga.us:~/approxGraphComp/debug/"$1".dot "$1".dot
dot -Tpdf  "$1".dot >> graph.pdf
open graph.pdf 