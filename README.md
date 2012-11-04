This version is now integrated into LLVM Makefile infrastructure.

Compile How-To:
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
