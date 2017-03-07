;;(use-modules (ice-9 readline))
;;(activate-readline)
(progn
 (start-gnuplot)
 (turtle-reset)
 (loop 
   (format t "~%> ")
   (print (eval (read))))
 (stop-gnuplot))

