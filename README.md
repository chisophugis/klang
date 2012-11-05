
## About
This project has been started from LLVM tutorial and implement the same compiler - kaleidoscope - but in different structure!
The structure of klang is meant to be similar to the one of clang's.

### Purpose of this work
This small project is one of means of studying LLVM and Clang.

## Guide
### Compile How-To:

```
$ cd LLVM_SRC_DIR/projects
$ git clone git://github.com/Journeyer/klang.git
$ cd klang/autoconf
$ ./AutoRegen.sh
```

```
$ cd ../../../
$ (cd LLVM_SRC_DIR)
$ vi configure
```
Put 'projects/klang' under 'projects/sample'.
Put 'klang)       subdirs="$subdirs projects/klang"' under 'sample)       subdirs="$subdirs projects/sample"'.
Be careful for ;; when doing above.
We are mimicing sample.

```
$ cd LLVM_OBJ_DIR
$ ../llvm/configure
$ make
```

Later on, you can compile klang under LLVM_OBJ_DIR/projects/klang


## References
- LLVM tutorial               http://llvm.org/docs/tutorial/
- Creating an LLVM Project    http://llvm.org/docs/Projects.html
- LLVM Makefile Guide         http://llvm.org/docs/MakefileGuide.html#makefile-guide

