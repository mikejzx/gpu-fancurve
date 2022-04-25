OUT=gpu-fancurve
CPP=g++
CFLAGS=-std=c++17 -O2
SRC=fan.cpp

.PHONY: install uninstall clean

# Compile target
$(OUT): $(SRC) Makefile
	$(CPP) $(SRC) $(CFLAGS) -o $(OUT)

# Clean objects
clean:
	rm $(OUT)

# Run the install script
install:
	@./install.sh

# Run uninstall script
uninstall:
	@./uninstall.sh

# Run update config script
update_config:
	@./update-config.sh
