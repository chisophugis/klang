
Sequence Diagram
================

**input code**::

  extern putchard(char)
  def printstar(n)
    for i = 1, i < n, 1.0 in
      putchard(42);  # ascii 42 = '*'
  
  # print 100 '*' characters
  printstar(100);

This code is introduced in 5.3 'for' Loop Expression. <http://llvm.org/docs/tutorial/LangImpl5.html#for-loop-expression>

**Call Sequences in UML**

.. image:: _static/KlangSequenceDiagram.jpg

