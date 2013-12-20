#!bin/jse

x = sys.io.file.open(sys.main, "r"); // r, w, or rw

sys.exec('cat <&'+x);

sys.io.file.close(x);

