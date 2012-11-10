
## About
This project has been started from LLVM tutorial and being implemented to be the
same compiler - kaleidoscope - but in different structure!
The structure of klang is meant to be similar to the one of clang's.

### Purpose of this work
This small project is one of the means of studying LLVM and Clang.

## Guide
### Getting the source How-To:

  ```
  $ cd LLVM_SRC_DIR/projects
  $ git clone git://github.com/Journeyer/klang.git
  $ cd klang
  $ git fetch -u git://github.com/Journeyer/klang.git +refs/heads/*:refs/heads/*
  ```

### Branches
- ast ..... ch.2
- ir  ..... ch.3
- jit ..... ch.4
- cf  ..... ch.5
- userop .. ch.6
- master .. up-to-date

### Compile How-To:

  ```
  $ git checkout master
  $ cd autoconf
  $ ./AutoRegen.sh
  ```
note : Makefile infrastructure of LLVM project is applied only after
the branch cf. For the branches before cf, simple Makefile is used.
But to use this Makefile you need to fix Config.mk to your LLVM path.

  ```
  $ cd ../../../
  $ (cd LLVM_SRC_DIR)
  $ vi configure
  Insert items for klang under items for sample as shown below
  ```

This step is the one I am not sure of. If anyone know about this, please share
us! This step is not documented ever!! I just found that this is necessary.
  ```
  diff --git a/../../llvm31/llvm/configure b/configure
  index a4c1592..f7f1757 100755
  --- a/../../llvm31/llvm/configure
  +++ b/configure
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
  ```

  ```
  $ cd LLVM_OBJ_DIR
  $ ../llvm/configure
  $ make
  ```

Later on, you can compile klang under LLVM_OBJ_DIR/projects/klang


## References
  - LLVM tutorial         http://llvm.org/docs/tutorial/
  - Creating an LLVM Project    http://llvm.org/docs/Projects.html
  - LLVM Makefile Guide   http://llvm.org/docs/MakefileGuide.html#makefile-guide

