#!/usr/bin/env python

import sys
import os
import glob
import shutil
import subprocess

def sexe(cmd):
    print "[sexe:%s]" % cmd
    subprocess.call(cmd,shell=True)

def check_remove(name):
    if os.path.exists(name):
        r = raw_input("%s already exists: remove ? " % name)
        if r.lower()[0] == "y":
            if os.path.isdir(name):
                shutil.rmtree(name)
            else:
                os.unlink(name)
        else:
            print "<exiting>"
            sys.exit(-1)
    
def main():
    if len(sys.argv) < 2:
        print "usage: python create_tarball.py <output-name>"
        print "example: "
        print "  >python create_tarball.py visit-vtk-5.8.a"
        sys.exit(1)
    odir = sys.argv[1]
    otar = odir + ".tar.gz"
    check_remove(odir)
    check_remove(otar)
    os.mkdir(odir)
    excludes = [".git",".gitattributes","README.md","create_tarball.py",odir,otar]
    flist = glob.glob("*")
    flist = [f for f in flist if not f in excludes]
    flist = " ".join(flist)
    cmd = "cp -r %s %s" % (flist,odir)
    sexe(cmd)
    cmd = "tar -czf %s %s" % (otar,odir)
    sexe(cmd)
    shutil.rmtree(odir)

if __name__ == "__main__":
    main()
