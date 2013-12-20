#!bin/jse

var x = sys.io.file.open(sys.main, "r"); // r, w, or rw

sys.exec('bash', '-c', 'cat <&'+x);

sys.io.file.close(x);

