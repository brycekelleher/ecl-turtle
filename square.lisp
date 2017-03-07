(defun draw-square ()
  (progn
    (turtle-move  3)
    (turtle-turn 90)
    (turtle-move  3)
    (turtle-turn 90)
    (turtle-move  3)
    (turtle-turn 90)
    (turtle-move  3)
    (turtle-turn 90)))

(defun draw-test (angle da)
  (if (< angle 360)
    (progn (draw-square)
           (turtle-turn da)
           (draw-test (+ angle da) da))))

