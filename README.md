# kcalc
Live variable calculation through bash (using bc)

Based on bc

1) Put your equations in "calculations" using this syntax:
VAR=3*x

2) Run calc to calculate

The script automatically assigns results to the variable and replaces it in subsequent equations.
If a variable is not defined, it will render as zero.
