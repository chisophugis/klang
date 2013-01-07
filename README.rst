About
=====
This project has been started from LLVM tutorial and being implemented to be the
same compiler - kaleidoscope - but in different structure!
The structure of klang is meant to be similar to the one of clang's.


**Purpose of this work**

This small project is one of the means of studying LLVM and Clang.

**Watch out**

This project aims to support most systems. But at this early stage, only Ubuntu
environment is supported and tested. Other linux would be OK but Mac and Windows
are not supported at this moment!

News
====
`Klang's website <http://journeyer.github.com/klang/>`_ is now open, thanks to
`the very kind helper <https://github.com/chisophugis>`_, though contents are
poor yet.


Guide
=====
**Getting the source How-To**::

  $ cd LLVM_SRC_DIR/projects
  $ git clone git://github.com/Journeyer/klang.git
  $ cd klang
  $ git fetch -u git://github.com/Journeyer/klang.git +refs/heads/*:refs/heads/*


**Branches**::

 ast ........... ch.2 Implementing a Parser and AST
 ir  ........... ch.3 Implementing Code Generation to LLVM IR
 jit ........... ch.4 Adding JIT and Optimizer Support
 cf  ........... ch.5 Extending the language: control flow
 userop ........ ch.6 Extending the language: user-defined operators
 ssa   ......... ch.7 Extending the language: mutable variables
                                               / SSA construction
 master ........ up-to-date


**Compile How-To**::

  $ git checkout master
  $ cd autoconf
  $ ./AutoRegen.sh

Makefile infrastructure of LLVM project is applied only after the branch
``cf``. For the branches before ``cf``, simple Makefile is used. To use this
simple Makefile you need to modify Config.mk with your own LLVM paths::

  $ cd ../../../
  $ (cd LLVM_SRC_DIR)
  $ vi configure
  Insert items for klang under items for sample as shown below


This step is the one I am not sure of. If anyone know about this, please share
us! This step is not documented ever!! I just found that this is necessary::

   @@ -809,6 +809,7 @@ projects/llvm-test
   projects/poolalloc
   projects/llvm-poolalloc
   projects/sample
  +projects/klang
   projects/privbracket
   projects/llvm-stacker
   projects/llvm-reopt
   @@ -3488,6 +3489,8 @@ do
   case ${i} in
   sample)       subdirs="$subdirs projects/sample"
   ;;
  +      klang)       subdirs="$subdirs projects/klang"
  +    ;;
   privbracket)  subdirs="$subdirs projects/privbracket"
   ;;
   llvm-stacker) subdirs="$subdirs projects/llvm-stacker"



  $ cd LLVM_OBJ_DIR
  $ ../llvm/configure
  $ make


Later on, you can compile klang under ``LLVM_OBJ_DIR/projects/klang``


Mandelbrot set plotted out using Klang!
=======================================
::

 $ cd Release+Asserts/bin/
 $ ./klang ../../../../../llvm/projects/klang/sample/mandel.k

 *******************************************************************************
 *******************************************************************************
 ****************************************++++++*********************************
 ************************************+++++...++++++*****************************
 *********************************++++++++.. ...+++++***************************
 *******************************++++++++++..   ..+++++**************************
 ******************************++++++++++.     ..++++++*************************
 ****************************+++++++++....      ..++++++************************
 **************************++++++++.......      .....++++***********************
 *************************++++++++.   .            ... .++**********************
 ***********************++++++++...                     ++**********************
 *********************+++++++++....                    .+++*********************
 ******************+++..+++++....                      ..+++********************
 **************++++++. ..........                        +++********************
 ***********++++++++..        ..                         .++********************
 *********++++++++++...                                 .++++*******************
 ********++++++++++..                                   .++++*******************
 *******++++++.....                                    ..++++*******************
 *******+........                                     ...++++*******************
 *******+... ....                                     ...++++*******************
 *******+++++......                                    ..++++*******************
 *******++++++++++...                                   .++++*******************
 *********++++++++++...                                  ++++*******************
 **********+++++++++..        ..                        ..++********************
 *************++++++.. ..........                        +++********************
 ******************+++...+++.....                      ..+++********************
 *********************+++++++++....                    ..++*********************
 ***********************++++++++...                     +++*********************
 *************************+++++++..   .            ... .++**********************
 **************************++++++++.......      ......+++***********************
 ****************************+++++++++....      ..++++++************************
 *****************************++++++++++..     ..++++++*************************
 *******************************++++++++++..  ...+++++**************************
 *********************************++++++++.. ...+++++***************************
 ***********************************++++++....+++++*****************************
 ***************************************++++++++********************************
 *******************************************************************************
 *******************************************************************************
 *******************************************************************************
 *******************************************************************************
 *******************************************************************************
 Evaluated to 0.000000

 .......


References
==========
* `LLVM tutorial <http://llvm.org/docs/tutorial/>`_
* `Creating an LLVM Project <http://llvm.org/docs/Projects.html>`_
* `LLVM Makefile Guide <http://llvm.org/docs/MakefileGuide.html#makefile-guide>`_
* `Sphinx Quickstart Template <http://www.llvm.org/docs/SphinxQuickstartTemplate.html>`_
* `Mandelbrot set <http://en.wikipedia.org/wiki/Mandelbrot_set>`_
* `How to set up LLVM-style RTTI for your class hierarchy <http://llvm.org/docs/HowToSetUpLLVMStyleRTTI.html#how-to-set-up-llvm-style-rtti>`_

