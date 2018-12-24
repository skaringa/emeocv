.PHONY: clean, mrproper

PROJECT = emeocv

OBJS = $(addprefix $(OUTDIR)/,\
  Directory.o \
  Config.o \
  ImageProcessor.o \
  ImageInput.o \
  KNearestOcr.o \
  Plausi.o \
  RRDatabase.o \
  main.o \
  )

CC = g++
CFLAGS = -Wno-write-strings -I .

# Check if opencv can be found, if not try to find opencv4 instead
OPENCV-found := $(shell 'pkg-config opencv --cflags' 2>/dev/null)

ifdef OPENCV-found
 	CFLAGS += `pkg-config opencv --cflags`
	LDLIBS = `pkg-config opencv --libs`
else
 	CFLAGS += `pkg-config opencv4 --cflags`
	LDLIBS = `pkg-config opencv4 --libs`
endif

# DEBUG
ifneq ($(RELEASE),true)
CFLAGS += -g -D _DEBUG
OUTDIR = Debug
else
OUTDIR = Release
endif

BIN := $(OUTDIR)/$(PROJECT)

LDLIBS += -lrrd -llog4cpp

SUFFIXES= .cpp .o
.SUFFIXES: $(SUFFIXES) .


all: $(BIN)

$(OUTDIR):
	mkdir $(OUTDIR)

$(OBJS): $(OUTDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN) : $(OUTDIR) $(OBJS)
	$(CC) $(CFLAGS) $(LDFALGS) $(OBJS) $(LDLIBS) -o $(BIN)

.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp

clean:
	rm -rf $(OUTDIR)/*.o

mrproper: clean
	rm -rf $(BIN)
