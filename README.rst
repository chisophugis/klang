Information About this Project
==============================

Hi, I have taken over custody of this project from its original author
`Journeyer <https://github.com/Journeyer>`_ who had to stop working on it
due to other priorities.

The brilliant idea that Journeyer came up with is to implement the `LLVM
Kaleidoscope tutorial <http://llvm.org/docs/tutorial/>`_ with a design that
mimics Clang's organization. That means that this project will
simultaneously teach you about both LLVM and Clang!
Unfortunately the project wasn't fully completed when I took custody of it,
but Journeyer set a solid foundation to build on. I'm hoping that
interested LLVM newbies will be able to cut their teeth developing on this
foundation.

This is a *really* good way to get acquainted with LLVM and Clang. I
seriously wish I had come up with this idea when I was first starting out
learning about them. Working on this will have a very high learning/effort
ratio if your goal is to become familiar with LLVM and Clang, and there are
lots of easy changes to be made.

I would be very happy to provide guidance for anyone interested in working
on this project, but I cannot commit to lots of coding (I already
have a long queue of things that I'm working on upstream on LLVM trunk).
This project naturally lends itself to newcomers since you can learn as you
go, so if I did all the coding on this project then someone else is losing
the learning/resume opportunity of implementing those parts! (which might
jumpstart a career working on LLVM, perhaps.) Feel free to send me email
(chisophugis@gmail.com or silvas@purdue.edu) if you are looking for
ideas/guidance, or check out our `Issues on github
<https://github.com/chisophugis/klang/issues>`_  (there are currently only
a few issues there, but if you email me I can add plenty of good things to
work on).

Fork the project and send pull requests, or star it to indicate interest! I
will review your code with exactly the same eye that I use to review code
that goes into LLVM trunk, so if you find upstream LLVM development
intimidating or confusing compared to github, this may be an easy way to
get your feet wet before diving into upstream (which you should; we are
nice people!).

And remember that you can always ask on `LLVMdev
<http://lists.cs.uiuc.edu/mailman/listinfo/llvmdev>`_  about LLVM or
`cfe-dev <http://lists.cs.uiuc.edu/mailman/listinfo/cfe-dev>`_  about
Clang.

We really need an example of a *simple* (but well-written) compiler built
on LLVM and harnessing LLVM's capabilities. This project, if (when?)
brought to fruition, will likely be merged into the official LLVM tree (and
you could continue working on it there).

Please contribute! Even filing bugs is helpful! A super-simple way to get
started is to clone the repo and try to build it on your machine; file a
bug if it doesn't work! (Bonus points for pull-requesting a fix!)



Below is the README as it was when I took custody of the project:


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

Klang now supports file input. You can write Kaleidoscope source code in any
file and feed it to Klang! Klang will compile and execute it!::

 $ ./klang mandel.k
 $ ./klang fib.k
 $ ./klang sin.k
 $ ./klang for.k
 $ cat for.k | ./klang
 $ cat for.k | ./klang -

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
 $ ./klang ../../../../../llvm/projects/klang/samples/mandel.k

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

